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


Profile::Profile() {
    
}


Profile::~Profile() {
    
}


void Profile::generate_profile(const std::function<double(double)>& acceleration_equation, const std::function<double(double)>& deceleration_equation, int ticks_accel, int ticks_decel, int max_velocity, int min_velocity) {
    ticks_to_accelerate = ticks_accel;
    ticks_to_decelerate = ticks_decel;
    
    acceleration_profile.clear();
    deceleration_profile.clear();
    
    for(int i = 0; i < ticks_to_accelerate; i++) {
        double value = acceleration_equation(i);
        double value_norm = (value - min_velocity) / (max_velocity - min_velocity);  // normalize between [0,1]
        acceleration_profile.push_back(value_norm);
    }
    
    for(int i = 0; i < ticks_to_decelerate; i++) {
        double value = deceleration_equation(i);
        double value_norm = (value - min_velocity) / (max_velocity - min_velocity);  // normalize between [0,1]
        deceleration_profile.push_back(value_norm);
    }
}


bool Profile::is_generated() {
    return (!acceleration_profile.empty() && !deceleration_profile.empty());
}


double Profile::get_target_velocity(int current_enc_value, int max_enc_value, int max_velocity) {
    double norm_velocity;
    if(current_enc_value < 0) {
        current_enc_value = 0;
    }
    std::cout << current_enc_value << " " << max_enc_value << " " << max_velocity << " " << ticks_to_accelerate << " " << ticks_to_decelerate << " " << acceleration_profile.size() << " " << deceleration_profile.size() << "\n";
    if(max_enc_value > (ticks_to_accelerate + ticks_to_decelerate)) {
        if(current_enc_value < ticks_to_accelerate) {                         // acceleration segment
            norm_velocity = acceleration_profile.at(current_enc_value);
        } else if(max_enc_value - current_enc_value < ticks_to_decelerate) {  // deceleration segment
            int i = (current_enc_value - max_enc_value) + ticks_to_decelerate;
            if(i < deceleration_profile.size()) {
                norm_velocity = deceleration_profile.at(i);
            } else {  // out of range of controller, so set to 0
                norm_velocity = 0;
            }
            
        } else {                                                              // constant max velocity segment
            norm_velocity = 1;
        }
    } else {  // will not be able to follow entire profile
        int i = (current_enc_value - max_enc_value) + ticks_to_decelerate;
        if(i < deceleration_profile.size()) {
            norm_velocity = deceleration_profile.at(i);
        } else {  // out of range of controller, so set to 0
            norm_velocity = 0;
        }
    }
    
    double target_velocity = norm_velocity * max_velocity;
    std::cout << target_velocity << " " << current_enc_value << " " << max_enc_value << " " << max_velocity << " " << ticks_to_accelerate << " " << ticks_to_decelerate << " " << acceleration_profile.size() << " " << deceleration_profile.size() << "\n";

    return target_velocity;
}




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

Profile Chassis::profile_1;


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
    
    front_left_drive->set_motor_mode(e_voltage);
    front_right_drive->set_motor_mode(e_voltage);
    back_left_drive->set_motor_mode(e_voltage);
    back_right_drive->set_motor_mode(e_voltage);
    
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


