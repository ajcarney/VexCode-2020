/**
 * @file: ./RobotCode/src/objects/subsystems/chassis.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * @see: chassis.hpp
 *
 * contains implementation for chassis subsytem class
 */
 
#include <cmath>

#include "main.h"


#include "../serial/Logger.hpp"
#include "../position_tracking/PositionTracker.hpp"
#include "chassis.hpp"


int Chassis::num_instances = 0;
std::queue<chassis_action> Chassis::command_queue;
std::vector<int> Chassis::commands_finished;
std::atomic<bool> Chassis::send_lock = ATOMIC_VAR_INIT(false);
std::atomic<bool> Chassis::receive_lock = ATOMIC_VAR_INIT(false);

Motor* Chassis::front_left_drive;
Motor* Chassis::front_right_drive;
Motor* Chassis::back_left_drive;
Motor* Chassis::back_right_drive;

Encoder* Chassis::left_encoder;
Encoder* Chassis::right_encoder;
pros::Imu* Chassis::imu;
double Chassis::width;
double Chassis::gear_ratio;
double Chassis::wheel_diameter;

Chassis::Chassis( Motor &front_left, Motor &front_right, Motor &back_left, Motor &back_right, Encoder &l_encoder, Encoder &r_encoder, pros::Imu Imu, double chassis_width, double gearing /*1*/, double wheel_size /*4.05*/)
{
    front_left_drive = &front_left;
    front_right_drive = &front_right;
    back_left_drive = &back_left;
    back_right_drive = &back_right;
    
    left_encoder = &l_encoder;
    right_encoder = &r_encoder;
    imu = &Imu;
    
    wheel_diameter = wheel_size;
    gear_ratio = gearing;
    width = chassis_width;
    
    if(num_instances == 0 || thread == NULL) {
        thread = new pros::Task( chassis_motion_task, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "chassis_thread");
    }
    
    num_instances += 1;

    front_left_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    front_right_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    back_left_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    back_right_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    front_left_drive->disable_velocity_pid();
    front_right_drive->disable_velocity_pid();
    back_left_drive->disable_velocity_pid();
    back_right_drive->disable_velocity_pid();
    
    front_left_drive->disable_slew();
    front_right_drive->disable_slew();
    back_left_drive->disable_slew();
    back_right_drive->disable_slew();
}




Chassis::~Chassis()
{
    num_instances -= 1;
    if(num_instances == 0) {
        delete thread;
    }
}




void Chassis::chassis_motion_task(void*) {
    while(1) {
        if(command_queue.empty()) {  // delay unitl there is a command in the queue
            pros::delay(10);
            continue;
        }
        
        // take lock and get command
        while ( send_lock.exchange( true ) ); //aquire lock
        chassis_action action = command_queue.front();
        command_queue.pop();
        send_lock.exchange( false ); //release lock
        
        // execute command
        switch(action.command) {
            case e_straight_drive:
                t_straight_drive(action.args);
                break;
            case e_turn:
                t_turn(action.args);
                break;
            case e_drive_to_point: {
                PositionTracker* tracker = PositionTracker::get_instance();
                std::vector<waypoint> waypoints;  // calculate waypoints based on starting position

                long double dx = action.args.setpoint1 - tracker->get_position().x_pos;
                long double dy = action.args.setpoint2 - tracker->get_position().y_pos;
                // convert end coordinates to polar and then calculate waypoints
                long double delta_radius_polar = std::sqrt((std::pow(dx, 2) + std::pow(dy, 2)));
                long double delta_theta_polar = std::atan2(dx, dy);
                
                for(int i=action.args.recalculations + 1; i > 0; i--) {  // calculate additional waypoints, start with last (endpoint) and go down
                    long double radius = (i * delta_radius_polar) / (action.args.recalculations + 1);
                    waypoint recalc_point;
                    recalc_point.dx = radius * std::cos(delta_theta_polar);  // convert back to cartesian coordinates
                    recalc_point.dy = radius * std::sin(delta_theta_polar);
                    recalc_point.radius = radius;
                    recalc_point.dtheta = delta_theta_polar;
                    waypoints.insert(waypoints.begin(), recalc_point);
                }
                
                if(action.args.log_data) {
                    Logger logger;
                    log_entry entry;
                    std::string msg = (
                        "[INFO] " + std::string("CHASSIS_ODOM")
                        + ", Time: " + std::to_string(pros::millis())
                    );
                    int i = 0;
                    for(waypoint point : waypoints) {  // add waypoints to debug message
                        msg += ", waypoint " + std::to_string(i) + ": " + point.get_string();
                    }
                    entry.content = msg;
                    entry.stream = "clog";
                    logger.add(entry);  
                }
                
                int start = pros::millis();
                for(waypoint point : waypoints) {  // move to each generated waypoint
                    if(pros::millis() - start > action.args.timeout) {  // end early if past the timeout point
                        break;
                    }
                    t_move_to_waypoint(action.args, point);
                }
                break;
            }
        }
        
        while ( send_lock.exchange( true ) ); //aquire lock
        commands_finished.push_back(action.command_uid);
        send_lock.exchange( false ); //release lock
    }
}




