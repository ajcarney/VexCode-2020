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
                straight_drive_task(action.args);
            case e_turn:
                turn_task(action.args);
                
        }
        
        while ( send_lock.exchange( true ) ); //aquire lock
        commands_finished.push_back(action.command_uid);
        send_lock.exchange( false ); //release lock
    }
}




void Chassis::straight_drive_task(chassis_params args) {
    Configuration* config = Configuration::get_instance();
    double kP = config->chassis_pid.kP;
    double kI = config->chassis_pid.kI;
    double kD = config->chassis_pid.kD;
    double I_max = config->chassis_pid.I_max;
    
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
    
    double prev_l_encoder = std::get<0>(get_average_encoders(l_id, r_id));
    double prev_r_encoder = std::get<1>(get_average_encoders(l_id, r_id));
    double prev_angle = imu->get_heading();
    double relative_angle = 0;
    
    int timeout = pros::millis() + args.timeout;
    bool settled = false;
    
    int time = pros::millis();

    do {
        // pid distance controller
        double error =  (args.setpoint) - ((std::get<0>(get_average_encoders(l_id, r_id)) + std::get<1>(get_average_encoders(l_id, r_id))) / 2);

        if ( std::abs(integral) > I_max ) {
            integral = 0;  // reset integral if greater than max allowable value
        } else {
            int delta_t = pros::millis() - time;
            integral = integral + (error * delta_t);
        }
        
        time = pros::millis();
        
        
        double derivative = error - prev_error;
        prev_error = error;
        
        
        int right_voltage = (kP * error) + (kI * integral) + (kD * derivative);
        int left_voltage = (kP * error) + (kI * integral) + (kD * derivative);

        
        
        // p controller heading correction
        double delta_l = std::get<0>(get_average_encoders(l_id, r_id)) - prev_l_encoder;
        double delta_r = std::get<1>(get_average_encoders(l_id, r_id)) - prev_r_encoder;
        double delta_theta = calc_delta_theta(prev_angle, delta_l, delta_r);
        prev_angle = prev_angle + delta_theta;
        relative_angle = relative_angle + delta_theta;
        double heading_error = args.relative_heading - relative_angle;
        std::cout << "delta_theta: " << delta_theta << " | prev_anlge: " << prev_angle << " | relative angle: " << relative_angle << " | heading_error: " << heading_error << "\n";
        if(args.correct_heading) {
            left_voltage += 100 * heading_error;  
            right_voltage -= 100 * heading_error;
        }
        
        
        
        // cap voltage to max voltage with regard to velocity
        if ( std::abs(left_voltage) > args.max_voltage ) {
            left_voltage = left_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }
        if ( std::abs(right_voltage) > args.max_voltage ) {
            right_voltage = right_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }

        if ( args.log_data )
        {
            Logger logger;
            log_entry entry;
            entry.content = (
                "[INFO] " + std::string("CHASSIS_PID ")
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
                + ", Time: " + std::to_string(pros::millis())
                + ", Position_Sp: " + std::to_string(args.setpoint)
                + ", position_l: " + std::to_string(std::get<0>(get_average_encoders(l_id, r_id)))
                + ", position_r: " + std::to_string(std::get<1>(get_average_encoders(l_id, r_id)))                
                + ", Heading_Sp: " + std::to_string(args.relative_heading)
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
        
        // // settled is when error is almost zero and voltage is minimal
        if ( ( std::abs(error) < 5 ) && std::abs(left_voltage) < 1000)
        {        
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




void Chassis::turn_task(chassis_params args) {
    Configuration* config = Configuration::get_instance();
    double kP = config->chassis_pid.kP;
    double kI = config->chassis_pid.kI;
    double kD = config->chassis_pid.kD;
    double I_max = config->chassis_pid.I_max;
    
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
    
    double prev_l_encoder = std::get<0>(get_average_encoders(l_id, r_id));
    double prev_r_encoder = std::get<1>(get_average_encoders(l_id, r_id));
    double intitial_angle = imu->get_heading();
    double prev_angle = imu->get_heading();
    double relative_angle = 0;
    
    int timeout = pros::millis() + args.timeout;
    bool settled = false;
    
    int time = pros::millis();

    do {
        // pid angle controller
        double delta_l = std::get<0>(get_average_encoders(l_id, r_id)) - prev_l_encoder;
        double delta_r = std::get<1>(get_average_encoders(l_id, r_id)) - prev_r_encoder;
        double delta_theta = calc_delta_theta(prev_angle, delta_l, delta_r);
        prev_angle = prev_angle + delta_theta;
        relative_angle = relative_angle + delta_theta;
        double error = args.setpoint - relative_angle;
        std::cout << "delta_theta: " << delta_theta << " | prev_anlge: " << prev_angle << " | relative angle: " << relative_angle << " | heading_error: " << error << "\n";

        
        if ( std::abs(integral) > I_max ) {
            integral = 0;  //reset integral if error is too small or greater than max value
        } else {
            int delta_t = pros::millis() - time;
            integral = integral + (error * delta_t);
        }
        time = pros::millis();
        
        
        double derivative = error - prev_error;
        prev_error = error;
        
        
        int right_voltage = (kP * error) + (kI * integral) + (kD * derivative);
        int left_voltage = (kP * error) + (kI * integral) + (kD * derivative);

        
        
        // cap voltage to max voltage with regard to velocity
        if ( std::abs(left_voltage) > args.max_voltage ) {
            left_voltage = left_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }
        if ( std::abs(right_voltage) > args.max_voltage ) {
            right_voltage = right_voltage > 0 ? args.max_voltage : -args.max_voltage;
        }

        if ( args.log_data )
        {
            Logger logger;
            log_entry entry;
            entry.content = (
                "[INFO] " + std::string("CHASSIS_PID ")
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
                + ", Time: " + std::to_string(pros::millis())
                + ", Position_Sp: " + std::to_string(0)
                + ", position_l: " + std::to_string(std::get<0>(get_average_encoders(l_id, r_id)))
                + ", position_r: " + std::to_string(std::get<1>(get_average_encoders(l_id, r_id)))
                + ", Heading_Sp: " + std::to_string(args.setpoint)
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
        if ( ( std::abs(error) < 1 ) && std::abs(left_voltage) < 1000)
        {        
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




int Chassis::straight_drive(double encoder_ticks, int relative_heading /*0*/, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool correct_heading /*true*/, bool slew /*false*/, bool log_data /*false*/) {
    chassis_params args = {
        encoder_ticks,
        encoder_ticks,
        relative_heading,
        max_voltage,
        timeout,
        slew,
        correct_heading,
        asynch,
        log_data
    };
    
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
    chassis_params args = {
        degrees,
        0,
        0,
        max_voltage,
        timeout,
        slew,
        0,
        asynch,
        log_data
    };
    
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
    chassis_params args = {
        -degrees,
        0,
        0,
        max_voltage,
        timeout,
        slew,
        0,
        asynch,
        log_data
    };
    
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




/**
 * takes the average of each side of the drive encoders
 * hopefully to reduce error of encoders
 * returns tuple of encoder values
 */
std::tuple<double, double> Chassis::get_average_encoders(int l_uid, int r_uid)
{
    // use a weighted average to merge all encoders on the robot for a hopefully more accurate reading
    double left_encoder_val = (.1 * front_left_drive->get_encoder_position() * gear_ratio) + (.35 * back_left_drive->get_encoder_position() * gear_ratio) + (.55 * left_encoder->get_position(l_uid));
    double right_encoder_val = (.1 * front_right_drive->get_encoder_position() * gear_ratio) + (.35 * back_right_drive->get_encoder_position() * gear_ratio) + (.55 * right_encoder->get_position(r_uid));
        
    return {left_encoder_val, right_encoder_val};
}




double Chassis::calc_delta_theta(double prev_angle, double delta_l, double delta_r) {
    double imu_reading = std::fmod(imu->get_heading() + 360.0, 360); // scale imu to 0, 360
    double delta_imu = (imu_reading - prev_angle);
    if(delta_imu > 180) {  // find least distance between angles
        delta_imu = 360 - delta_imu;
    } else if (delta_imu < -180) {
        delta_imu = 360 + delta_imu;
    }
    
    std::cout << delta_imu << " | " << imu_reading << "\n";

    long double circumference = width * 3.14159265358;                       // circumference of turning arc 
    long double inches_per_tick = (3.14159265358 * wheel_diameter) / 360.0;  // inches per tick = circumference of wheel / num encoder ticks per revolution
    long double inches_turned = inches_per_tick * (delta_l - delta_r);
    double delta_encoder_degrees = (360 * inches_turned) / circumference;
    
    double delta_theta = (.6 * delta_imu) + (.4 * delta_encoder_degrees);
    double theta = (.6 * imu_reading) + (.4 * (delta_encoder_degrees + prev_angle));

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




/**
 * sets velocity of motor and uses mathematical scaling to maintain accuracy
 */
void Chassis::scaled_drive(float left, float right, float distance, bool pid, float timeout /*INT32_MAX*/ )
{     
    // front_left_drive->tare_encoder();
    // front_right_drive->tare_encoder();
    // back_left_drive->tare_encoder();
    // back_right_drive->tare_encoder();
    // 
    // int stop = pros::millis() + timeout;
    // 
    // while((fabs(std::get<0>(get_average_encoders())) <= distance && fabs(std::get<1>(get_average_encoders())) <= distance) && pros::millis() < stop )
    // {
    //     if(pid)
    //     {
    //         left = ((distance - fabs(std::get<0>(get_average_encoders())))*(1/5.6))*(left/fabs(left)) + (25*(left/fabs(left)));
    //         right = ((distance - fabs(std::get<0>(get_average_encoders())))*(1/5.6))*(right/fabs(left)) + (25*(right/fabs(right)));
    //     }
    // 
    //     front_left_drive->move(left);
    //     front_right_drive->move(right);
    //     back_left_drive->move(left);
    //     back_right_drive->move(right);
    //     pros::delay(10);
    // }
    // 
    // front_left_drive->move(0);
    // front_right_drive->move(0);
    // back_left_drive->move(0);
    // back_right_drive->move(0);
}
 