void Chassis::generate_profiles() {
    if(!profile_1.is_generated()) {
        profile_1.generate_profile(
            [](double n) -> double { return 183335300 + ((16.29262 - 183335300) / (1 + std::pow((n / 5807375), 1.381135))); },
            [](double n) -> double { return -14.87879 + ((199.9947 - -14.87879) / (1 + std::pow((n / 216.5658), 1.756448))); },  // 700 endpoint
            // [](double n) -> double { return -718.7411 + ((200.0491 - -718.7411) / (1 + std::pow((n / 3436.08), 0.8049193))); },  // 700 endpoint
            // [](double n) -> double { return -39.01548 + ((195.369 - -39.01548) / (1 + std::pow((n / 978.0565), 2.266577))); },  // 1700 endpoint
            // [](double n) -> double { return (-0.2 * n + 1000); },  // 1000 endpoint
            250,
            700,
            200,
            0
        );
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
            case e_pid_straight_drive:
                t_pid_straight_drive(action.args);
                break;
            case e_profiled_straight_drive:
                t_profiled_straight_drive(action.args);
                break;
            case e_turn:
                t_turn(action.args);
                break;
            case e_drive_to_point: {
                // PositionTracker* tracker = PositionTracker::get_instance();
                std::vector<waypoint> waypoints;  // calculate waypoints based on starting position

                long double dx = action.args.setpoint1 - PositionTracking::get_position().x_pos;
                long double dy = action.args.setpoint2 - PositionTracking::get_position().y_pos;
                // convert end coordinates to polar and then calculate waypoints
                long double delta_radius_polar = std::sqrt((std::pow(dx, 2) + std::pow(dy, 2)));
                long double delta_theta_polar = std::atan2(dx, dy);
                
                for(int i=action.args.recalculations + 1; i > 0; i--) {  // calculate additional waypoints, start with last endpoint and go down
                    long double radius = (i * delta_radius_polar) / (action.args.recalculations + 1);
                    waypoint recalc_point;
                    recalc_point.x = action.args.setpoint1 + (radius * std::cos(delta_theta_polar));  // intital x + dx
                    recalc_point.y = action.args.setpoint2 + (radius * std::sin(delta_theta_polar));  // initial y + dy
                    recalc_point.dx = radius * std::cos(delta_theta_polar);
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
            } case e_turn_to_point: {
                // PositionTracker* tracker = PositionTracker::get_instance();

                long double dx = action.args.setpoint1 - PositionTracking::get_position().x_pos;
                long double dy = action.args.setpoint2 - PositionTracking::get_position().y_pos;
                
                // convert end coordinates to polar to find the change in angle
                long double dtheta = std::atan2(dx, dy);
                
                // delta theta is bounded by [-pi, pi], re map it to [0,pi]
                if(dtheta < -M_PI) {
                    dtheta += M_PI;
                }
                
                // current angle is bounded by [-pi, pi] re map it to [0, pi]
                long double current_angle = PositionTracking::get_heading_rad();
                if(current_angle < -M_PI) {
                    current_angle += M_PI;
                }
                
                // calculate how much the robot needs to turn to be at the angle
                long double to_turn = current_angle - dtheta;  // change in robot angle
                
                if(to_turn > (M_PI / 2)) {  // find minimal angle change and direction of change [-PI/2, PI/2]
                    to_turn = -M_PI + to_turn;  // give negative value to turn left to point
                }
                
                to_turn = PositionTracking::to_degrees(to_turn);
                
                // set up turn
                chassis_params turn_args;
                turn_args.setpoint1 = to_turn;
                turn_args.max_voltage = action.args.max_voltage;
                turn_args.timeout = action.args.timeout; // TODO: add time estimation
                turn_args.motor_slew = action.args.motor_slew;
                turn_args.log_data = action.args.log_data;

                if(action.args.log_data) {
                    Logger logger;
                    log_entry entry;
                    std::string msg = (
                        "[INFO] " + std::string("CHASSIS_ODOM")
                        + ", Time: " + std::to_string(pros::millis())
                        + ", turning: " + std::to_string(to_turn)
                    );
                    entry.content = msg;
                    entry.stream = "clog";
                    logger.add(entry);  
                }
                
                // perform turn
                t_turn(turn_args);
                
                break;
            } case e_turn_to_angle: {
                // PositionTracker* tracker = PositionTracker::get_instance();
                                
                // current angle is bounded by [-pi, pi] re map it to [0, pi]
                long double current_angle = PositionTracking::get_heading_rad();
                if(current_angle < -M_PI) {
                    current_angle += M_PI;
                }
                
                // calculate how much the robot needs to turn to be at the angle
                long double to_turn = current_angle - action.args.setpoint1;  // change in robot angle
                
                if(to_turn > (M_PI / 2)) {  // find minimal angle change and direction of change [-PI/2, PI/2]
                    to_turn = -M_PI + to_turn;  // give negative value to turn left to point
                }
                
                to_turn = PositionTracking::to_degrees(to_turn);
                
                // set up turn
                chassis_params turn_args;
                turn_args.setpoint1 = to_turn;
                turn_args.max_voltage = action.args.max_voltage;
                turn_args.timeout = action.args.timeout; // TODO: add time estimation
                turn_args.motor_slew = action.args.motor_slew;
                turn_args.log_data = action.args.log_data;

                if(action.args.log_data) {
                    Logger logger;
                    log_entry entry;
                    std::string msg = (
                        "[INFO] " + std::string("CHASSIS_ODOM")
                        + ", Time: " + std::to_string(pros::millis())
                        + ", turning: " + std::to_string(to_turn)
                    );
                    entry.content = msg;
                    entry.stream = "clog";
                    logger.add(entry);  
                }
                
                // perform turn
                t_turn(turn_args);
                
                break;
            }
        }
        
        while ( send_lock.exchange( true ) ); //aquire lock
        commands_finished.push_back(action.command_uid);
        send_lock.exchange( false ); //release lock
    }
}