void Chassis::t_straight_drive(chassis_params args) {
    PositionTracker* tracker = PositionTracker::get_instance();
    Configuration* config = Configuration::get_instance();
    
    double kP_l = 38;
    double kI_l = .0125;
    double kD_l = 4;
    double I_max_l = INT32_MAX;
    
    double kP_r = kP_l;
    double kI_r = kI_l;
    double kD_r = kD_l;
    double I_max_r = I_max_l;
    
    front_left_drive->tare_encoder();
    front_right_drive->tare_encoder();
    back_left_drive->tare_encoder();
    back_right_drive->tare_encoder();
    
    int r_id = right_encoder->get_unique_id();
    int l_id = left_encoder->get_unique_id();
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    
    front_left_drive->move(0);
    front_right_drive->move(0);
    back_left_drive->move(0);
    back_right_drive->move(0);
    
    double integral_l = 0;
    double integral_r = 0;
    double prev_error_l = 0;
    double prev_error_r = 0;
    double prev_l_voltage = 0;
    double prev_r_voltage = 0;
    
    double prev_l_encoder = std::get<0>(Sensors::get_average_encoders(l_id, r_id));
    double prev_r_encoder = std::get<1>(Sensors::get_average_encoders(l_id, r_id));
    // double prev_angle = imu->get_heading();
    // double prev_angle = 0;
    double relative_angle = 0;
    
    int timeout = pros::millis() + args.timeout;
    bool settled = false;
    bool use_integral_l = true;
    bool use_integral_r = true;
    
    int time = pros::millis();

    do {
        // pid distance controller
        double error_l = args.setpoint1 - std::get<0>(Sensors::get_average_encoders(l_id, r_id));
        double error_r = args.setpoint1 - std::get<1>(Sensors::get_average_encoders(l_id, r_id));

        if ( std::abs(integral_l) > I_max_l || !use_integral_l) {
            integral_l = 0;  // reset integral if greater than max allowable value
            use_integral_l = false;
        } else {
            int dt = pros::millis() - time;
            time = pros::millis();
            integral_l = integral_l + (error_l * dt);
        }
        
        if ( std::abs(integral_r) > I_max_l || !use_integral_r) {
            integral_r = 0;  // reset integral if greater than max allowable value
            use_integral_r = false;
        } else {
            int dt = pros::millis() - time;
            time = pros::millis();
            integral_r = integral_r + (error_r * dt);
        }
        
        time = pros::millis();
        
        double derivative_l = error_l - prev_error_l;
        double derivative_r = error_r - prev_error_r;
        prev_error_l = error_l;
        prev_error_r = error_r;
        
        int left_voltage = (kP_l * error_l) + (kI_l * integral_l) + (kD_l * derivative_l);
        int right_voltage = (kP_r * error_r) + (kI_r * integral_r) + (kD_r * derivative_r);

        
        // p controller heading correction
        // double delta_l = std::get<0>(Sensors::get_average_encoders(l_id, r_id)) - prev_l_encoder;
        // double delta_r = std::get<1>(Sensors::get_average_encoders(l_id, r_id)) - prev_r_encoder;
        // double delta_theta = calc_delta_theta(prev_angle, delta_l, delta_r);
        // prev_angle = prev_angle + delta_theta;
        double delta_theta = tracker->to_degrees(tracker->get_delta_theta_rad());
        relative_angle = relative_angle + delta_theta;
        double heading_error = args.setpoint2 - relative_angle;
        // std::cout << "delta_theta: " << delta_theta << " | prev_anlge: " << prev_angle << " | relative angle: " << relative_angle << " | heading_error: " << heading_error << "\n";
        if(args.correct_heading) {
            left_voltage += 0 * heading_error;  
            right_voltage -= 0 * heading_error;
        }
        
        // cap voltage to max voltage with regard to velocity
        if ( std::abs(left_voltage) > args.max_voltage ) {
            left_voltage = left_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }
        if ( std::abs(right_voltage) > args.max_voltage ) {
            right_voltage = right_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }

        if ( args.log_data ) {
            Logger logger;
            log_entry entry;
            entry.content = (
                "[INFO] " + std::string("CHASSIS_PID")
                + ", Time: " + std::to_string(pros::millis())
                + ", Actual_Vol1: " + std::to_string(front_left_drive->get_actual_voltage())
                + ", Actual_Vol2: " + std::to_string(front_right_drive->get_actual_voltage())
                + ", Actual_Vol3: " + std::to_string(back_left_drive->get_actual_voltage())
                + ", Actual_Vol4: " + std::to_string(back_right_drive->get_actual_voltage())
                + ", Slew: " + std::to_string(args.motor_slew)
                + ", Brake: " + std::to_string(front_left_drive->get_brake_mode())
                + ", Gear: " + std::to_string(front_left_drive->get_gearset())
                + ", I_max: " + std::to_string(I_max_l)
                + ", I: " + std::to_string(integral_l)
                + ", kD: " + std::to_string(kD_l)
                + ", kI: " + std::to_string(kI_l)
                + ", kP: " + std::to_string(kP_l)
                + ", Position_Sp: " + std::to_string(args.setpoint1)
                + ", position_l: " + std::to_string(std::get<0>(Sensors::get_average_encoders(l_id, r_id)))
                + ", position_r: " + std::to_string(std::get<1>(Sensors::get_average_encoders(l_id, r_id)))                
                + ", Heading_Sp: " + std::to_string(args.setpoint2)
                + ", Relative_Heading: " + std::to_string(relative_angle)
                + ", Actual_Vel1: " + std::to_string(front_left_drive->get_actual_velocity())
                + ", Actual_Vel2: " + std::to_string(front_right_drive->get_actual_velocity())
                + ", Actual_Vel3: " + std::to_string(back_left_drive->get_actual_velocity())
                + ", Actual_Vel4: " + std::to_string(back_right_drive->get_actual_velocity())
            );
            entry.stream = "clog";
            logger.add(entry);            
        }

        
        front_left_drive->set_voltage(left_voltage);
        front_right_drive->set_voltage(right_voltage);
        back_left_drive->set_voltage(left_voltage);
        back_right_drive->set_voltage(right_voltage);
        
        prev_l_voltage = left_voltage;
        prev_r_voltage = right_voltage;
        
        // // settled is when error is almost zero and voltage is minimal for both sides
        if ( (std::abs(error_l) < 5) && (std::abs(error_r) < 5) && std::abs(left_voltage) < 1000 && std::abs(right_voltage) < 1000) {        
            break; // end before timeout 
        }

        pros::delay(10);
    } while ( pros::millis() < timeout ); 
    
    front_left_drive->set_voltage(0);
    front_right_drive->set_voltage(0);
    back_left_drive->set_voltage(0);
    back_right_drive->set_voltage(0);
    
    right_encoder->forget_position(r_id);  // free up space in the encoders log
    left_encoder->forget_position(l_id);
}