void Chassis::t_pid_straight_drive(chassis_params args) {
    // PositionTracker* tracker = PositionTracker::get_instance();
    Configuration* config = Configuration::get_instance();
    
    double kP_l = 45;
    double kI_l = 0.00140;
    double kD_l = 26;
    double I_max_l = INT32_MAX;
    
    double kP_r = 33;
    double kI_r = 0.00105;
    double kD_r = 33;
    double I_max_r = INT32_MAX;
    
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

    long double relative_angle = 0;
    long double integral_heading = 0;
    double prev_heading_error = 0;
    
    int timeout = pros::millis() + args.timeout;
    bool settled = false;
    bool use_integral_l = true;
    bool use_integral_r = true;
    
    int time = pros::millis();

    do {
        int dt = pros::millis() - time;
        // pid distance controller
        double error_l = args.setpoint1 - std::get<0>(Sensors::get_average_encoders(l_id, r_id));
        double error_r = args.setpoint1 - std::get<1>(Sensors::get_average_encoders(l_id, r_id));

        if ( std::abs(integral_l) > I_max_l || !use_integral_l) {
            integral_l = 0;  // reset integral if greater than max allowable value
            use_integral_l = false;
        } else {
            integral_l = integral_l + (error_l * dt);
        }
        
        if ( std::abs(integral_r) > I_max_l || !use_integral_r) {
            integral_r = 0;  // reset integral if greater than max allowable value
            use_integral_r = false;
        } else {
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
        double delta_theta = PositionTracking::to_degrees(PositionTracking::get_delta_theta_rad());
        relative_angle = relative_angle + delta_theta;
        double heading_error = args.setpoint2 - relative_angle;
        integral_heading = integral_heading + (heading_error * dt);
        double d_error = heading_error - prev_heading_error;
        prev_heading_error = heading_error;
        // std::cout << "delta_theta: " << delta_theta << " | prev_anlge: " << prev_angle << " | relative angle: " << relative_angle << " | heading_error: " << heading_error << "\n";
        if(args.correct_heading && heading_error > 0.00001) {  // veering right
            // int voltage = (1400 * heading_error) + (1.011 * integral_heading) + (95 * d_error);
            int voltage = 3300;
            left_voltage -= voltage;  
            right_voltage += voltage;
        } else if ( args.correct_heading && heading_error < -0.00001) {  // veering left
            int voltage = 3300;
            left_voltage += voltage;  
            right_voltage -= voltage;
        }
        
        // cap voltage to max voltage with regard to velocity
        if ( std::abs(left_voltage) > args.max_voltage ) {
            left_voltage = left_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }
        if ( std::abs(right_voltage) > args.max_voltage ) {
            right_voltage = right_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }
        
        time = pros::millis();

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
        if ( (std::abs(error_l) < 5) && (std::abs(error_r) < 5) && std::abs(left_voltage) < 2000 && std::abs(right_voltage) < 2000) {        
            //break; // end before timeout 
        }

        pros::delay(10);
    } while ( pros::millis() < args.timeout ); 
    
    front_left_drive->set_voltage(0);
    front_right_drive->set_voltage(0);
    back_left_drive->set_voltage(0);
    back_right_drive->set_voltage(0);
    
    front_left_drive->enable_driver_control();
    front_right_drive->enable_driver_control();
    back_left_drive->enable_driver_control();
    back_right_drive->enable_driver_control();
    
    right_encoder->forget_position(r_id);  // free up space in the encoders log
    left_encoder->forget_position(l_id);
}




void Chassis::t_profiled_straight_drive(chassis_params args) {
    // PositionTracker* tracker = PositionTracker::get_instance();
    Configuration* config = Configuration::get_instance();
    
    front_left_drive->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    front_right_drive->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    back_left_drive->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    back_right_drive->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    
    double kP = 0;
    double kI = 0;
    double kD = 0;
    double I_max = INT32_MAX;
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    
    front_left_drive->set_motor_mode(e_builtin_velocity_pid);
    front_right_drive->set_motor_mode(e_builtin_velocity_pid);
    back_left_drive->set_motor_mode(e_builtin_velocity_pid);
    back_right_drive->set_motor_mode(e_builtin_velocity_pid);
    
    front_left_drive->move(0);
    front_right_drive->move(0);
    back_left_drive->move(0);
    back_right_drive->move(0);
    
    front_left_drive->tare_encoder();
    front_right_drive->tare_encoder();
    back_left_drive->tare_encoder();
    back_right_drive->tare_encoder();
    
    int r_id = right_encoder->get_unique_id();
    int l_id = left_encoder->get_unique_id();
    
    long double relative_angle = 0;
    long double abs_angle = PositionTracking::to_degrees(PositionTracking::get_heading_rad());
    long double prev_abs_angle = abs_angle;
    long double integral = 0;
    double prev_error = 0;
    bool use_integral = true;
    int time = pros::millis();
    
    do {
        double velocity_l = args.profile.get_target_velocity(std::get<0>(Sensors::get_average_encoders(l_id, r_id)), args.setpoint1, args.max_velocity);
        double velocity_r = args.profile.get_target_velocity(std::get<1>(Sensors::get_average_encoders(l_id, r_id)), args.setpoint1, args.max_velocity);
        
        if(std::abs(std::get<0>(Sensors::get_average_encoders(l_id, r_id)) - args.setpoint1) < 2) {
            velocity_l = 0;
        }
        
        if(std::abs(std::get<1>(Sensors::get_average_encoders(l_id, r_id)) - args.setpoint1) < 2) {
            velocity_r = 0;
        }
        
        int dt = pros::millis() - time;
        
        abs_angle = PositionTracking::to_degrees(PositionTracking::get_heading_rad());
        long double delta_theta = abs_angle - prev_abs_angle;
        relative_angle = relative_angle + delta_theta;
        prev_abs_angle = abs_angle;
        
        long double error = args.setpoint2 - relative_angle;
        std::cout << "relative angle: " << relative_angle << " | dtheta: " << delta_theta << "\n";
        if ( std::abs(integral) > I_max || !use_integral) {
            integral = 0;  // reset integral if greater than max allowable value
            use_integral = false;
        } else {
            integral = integral + (error * dt);
        }
        
        double derivative = error - prev_error;
        prev_error = error;
        
        time = pros::millis();
        
        
        int velocity_correction = (kP * error) + (kI * integral) + (kD * derivative);
        if(args.correct_heading && error > 0.000001) {  // veering right
            velocity_l -= velocity_correction;  
            velocity_r += velocity_correction;
        } else if ( args.correct_heading && error < -0.000001) {  // veering left
            velocity_l += velocity_correction;  
            velocity_r -= velocity_correction;
        }
        
        
        // cap velocity to max velocity with regard to velocity
        if ( std::abs(velocity_l) > args.max_velocity ) {
            velocity_l = velocity_l > 0 ? args.max_velocity : -args.max_velocity;
        }
        if ( std::abs(velocity_r) > args.max_velocity ) {
            velocity_r = velocity_r > 0 ? args.max_velocity : -args.max_velocity;
        }
                
        std::cout << velocity_l << " " << velocity_r << "\n";    
        
        front_left_drive->move_velocity(velocity_l);
        front_right_drive->move_velocity(velocity_r);
        back_left_drive->move_velocity(velocity_l);
        back_right_drive->move_velocity(velocity_r);

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
        pros::delay(10);
    } while ( pros::millis() < 12000 ); 
    
    front_left_drive->set_motor_mode(e_voltage);
    front_right_drive->set_motor_mode(e_voltage);
    back_left_drive->set_motor_mode(e_voltage);
    back_right_drive->set_motor_mode(e_voltage);
    
    front_left_drive->set_voltage(0);
    front_right_drive->set_voltage(0);
    back_left_drive->set_voltage(0);
    back_right_drive->set_voltage(0);
    
    front_left_drive->enable_driver_control();
    front_right_drive->enable_driver_control();
    back_left_drive->enable_driver_control();
    back_right_drive->enable_driver_control();
    
    front_left_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    front_right_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    back_left_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    back_right_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    right_encoder->forget_position(r_id);  // free up space in the encoders log
    left_encoder->forget_position(l_id);
}




void Chassis::t_turn(chassis_params args) {
    // PositionTracker* tracker = PositionTracker::get_instance();
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
        double delta_theta = PositionTracking::to_degrees(PositionTracking::get_delta_theta_rad());
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
    
    front_left_drive->enable_driver_control();
    front_right_drive->enable_driver_control();
    back_left_drive->enable_driver_control();
    back_right_drive->enable_driver_control();
    
    right_encoder->forget_position(r_id);  // free up space in the encoders log
    left_encoder->forget_position(l_id);
}



void Chassis::t_move_to_waypoint(chassis_params args, waypoint point) {
    // PositionTracker* tracker = PositionTracker::get_instance();
    
    // delta theta is bounded by [-pi, pi], re map it to [0,pi]
    if(point.dtheta < -M_PI) {
        point.dtheta += M_PI;
    }
    
    // current angle is bounded by [-pi, pi] re map it to [0, pi]
    long double current_angle = PositionTracking::get_heading_rad();
    if(current_angle < -M_PI) {
        current_angle += M_PI;
    }
    
    // forward drive to point
    long double forward_to_turn = current_angle - point.dtheta;  // change in robot angle
    
    if(forward_to_turn > (M_PI / 2)) {  // find minimal angle change and direction of change [-PI/2, PI/2]
        forward_to_turn = -M_PI + forward_to_turn;  // give negative value to turn left to point
    }
    
    forward_to_turn = PositionTracking::to_degrees(forward_to_turn);
    
    
    // backward drive to point
    long double backward_dtheta = (M_PI/2) + point.dtheta;  // find opposite of angle
    long double backward_to_turn = current_angle - backward_dtheta;
    if(backward_to_turn > (M_PI / 2)) {  // find minimal angle change and direction of change [-PI/2, PI/2]
        backward_to_turn = -M_PI + backward_to_turn;  // give negative value to turn left to point
    }
    backward_to_turn = PositionTracking::to_degrees(backward_to_turn);
    
    long double to_turn;
    int direction;
    if(args.explicit_direction == 1) {  // force positive direction
        to_turn = forward_to_turn;
        direction = 1;
    } else if(args.explicit_direction == -1) {  // force negative direction
        to_turn = backward_to_turn;
        direction = -1;
    } else if(std::abs(forward_to_turn) < std::abs(backward_to_turn)) {  // faster to go forwards
        to_turn = forward_to_turn;
        direction = 1;
    } else {  // faster to go backwards
        to_turn = backward_to_turn;
        direction = -1;
    }
    
    // set up turn
    chassis_params turn_args;
    turn_args.setpoint1 = to_turn;
    turn_args.max_voltage = args.max_voltage;
    turn_args.timeout = 15000; // TODO: add time estimation
    turn_args.motor_slew = args.motor_slew;
    turn_args.log_data = args.log_data;
    
    // perform turn
    t_turn(turn_args);
    pros::delay(100);  // add delay for extra settling
    
    // caclulate distance to move to point
    long double dx = args.setpoint1 - PositionTracking::get_position().x_pos;
    long double dy = args.setpoint2 - PositionTracking::get_position().y_pos;
    // convert end coordinates to polar and then calculate waypoints
    long double distance = std::sqrt((std::pow(dx, 2) + std::pow(dy, 2)));
    
    long double to_drive = direction * PositionTracking::to_encoder_ticks(distance, wheel_diameter);
    
    // set up straight drive
    chassis_params drive_straight_args;
    drive_straight_args.setpoint1 = to_drive;
    drive_straight_args.setpoint2 = 0;
    drive_straight_args.max_voltage = args.max_voltage;
    drive_straight_args.timeout = 15000;
    drive_straight_args.motor_slew = args.motor_slew;
    drive_straight_args.correct_heading = true;
    drive_straight_args.log_data = args.log_data;

    t_profiled_straight_drive(drive_straight_args);
    
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
}



int Chassis::pid_straight_drive(double encoder_ticks, int relative_heading /*0*/, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool correct_heading /*true*/, bool slew /*false*/, bool log_data /*false*/) {
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
    
    chassis_action command = {args, uid, e_pid_straight_drive};
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

int Chassis::profiled_straight_drive(double encoder_ticks, int max_velocity  /*200*/, int profile /*0*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool correct_heading /*true*/, int relative_heading /*0*/, bool slew /*false*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = encoder_ticks;
    args.setpoint2 = relative_heading;
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.motor_slew = slew;
    args.correct_heading = correct_heading;
    args.log_data = log_data;
    if(profile == 0) {
        args.profile = profile_1;
    }
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(encoder_ticks) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_profiled_straight_drive};
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


int Chassis::drive_to_point(double x, double y, int recalculations /*0*/, int explicit_direction /*0*/, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool slew /*false*/, bool log_data /*true*/) {
    chassis_params args;
    args.setpoint1 = x;
    args.setpoint2 = y;
    args.max_voltage = max_voltage;
    args.timeout = timeout;
    args.recalculations = recalculations;
    args.explicit_direction = explicit_direction;
    args.motor_slew = slew;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(x) + 1) + max_voltage + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_drive_to_point};
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



int Chassis::turn_to_point(double x, double y, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool slew /*false*/, bool log_data /*true*/) {
    chassis_params args;
    args.setpoint1 = x;
    args.setpoint2 = y;
    args.max_voltage = max_voltage;
    args.timeout = timeout;
    args.motor_slew = slew;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(x) + 1) + max_voltage + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_turn_to_point};
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



int Chassis::turn_to_angle(double theta, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool slew /*false*/, bool log_data /*true*/) {
    chassis_params args;
    args.setpoint1 = theta;
    args.max_voltage = max_voltage;
    args.timeout = timeout;
    args.motor_slew = slew;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(theta) + 1) + max_voltage + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_turn_to_angle};
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