void Chassis::t_turn(chassis_params args) {
    PositionTracker* tracker = PositionTracker::get_instance();
    Configuration* config = Configuration::get_instance();
    
    double kP = 10;
    double kI = 0;
    double kD = 0;
    double I_max = INT32_MAX;
    
    front_left_drive->tare_encoder();
    front_right_drive->tare_encoder();
    back_left_drive->tare_encoder();
    back_right_drive->tare_encoder();
    
    int r_id = right_encoder->get_unique_id();
    int l_id = left_encoder->get_unique_id();
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    
    front_left_drive->move(0);
    front_right_drive->move(0);
    back_left_drive->move(0);
    back_right_drive->move(0);
    
    double integral = 0;
    double prev_error = 0;
    double prev_l_voltage = 0;
    double prev_r_voltage = 0;
    
    double prev_l_encoder = std::get<0>(Sensors::get_average_encoders(l_id, r_id));
    double prev_r_encoder = std::get<1>(Sensors::get_average_encoders(l_id, r_id));
    // double intitial_angle = imu->get_heading();
    // double prev_angle = imu->get_heading();
    // double prev_angle = 0;
    double relative_angle = 0;

    int timeout = pros::millis() + 5000;
    bool settled = false;
    bool use_integral = true;
    
    int time = pros::millis();

    do {
        // pid angle controller
        // double delta_l = std::get<0>(Sensors::get_average_encoders(l_id, r_id)) - prev_l_encoder;
        // double delta_r = std::get<1>(Sensors::get_average_encoders(l_id, r_id)) - prev_r_encoder;
        // double delta_theta = calc_delta_theta(prev_angle, delta_l, delta_r);
        // prev_angle = prev_angle + delta_theta;
        double delta_theta = tracker->to_degrees(tracker->get_delta_theta_rad());
        relative_angle = relative_angle + delta_theta;
        double error = args.setpoint1 - relative_angle;
        // std::cout << "delta_theta: " << delta_theta << " | prev_anlge: " << prev_angle << " | relative angle: " << relative_angle << " | heading_error: " << error << "\n";

        
        if ( std::abs(integral) > I_max || !use_integral) {
            integral = 0;  //reset integral if error is too small or greater than max value
            use_integral = false;
        } else {
            int dt = pros::millis() - time;
            integral = integral + (error * dt);
        }
        time = pros::millis();
        
        
        double derivative = error - prev_error;
        prev_error = error;
        
        int right_voltage;
        int left_voltage;
        if(args.setpoint1 > 0) {  // turn right
            right_voltage = -1 * ((kP * error) + (kI * integral) + (kD * derivative));
            left_voltage = (kP * error) + (kI * integral) + (kD * derivative);
        } else {  // turn left
            right_voltage = (kP * error) + (kI * integral) + (kD * derivative);
            left_voltage = -1 * ((kP * error) + (kI * integral) + (kD * derivative));
        }
        
        
        // cap voltage to max voltage with regard to velocity
        if ( std::abs(left_voltage) > args.max_voltage ) {
            left_voltage = left_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }
        if ( std::abs(right_voltage) > args.max_voltage ) {
            right_voltage = right_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }

        if ( args.log_data ) {
            Logger logger;
            log_entry entry;
            entry.content = (
                "[INFO] " + std::string("CHASSIS_PID")
                + ", Time: " + std::to_string(pros::millis())
                + ", Actual_Vol1: " + std::to_string(front_left_drive->get_actual_voltage())
                + ", Actual_Vol2: " + std::to_string(front_right_drive->get_actual_voltage())
                + ", Actual_Vol3: " + std::to_string(back_left_drive->get_actual_voltage())
                + ", Actual_Vol4: " + std::to_string(back_right_drive->get_actual_voltage())
                + ", Slew: " + std::to_string(args.motor_slew)
                + ", Brake: " + std::to_string(front_left_drive->get_brake_mode())
                + ", Gear: " + std::to_string(front_left_drive->get_gearset())
                + ", I_max: " + std::to_string(I_max)
                + ", I: " + std::to_string(integral)
                + ", kD: " + std::to_string(kD)
                + ", kI: " + std::to_string(kI)
                + ", kP: " + std::to_string(kP)
                + ", Position_Sp: " + std::to_string(0)
                + ", position_l: " + std::to_string(std::get<0>(Sensors::get_average_encoders(l_id, r_id)))
                + ", position_r: " + std::to_string(std::get<1>(Sensors::get_average_encoders(l_id, r_id)))
                + ", Heading_Sp: " + std::to_string(args.setpoint1)
                + ", Relative_Heading: " + std::to_string(relative_angle)
                + ", Actual_Vel1: " + std::to_string(front_left_drive->get_actual_velocity())
                + ", Actual_Vel2: " + std::to_string(front_right_drive->get_actual_velocity())
                + ", Actual_Vel3: " + std::to_string(back_left_drive->get_actual_velocity())
                + ", Actual_Vel4: " + std::to_string(back_right_drive->get_actual_velocity())
            );
            entry.stream = "clog";
            logger.add(entry);            
        }

        
        front_left_drive->set_voltage(left_voltage);
        front_right_drive->set_voltage(-right_voltage);
        back_left_drive->set_voltage(left_voltage);
        back_right_drive->set_voltage(-right_voltage);
        
        prev_l_voltage = left_voltage;
        prev_r_voltage = right_voltage;
        
        // // settled is when error is almost zero and voltage is minimal
        if ( ( std::abs(error) < 1 ) && std::abs(left_voltage) < 1000) {        
            break; // end before timeout 
        }

        pros::delay(10);
    } while ( pros::millis() < timeout ); 
    
    front_left_drive->set_voltage(0);
    front_right_drive->set_voltage(0);
    back_left_drive->set_voltage(0);
    back_right_drive->set_voltage(0);
    
    right_encoder->forget_position(r_id);  // free up space in the encoders log
    left_encoder->forget_position(l_id);
}



void Chassis::t_move_to_waypoint(chassis_params args, waypoint point) {
    PositionTracker* tracker = PositionTracker::get_instance();
    
    // delta theta is bounded by [-pi, pi], re map it to [0,pi]
    if(point.dtheta < -M_PI) {
        point.dtheta += M_PI;
    }
    
    // current angle is bounded by [-pi, pi] re map it to [0, pi]
    long double current_angle = tracker->get_heading_rad();
    if(current_angle < -M_PI) {
        current_angle += M_PI;
    }
    
    long double to_turn = current_angle - point.dtheta;  // change in robot angle
    
    if(to_turn > (M_PI / 2)) {  // find minimal angle change and direction of change [-PI/2, PI/2]
        to_turn = -M_PI + to_turn;  // give negative value to turn left to point
    }
    
    to_turn = tracker->to_degrees(to_turn);
    long double to_drive = tracker->to_encoder_ticks(point.radius, wheel_diameter);
    
    chassis_params turn_args;
    turn_args.setpoint1 = to_turn;
    turn_args.max_voltage = args.max_voltage;
    turn_args.timeout = 15000; // TODO: add time estimation
    turn_args.motor_slew = args.motor_slew;
    turn_args.log_data = args.log_data;
    
    chassis_params drive_straight_args;
    drive_straight_args.setpoint1 = to_drive;
    drive_straight_args.setpoint2 = 0;
    drive_straight_args.max_voltage = args.max_voltage;
    drive_straight_args.timeout = 15000;
    drive_straight_args.motor_slew = args.motor_slew;
    drive_straight_args.correct_heading = true;
    drive_straight_args.log_data = args.log_data;
    
    if(args.log_data) {
        Logger logger;
        log_entry entry;
        entry.content = (
            "[INFO] " + std::string("CHASSIS_ODOM")
            + ", Time: " + std::to_string(pros::millis())
            + ", Waypoint: " + point.get_string()
            + ", ToTurn: " + std::to_string(to_turn)
            + ", ToDrive: " + std::to_string(to_drive)
        );
        entry.stream = "clog";
        logger.add(entry);  
    }
    
    // perform movement
    t_turn(turn_args);
    pros::delay(100);  // add delay for extra settling
    t_straight_drive(drive_straight_args);
}



int Chassis::straight_drive(double encoder_ticks, int relative_heading /*0*/, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool correct_heading /*true*/, bool slew /*false*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = encoder_ticks;
    args.setpoint2 = relative_heading;
    args.max_voltage = max_voltage;
    args.timeout = timeout;
    args.motor_slew = slew;
    args.correct_heading = correct_heading;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(encoder_ticks) + 1) + max_voltage + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_straight_drive};
    while ( send_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    send_lock.exchange( false ); //release lock
    
    if(!asynch) {
        while(std::find(commands_finished.begin(), commands_finished.end(), uid) == commands_finished.end()) {
            pros::delay(10);
        }
        commands_finished.erase(std::remove(commands_finished.begin(), commands_finished.end(), uid), commands_finished.end()); 
    }
    
    return uid;
}


int Chassis::turn_right(double degrees, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool slew /*false*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = degrees;
    args.max_voltage = max_voltage;
    args.timeout = timeout;
    args.motor_slew = slew;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(degrees) + 1) + max_voltage + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_turn};
    while ( send_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    send_lock.exchange( false ); //release lock
    
    if(!asynch) {
        while(std::find(commands_finished.begin(), commands_finished.end(), uid) == commands_finished.end()) {
            pros::delay(10);
        }
        commands_finished.erase(std::remove(commands_finished.begin(), commands_finished.end(), uid), commands_finished.end()); 
    }
    
    return uid;
}



int Chassis::turn_left(double degrees, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool slew /*false*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = -degrees;
    args.max_voltage = max_voltage;
    args.timeout = timeout;
    args.motor_slew = slew;
    args.log_data = log_data;

    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(degrees) + 1) + max_voltage + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_turn};
    while ( send_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    send_lock.exchange( false ); //release lock
    
    if(!asynch) {
        while(std::find(commands_finished.begin(), commands_finished.end(), uid) == commands_finished.end()) {
            pros::delay(10);
        }
        commands_finished.erase(std::remove(commands_finished.begin(), commands_finished.end(), uid), commands_finished.end()); 
    }
    
    return uid;
}




/**
 * sets scaled voltage of each drive motor
 */
void Chassis::move( int voltage )
{
    front_left_drive->move(voltage);
    front_right_drive->move(voltage);
    back_left_drive->move(voltage);
    back_right_drive->move(voltage);
}




double Chassis::calc_delta_theta(double prev_angle, double delta_l, double delta_r) {
    // double imu_reading = std::fmod(imu->get_heading() + 360.0, 360); // scale imu to 0, 360
    // double delta_imu = (imu_reading - prev_angle);
    // if(delta_imu > 180) {  // find least distance between angles
    //     delta_imu = 360 - delta_imu;
    // } else if (delta_imu < -180) {
    //     delta_imu = 360 + delta_imu;
    // }
    
    // std::cout << delta_imu << " | " << imu_reading << "\n";

    long double circumference = width * M_PI;                       // circumference of turning arc 
    long double inches_per_tick = (M_PI * wheel_diameter) / 360.0;  // inches per tick = circumference of wheel / num encoder ticks per revolution
    long double inches_turned = inches_per_tick * (delta_l - delta_r);
    double delta_encoder_degrees = (360 * inches_turned) / circumference;
    
    // double delta_theta = (.6 * delta_imu) + (.4 * delta_encoder_degrees);
    double delta_theta = delta_encoder_degrees;
    // double theta = (.6 * imu_reading) + (.4 * (delta_encoder_degrees + prev_angle));

    return delta_theta;
}


/**
 * sets a new brakemode for each drive motor
 */
void Chassis::set_brake_mode( pros::motor_brake_mode_e_t new_brake_mode )
{
    front_left_drive->set_brake_mode(new_brake_mode);
    front_right_drive->set_brake_mode(new_brake_mode);
    back_left_drive->set_brake_mode(new_brake_mode);
    back_right_drive->set_brake_mode(new_brake_mode);    
}




/**
 * sets all chassis motors to the opposite direction that they were facing
 * ie. reversed is now normal and normal is now reversed
 */
void Chassis::change_direction() 
{
    front_left_drive->reverse_motor();
    front_right_drive->reverse_motor();
    back_left_drive->reverse_motor();
    back_right_drive->reverse_motor();
}




/**
 * sets slew to enabled for each motor
 * sets the rate of the slew to the rate parameter
 */
void Chassis::enable_slew( int rate /*120*/ )
{
    front_left_drive->enable_slew();
    front_right_drive->enable_slew();
    back_left_drive->enable_slew();
    back_right_drive->enable_slew();
    
    front_left_drive->set_slew(rate);
    front_right_drive->set_slew(rate);
    back_left_drive->set_slew(rate);
    back_right_drive->set_slew(rate);
}




/**
 * sets slew to disabled for each motor
 */
void Chassis::disable_slew( ) 
{
    front_left_drive->disable_slew();
    front_right_drive->disable_slew();
    back_left_drive->disable_slew();
    back_right_drive->disable_slew();
}
