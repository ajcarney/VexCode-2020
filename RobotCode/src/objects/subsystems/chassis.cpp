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
#include <algorithm>
#include <deque>
#include <type_traits>

#include "main.h"

#include "../serial/Logger.hpp"
#include "../position_tracking/PositionTracker.hpp"
#include "chassis.hpp"



std::vector<double> generate_velocity_profile(int encoder_ticks, const std::function<double(double)>& max_acceleration, double max_decceleration, double max_velocity, double initial_velocity) {
    if(encoder_ticks <= 0) {
        Logger logger;
        log_entry entry;
        entry.content = (
            "[ERROR] " + std::string("PROFILE_CALCULATION")
            + ", Time: " + std::to_string(pros::millis())
            + ", Could not generate profile with negative or 0 encoder ticks"
            + ", enc_ticks: " + std::to_string(encoder_ticks)
            + ", max_velocity: " + std::to_string(max_velocity)
        );
        entry.stream = "clog";
        logger.add(entry);  
        pros::delay(100); // add delay for msg to be logged
        throw std::invalid_argument("Cannot generate profile with negative or 0 encoder ticks");
    }
    
    if(max_decceleration || max_acceleration == 0) {
        Logger logger;
        log_entry entry;
        entry.content = (
            "[ERROR] " + std::string("PROFILE_CALCULATION")
            + ", Time: " + std::to_string(pros::millis())
            + ", Could not generate profile with 0 accleration or decceleration"
            + ", enc_ticks: " + std::to_string(encoder_ticks)
            + ", max_velocity: " + std::to_string(max_velocity)
        );
        entry.stream = "clog";
        logger.add(entry);  
        pros::delay(100); // add delay for msg to be logged
        throw std::invalid_argument("Could not generate profile with 0 accleration or decceleration");
    }
        
    std::vector<double> profile = {initial_velocity};
    
    int i = 0;
    while(i < encoder_ticks) {
        int ticks_left = encoder_ticks - i;
        int ticks_to_deccelerate = profile.at(i) / max_decceleration;
        if(ticks_to_deccelerate < ticks_left) {
            double step = profile.at(i) + max_acceleration(i);
            if(step > max_velocity) {
                step = max_velocity;
            }
            profile.push_back(step);
        } else {
            profile.push_back(profile.at(i) - max_decceleration);
        }
        
        i += 1;
    }
    
    return profile;
}



int Chassis::num_instances = 0;
std::queue<chassis_action> Chassis::command_queue;
std::vector<int> Chassis::commands_finished;
std::atomic<bool> Chassis::command_start_lock = ATOMIC_VAR_INIT(false);
std::atomic<bool> Chassis::command_finish_lock = ATOMIC_VAR_INIT(false);

Motor* Chassis::front_left_drive;
Motor* Chassis::front_right_drive;
Motor* Chassis::back_left_drive;
Motor* Chassis::back_right_drive;

Encoder* Chassis::left_encoder;
Encoder* Chassis::right_encoder;
double Chassis::width;
double Chassis::gear_ratio;
double Chassis::wheel_diameter;


Chassis::Chassis( Motor &front_left, Motor &front_right, Motor &back_left, Motor &back_right, Encoder &l_encoder, Encoder &r_encoder, double chassis_width, double gearing /*1*/, double wheel_size /*4.05*/)
{
    front_left_drive = &front_left;
    front_right_drive = &front_right;
    back_left_drive = &back_left;
    back_right_drive = &back_right;
    
    left_encoder = &l_encoder;
    right_encoder = &r_encoder;
    
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




Chassis::~Chassis() {
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
        while ( command_start_lock.exchange( true ) ); //aquire lock
        chassis_action action = command_queue.front();
        command_queue.pop();
        command_start_lock.exchange( false ); //release lock
        
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
                PositionTracker* tracker = PositionTracker::get_instance();
                std::vector<waypoint> waypoints;  // calculate waypoints based on starting position

                long double dx = action.args.setpoint1 - tracker->get_position().x_pos;
                long double dy = action.args.setpoint2 - tracker->get_position().y_pos;
                std::cout << tracker->get_position().x_pos << " " << tracker->get_position().y_pos << "\n";
                // convert end coordinates to polar and then calculate waypoints
                long double delta_radius_polar = std::sqrt((std::pow(dx, 2) + std::pow(dy, 2)));
                long double delta_theta_polar = std::atan2(dy, dx);
                
                for(int i=action.args.recalculations + 1; i > 0; i--) {  // calculate additional waypoints, start with last endpoint and go down
                    long double radius = (i * delta_radius_polar) / (action.args.recalculations + 1);
                    waypoint recalc_point;
                    recalc_point.x = tracker->get_position().x_pos + (radius * std::cos(delta_theta_polar));  // intital x + dx
                    recalc_point.y = tracker->get_position().y_pos + (radius * std::sin(delta_theta_polar));  // initial y + dy
                    std::cout << radius << " " << delta_theta_polar << " " << (radius * std::cos(delta_theta_polar)) << " " << (radius * std::sin(delta_theta_polar)) << "\n";
                    recalc_point.dx = radius * std::cos(delta_theta_polar);
                    recalc_point.dy = radius * std::sin(delta_theta_polar);
                    recalc_point.radius = radius;
                    recalc_point.dtheta = delta_theta_polar;
                    waypoints.insert(waypoints.begin(), recalc_point);
                }
                std::cout << "\n\n\n\n\n";
                if(action.args.log_data) {
                    Logger logger;
                    log_entry entry;
                    std::string msg = (
                        "[INFO] " + std::string("CHASSIS_ODOM")
                        + ", Time: " + std::to_string(pros::millis())
                        + ", dx: " + std::to_string(dx)
                        + ", dy: " + std::to_string(dy)
                        + ", delta_theta_polar: " + std::to_string(delta_theta_polar)
                        + ", current x: " + std::to_string(tracker->get_position().x_pos)
                        + ", current y: " + std::to_string(tracker->get_position().y_pos)
                        + ", current theta: " + std::to_string(tracker->to_degrees(tracker->get_position().theta))
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
                PositionTracker* tracker = PositionTracker::get_instance();
                
                long double dx = action.args.setpoint1 - tracker->get_position().x_pos;
                long double dy = action.args.setpoint2 - tracker->get_position().y_pos;
                
                // convert end coordinates to polar to find the change in angle
                // long double dtheta = std::fmod((-M_PI / 2) + std::atan2(dy, dx), (2 * M_PI));
                long double dtheta = std::atan2(dy, dx);
                if(dtheta < 0) {  // map to [0, 2pi]
                    dtheta += 2 * M_PI;
                }

                // current angle is bounded by [-pi, pi] re map it to [0, 2pi]
                long double current_angle = tracker->get_heading_rad();
                if(current_angle < 0) {
                    current_angle += 2 * M_PI;
                }
                current_angle = (-current_angle) + (M_PI / 2);
                
                // calculate how much the robot needs to turn to be at the angle
                long double to_turn_face_forwards = current_angle - dtheta;  // change in robot angle
                long double to_turn_face_backwards = (current_angle - dtheta) - M_PI;
                
                if(to_turn_face_forwards > M_PI) {  // find minimal angle change and direction of change [-PI/2, PI/2]
                    to_turn_face_forwards = (-2 * M_PI) + to_turn_face_forwards;  // give negative value to turn left to point
                } else if(to_turn_face_forwards < -M_PI) {
                    to_turn_face_forwards = (2 * M_PI) + to_turn_face_forwards;  // give positive value to turn left to point
                }
                
                if(to_turn_face_backwards > M_PI) {  // find minimal angle change and direction of change [-PI/2, PI/2]
                    to_turn_face_backwards = (-2 * M_PI) + to_turn_face_backwards;  // give negative value to turn left to point
                } else if(to_turn_face_backwards < -M_PI) {
                    to_turn_face_backwards = (2 * M_PI) + to_turn_face_backwards;  // give positive value to turn left to point
                }
                
                
                long double to_turn;
                int direction;
                if(action.args.explicit_direction == 1) {  // force positive direction
                    to_turn = to_turn_face_forwards;
                    direction = 1;
                } else if(action.args.explicit_direction == -1) {  // force negative direction
                    to_turn = to_turn_face_backwards;
                    direction = -1;
                } else if(std::abs(to_turn_face_forwards) < std::abs(to_turn_face_backwards)) {  // faster to go forwards
                    to_turn = to_turn_face_forwards;
                    direction = 1;
                } else {  // faster to go backwards
                    to_turn = to_turn_face_backwards;
                    direction = -1;
                }
                
                to_turn = tracker->to_degrees(to_turn);
                
                
                // set up turn
                chassis_params turn_args;
                turn_args.setpoint1 = to_turn;
                turn_args.max_velocity = action.args.max_velocity;
                turn_args.timeout = 15000; // TODO: add time estimation
                turn_args.kP = 2.8;
                turn_args.kI = 0.0005;
                turn_args.kD = 50;
                turn_args.I_max = INT32_MAX;
                turn_args.motor_slew = action.args.motor_slew;
                turn_args.log_data = action.args.log_data;
                
                // perform turn
                std::cout << "starting turn\n";
                std::cout << to_turn << "\n";
                t_turn(turn_args);
                std::cout << "turn done\n";
                
                if(action.args.log_data) {
                    Logger logger;
                    log_entry entry;
                    entry.content = (
                        "[INFO] " + std::string("CHASSIS_ODOM")
                        + ", Time: " + std::to_string(pros::millis())
                        + ", X " + std::to_string(action.args.setpoint1)
                        + ", Y " + std::to_string(action.args.setpoint2)
                        + ", ToTurnForwards: " + std::to_string(tracker->to_degrees(to_turn_face_forwards))
                        + ", ToTurnBackwards: " + std::to_string(tracker->to_degrees(to_turn_face_backwards))
                        + ", ToTurn: " + std::to_string(to_turn)
                        + ", Direction: " + std::to_string(direction)
                        + ", dx: " + std::to_string(dx)
                        + ", dy: " + std::to_string(dy)
                        + ", X: " + std::to_string(tracker->get_position().x_pos)
                        + ", Y: " + std::to_string(tracker->get_position().y_pos)
                        + ", Theta: " + std::to_string(tracker->to_degrees(tracker->get_position().theta))
                    );
                    entry.stream = "clog";
                    logger.add(entry);  
                }
                
                pros::delay(100);  // add delay for extra settling
                
                break;
            } case e_turn_to_angle: {
                PositionTracker* tracker = PositionTracker::get_instance();
                                
                // current angle is bounded by [-pi, pi] re map it to [0, pi]
                long double current_angle = tracker->get_heading_rad();
                if(current_angle < -M_PI) {
                    current_angle += M_PI;
                }
                
                // calculate how much the robot needs to turn to be at the angle
                long double to_turn = action.args.setpoint1 - current_angle;  // change in robot angle
                
                if(to_turn > M_PI) {  // find minimal angle change and direction of change [-PI/2, PI/2]
                    to_turn = (-2 * M_PI) + to_turn;  // give negative value to turn left to point
                } else if(to_turn < -M_PI) {
                    to_turn = (2 * M_PI) + to_turn;  // give positive value to turn left to point
                }
                to_turn = tracker->to_degrees(to_turn);
                
                std::cout << current_angle << " " << to_turn << "\n";
                
                // set up turn
                chassis_params turn_args;
                turn_args.setpoint1 = to_turn;
                turn_args.max_velocity = action.args.max_velocity;
                turn_args.timeout = action.args.timeout; // TODO: add time estimation
                turn_args.kP = 2.8;
                turn_args.kI = 0.0005;
                turn_args.kD = 50;
                turn_args.I_max = INT32_MAX;
                turn_args.motor_slew = action.args.motor_slew;
                turn_args.log_data = action.args.log_data;

                if(action.args.log_data) {
                    Logger logger;
                    log_entry entry;
                    std::string msg = (
                        "[INFO] " + std::string("CHASSIS_ODOM")
                        + ", Time: " + std::to_string(pros::millis())
                        + ", turning: " + std::to_string(to_turn)
                        + ", Current re-bounded angle: " + std::to_string(tracker->to_degrees(current_angle))
                        + ", Current angle: " + std::to_string(tracker->to_degrees(tracker->get_heading_rad()))
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
        
        while ( command_finish_lock.exchange( true ) ); //aquire lock
        commands_finished.push_back(action.command_uid);
        command_finish_lock.exchange( false ); //release lock
    }
}




void Chassis::t_pid_straight_drive(chassis_params args) {
    PositionTracker* tracker = PositionTracker::get_instance();
    Configuration* config = Configuration::get_instance();
    
    double kP_l = args.kP;
    double kI_l = args.kI;
    double kD_l = args.kD;
    double I_max_l = args.I_max;
    
    double kP_r = kP_l;
    double kI_r = kI_l;
    double kD_r = kD_l;
    double I_max_r = I_max_l;
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    
    front_left_drive->set_motor_mode(e_builtin_velocity_pid);
    front_right_drive->set_motor_mode(e_builtin_velocity_pid);
    back_left_drive->set_motor_mode(e_builtin_velocity_pid);
    back_right_drive->set_motor_mode(e_builtin_velocity_pid);
    
    int r_id = right_encoder->get_unique_id(true);
    int l_id = left_encoder->get_unique_id(true);
    
    double integral_l = 0;
    double integral_r = 0;
    double prev_error_l = 0;
    double prev_error_r = 0;
    double prev_velocity_l = 0;
    double prev_velocity_r = 0;
    
    double prev_l_encoder = std::get<0>(Sensors::get_average_encoders(l_id, r_id));
    double prev_r_encoder = std::get<1>(Sensors::get_average_encoders(l_id, r_id));

    long double relative_angle = 0;
    long double abs_angle = tracker->to_degrees(tracker->get_heading_rad());
    long double prev_abs_angle = abs_angle;
    long double integral_heading = 0;
    double prev_heading_error = 0;
    
    bool settled = false;
    std::vector<double> previous_l_velocities;
    std::vector<double> previous_r_velocities;
    int velocity_history = 15;
    bool use_integral_l = true;
    bool use_integral_r = true;
    
    int current_time = pros::millis();
    int start_time = current_time;

    do {
        int dt = pros::millis() - current_time;
        // pid distance controller
        double error_l = args.setpoint1 - std::get<0>(Sensors::get_average_encoders(l_id, r_id));
        double error_r = args.setpoint2 - std::get<1>(Sensors::get_average_encoders(l_id, r_id));

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
        
        current_time = pros::millis();
        
        double derivative_l = error_l - prev_error_l;
        double derivative_r = error_r - prev_error_r;
        prev_error_l = error_l;
        prev_error_r = error_r;
        
        double left_velocity = (kP_l * error_l) + (kI_l * integral_l) + (kD_l * derivative_l);
        double right_velocity = (kP_r * error_r) + (kI_r * integral_r) + (kD_r * derivative_r);

        
    // slew rate code
        double delta_velocity_l = left_velocity - prev_velocity_l;
        double delta_velocity_r = right_velocity - prev_velocity_r;
        double slew_rate = args.motor_slew;
        if(std::abs(delta_velocity_l) > (dt * slew_rate) && (std::signbit(delta_velocity_l) == std::signbit(left_velocity)) ) {  // ignore deceleration
            if(delta_velocity_l == 0) {
                std::cout << "delta_velocity_l was equal to 0\n";
                pros::delay(100);
                std::cout << previous_l_velocities.at(999) << "\n";  // throw some error that is easy to see
            }
            int sign = std::abs(delta_velocity_l) / delta_velocity_l;
            // std::cout << "l over slew: " << sign << " " << dt << " " << slew_rate << "\n";
            left_velocity = prev_velocity_l + (sign * dt * slew_rate);
        }
        
        if(std::abs(delta_velocity_r) > (dt * slew_rate) && (std::signbit(delta_velocity_r) == std::signbit(right_velocity))) {
            if(delta_velocity_r == 0) {
                std::cout << previous_r_velocities.at(999) << "\n";  // throw some error that is easy to see
            }
            int sign = std::abs(delta_velocity_r) / delta_velocity_r;
            // std::cout << "r over slew: " << sign << " " << dt << " " << slew_rate << "\n";
            right_velocity = prev_velocity_r + (sign * dt * slew_rate);
        }
        
        
    // p controller heading correction
        abs_angle = tracker->get_heading_rad();
        abs_angle = std::atan2(std::sin(abs_angle), std::cos(abs_angle));
        long double delta_theta;
        // account for angle wrap around    ie. new = -1, prev = -359   == bad delta
        if(prev_abs_angle > 0 && abs_angle < 0 && std::abs(tracker->to_radians(prev_abs_angle)) + std::abs(abs_angle) > (M_PI)) {
            delta_theta = tracker->to_degrees((2*M_PI) + abs_angle) - prev_abs_angle;
        } else if(prev_abs_angle < 0 && abs_angle > 0 && std::abs(tracker->to_radians(prev_abs_angle)) + std::abs(abs_angle) > (M_PI)) {
            delta_theta = tracker->to_degrees(abs_angle - (2*M_PI)) - prev_abs_angle;
        } else {
            delta_theta = tracker->to_degrees(abs_angle) - prev_abs_angle;
        }

        relative_angle += delta_theta;
        prev_abs_angle = tracker->to_degrees(abs_angle);
        
        
        double heading_error = 0 - relative_angle;  // 0 is the setpoint because we want to drive straight
        integral_heading = integral_heading + (heading_error * dt);
        double d_heading_error = heading_error - prev_heading_error;
        prev_heading_error = heading_error;
        // std::cout << "delta_theta: " << delta_theta << " | prev_anlge: " << prev_angle << " | relative angle: " << relative_angle << " | heading_error: " << heading_error << "\n";
        int velocity_correction = (.05 * heading_error) + (0 * integral_heading) + (0 * d_heading_error);
        // int velocity_correction = (4 * heading_error) + (0 * integral_heading) + (54 * d_heading_error);
        if(args.correct_heading && heading_error > 0.00001) {  // veering left
            // velocity_correction = 5;
            right_velocity -= velocity_correction;  
        } else if ( args.correct_heading && heading_error < -0.00001) {  // veering right
            // velocity_correction = 5;
            left_velocity -= velocity_correction;
        }
        
        
        // cap voltage to max voltage with regard to velocity
        if ( std::abs(left_velocity) > args.max_velocity ) {
            left_velocity = left_velocity > 0 ? args.max_velocity : -args.max_velocity;
        }
        if ( std::abs(right_velocity) > args.max_velocity ) {
            right_velocity = right_velocity > 0 ? args.max_velocity : -args.max_velocity;
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

        prev_velocity_l = left_velocity;
        prev_velocity_r = right_velocity;
        
        previous_l_velocities.push_back(left_velocity);
        previous_r_velocities.push_back(right_velocity);
        if(previous_l_velocities.size() > velocity_history) {
            previous_l_velocities.erase(previous_l_velocities.begin());
        }
        
        if(previous_r_velocities.size() > velocity_history) {
            previous_r_velocities.erase(previous_r_velocities.begin());
        }
        
        // settled is when error is almost zero and velocity is minimal
        double l_difference = *std::minmax_element(previous_l_velocities.begin(), previous_l_velocities.end()).second - *std::minmax_element(previous_l_velocities.begin(), previous_l_velocities.end()).first;
        double r_difference = *std::minmax_element(previous_r_velocities.begin(), previous_r_velocities.end()).second - *std::minmax_element(previous_r_velocities.begin(), previous_r_velocities.end()).first;
        // std::cout << "difference: " << *std::minmax_element(previous_l_velocities.begin(), previous_l_velocities.end()).second << " " << previous_l_velocities.size() << "\n";
        if (
            std::abs(l_difference) < 2 
            && previous_l_velocities.size() == velocity_history 
            && std::abs(r_difference) < 2 
            && previous_r_velocities.size() == velocity_history
            && left_velocity < 2
            && right_velocity < 2
        ) { 
            break; // end before timeout 
        }
        
        
        front_left_drive->move_velocity(left_velocity);
        front_right_drive->move_velocity(right_velocity);
        back_left_drive->move_velocity(left_velocity);
        back_right_drive->move_velocity(right_velocity);

        pros::delay(10);
    } while ( pros::millis() < start_time + args.timeout ); 
    
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
    
    right_encoder->forget_position(r_id);  // free up space in the encoders log
    left_encoder->forget_position(l_id);
}




void Chassis::t_profiled_straight_drive(chassis_params args) {
    PositionTracker* tracker = PositionTracker::get_instance();
    Configuration* config = Configuration::get_instance();

    double kP = args.kP;
    double kI = args.kI;
    double kD = args.kD;
    double I_max = INT32_MAX;
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    
    front_left_drive->set_motor_mode(e_builtin_velocity_pid);
    front_right_drive->set_motor_mode(e_builtin_velocity_pid);
    back_left_drive->set_motor_mode(e_builtin_velocity_pid);
    back_right_drive->set_motor_mode(e_builtin_velocity_pid);
    
    int r_id = right_encoder->get_unique_id(true);
    int l_id = left_encoder->get_unique_id(true);
    
    long double relative_angle = 0;
    long double abs_angle = tracker->to_degrees(tracker->get_heading_rad());
    long double prev_abs_angle = abs_angle;
    long double integral = 0;
    long double prev_integral = 0;
    double prev_error = 0;
    bool use_integral = true;
    
    int current_time = pros::millis();
    int start_time = current_time;
    bool settled = false;
    bool was_at_target_l = false;
    bool was_at_target_r = false;
    
    std::vector<double> previous_l_velocities;
    std::vector<double> previous_r_velocities;
    int velocity_history = 15;
    
    auto accel_func = [](double n) -> double { return 0.005 * n; };
    // auto accel_func = [](double n) -> double { return 1; };
    std::vector<double> velocity_profile = generate_velocity_profile(std::abs(args.setpoint1), accel_func, .55, args.max_velocity, 50);  // .45 is decceleration, 10 is initial velocity
    
    do {
        int dt = pros::millis() - current_time;
        current_time = pros::millis();
        
        double velocity_l;
        double velocity_r;
        if(std::abs(std::get<0>(Sensors::get_average_encoders(l_id, r_id))) <= std::abs(args.setpoint1)) {
            velocity_l = velocity_profile.at(std::abs(std::get<0>(Sensors::get_average_encoders(l_id, r_id))));
        } else {
            was_at_target_l = true;
            velocity_l = 0;
        }
        
        if(std::abs(std::get<1>(Sensors::get_average_encoders(l_id, r_id))) <= std::abs(args.setpoint1)) {
            velocity_r = velocity_profile.at(std::abs(std::get<1>(Sensors::get_average_encoders(l_id, r_id))));
        } else {
            was_at_target_l = true;
            velocity_r = 0;
        }

        
        // double velocity;
        // if(velocity_l > velocity_r) {
        //     velocity_r = velocity_r;
        //     velocity_l = velocity_r;
        // } else {
        //     velocity_r = velocity_l;
        //     velocity_l = velocity_l;
        // }

        if(args.setpoint1 < 0) {
            velocity_l = -velocity_l;
            velocity_r = -velocity_r;
        }
        
        abs_angle = tracker->to_degrees(tracker->get_heading_rad());
        long double delta_theta = abs_angle - prev_abs_angle;
        relative_angle += delta_theta;
        prev_abs_angle = abs_angle;
        
        long double error = 0 - relative_angle;  // setpoint is 0 because we want to drive straight
        // long double error = std::get<0>(Sensors::get_average_encoders(l_id, r_id)) - std::get<1>(Sensors::get_average_encoders(l_id, r_id));
        std::cout << "relative angle: " << relative_angle << " | dtheta: " << delta_theta << "\n";
        // cap velocity to max velocity with regard to velocity
        integral = integral + (error * dt);
        if(integral > I_max) {
            integral = I_max;
        } else if (integral < -I_max) {
            integral = -I_max;
        }
        
        // if(std::signbit(error) != std::signbit(prev_error)) {
        //     std::cout << "halving " << integral << " " << prev_integral;
        //     integral = .5 * integral;
        // }
        prev_integral = integral;
        
        double derivative = error - prev_error;
        prev_error = error;
        
        
        // std::cout << error << " " << relative_angle << "\n";
        
    // pid heading correction
        // int velocity_correction = (kP * error) + (kI * integral) + (kD * derivative);
        
    // PI heading correction
        // double velocity_correction = std::abs(kI * integral);
        double velocity_correction = std::abs(args.kP * error + args.kI * integral + args.kD * derivative);
        std::cout << "integral: " << integral << " " << velocity_correction << "\n";
        if(args.correct_heading  && error > 0.000001) {  // veering off course, so correct
            velocity_r -= velocity_correction / 2;
            velocity_l += velocity_correction / 2;
        } else if(args.correct_heading && error < -0.000001) {
            velocity_l -= velocity_correction / 2;  
            velocity_r += velocity_correction / 2;
        }
        

    // cap velocity to max velocity with regard to direction
        if ( std::abs(velocity_l) > args.max_velocity ) {
            velocity_l = velocity_l > 0 ? args.max_velocity : -args.max_velocity;
        }
        if ( std::abs(velocity_r) > args.max_velocity ) {
            velocity_r = velocity_r > 0 ? args.max_velocity : -args.max_velocity;
        }

        if ( args.log_data ) {
            Logger logger;
            log_entry entry;
            entry.content = (
                "[INFO] " + std::string("CHASSIS_PROFILED_STRAIGHT_DRIVE")
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
                + ", Actual_Vel1: " + std::to_string(velocity_l)
                + ", Actual_Vel2: " + std::to_string(velocity_r)
                + ", Actual_Vel3: " + std::to_string(back_left_drive->get_actual_velocity())
                + ", Actual_Vel4: " + std::to_string(back_right_drive->get_actual_velocity())
                + ", Correction: " + std::to_string(velocity_correction)
            );
            entry.stream = "clog";
            logger.add(entry);            
        }
        
        double error_l = std::abs(args.setpoint1 - std::get<0>(Sensors::get_average_encoders(l_id, r_id)));
        double error_r = std::abs(args.setpoint1 - std::get<1>(Sensors::get_average_encoders(l_id, r_id)));
        
        previous_l_velocities.push_back(velocity_l);
        previous_r_velocities.push_back(velocity_r);
        if(previous_l_velocities.size() > velocity_history) {
            previous_l_velocities.erase(previous_l_velocities.begin());
        }
        if(previous_r_velocities.size() > velocity_history) {
            previous_r_velocities.erase(previous_r_velocities.begin());
        }
        
        // settled is when error is almost zero and velocity is minimal
        double l_difference = *std::minmax_element(previous_l_velocities.begin(), previous_l_velocities.end()).second - *std::minmax_element(previous_l_velocities.begin(), previous_l_velocities.end()).first;
        double r_difference = *std::minmax_element(previous_r_velocities.begin(), previous_r_velocities.end()).second - *std::minmax_element(previous_r_velocities.begin(), previous_r_velocities.end()).first;
        if (
            std::abs(l_difference) < 2 
            && previous_l_velocities.size() == velocity_history 
            && std::abs(r_difference) < 2 
            && previous_r_velocities.size() == velocity_history
            && std::abs(velocity_l) < 2
            && std::abs(velocity_r) < 2
        ) { 
            break; // end before timeout 
        }
        // if(error_l < 5 || error_r < 5) {  // shut off motors when one side reaches the setpoint
        //     velocity_l = 0;
        //     velocity_r = 0;
        //     break;
        // }
        
        std::cout << "velocity: " << velocity_l << " " << velocity_r << "\n";    
        std::cout << "error: " << error_r << " " << error_l << " " << error << "\n";
        front_left_drive->move_velocity(velocity_l);
        front_right_drive->move_velocity(velocity_r);
        back_left_drive->move_velocity(velocity_l);
        back_right_drive->move_velocity(velocity_r);
        
        pros::delay(10);
    } while (pros::millis() < start_time + args.timeout); 
    
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
    PositionTracker* tracker = PositionTracker::get_instance();
    Configuration* config = Configuration::get_instance();
    
    double kP = args.kP;
    double kI = args.kI;
    double kD = args.kD;
    double I_max = args.I_max;
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    
    front_left_drive->set_motor_mode(e_builtin_velocity_pid);
    front_right_drive->set_motor_mode(e_builtin_velocity_pid);
    back_left_drive->set_motor_mode(e_builtin_velocity_pid);
    back_right_drive->set_motor_mode(e_builtin_velocity_pid);
    
    front_left_drive->move_velocity(0);
    front_right_drive->move_velocity(0);
    back_left_drive->move_velocity(0);
    back_right_drive->move_velocity(0);
    
    front_left_drive->tare_encoder();
    front_right_drive->tare_encoder();
    back_left_drive->tare_encoder();
    back_right_drive->tare_encoder();
    
    int r_id = right_encoder->get_unique_id();
    int l_id = left_encoder->get_unique_id();
    right_encoder->reset(r_id);
    left_encoder->reset(l_id);
    
    long double relative_angle = 0;
    long double abs_angle = tracker->to_degrees(tracker->get_heading_rad());
    long double prev_abs_angle = abs_angle;
    long double integral = 0;
    double prev_error = 0;
    bool use_integral = true;
    int current_time = pros::millis();
    int start_time = current_time;
    
    double prev_velocity_l = 0;
    double prev_velocity_r = 0;
        
    // std::vector<double> previous_l_velocities;
    // std::vector<double> previous_r_velocities;
    // int velocity_history = 15;
    std::vector<double> error_history;
    int max_history_length = 20;
    
    do {
        int dt = pros::millis() - current_time;
        
        abs_angle = tracker->get_heading_rad();
        abs_angle = std::atan2(std::sin(abs_angle), std::cos(abs_angle));
        long double delta_theta;
        // account for angle wrap around
        // ie. new = -1, prev = -359   == bad delta
        if(prev_abs_angle > 0 && abs_angle < 0 && std::abs(tracker->to_radians(prev_abs_angle)) + std::abs(abs_angle) > (M_PI)) {
            delta_theta = tracker->to_degrees((2*M_PI) + abs_angle) - prev_abs_angle;
        } else if(prev_abs_angle < 0 && abs_angle > 0 && std::abs(tracker->to_radians(prev_abs_angle)) + std::abs(abs_angle) > (M_PI)) {
            delta_theta = tracker->to_degrees(abs_angle - (2*M_PI)) - prev_abs_angle;
        } else {
            delta_theta = tracker->to_degrees(abs_angle) - prev_abs_angle;
        }
        // long double delta_theta = abs_angle - prev_abs_angle;
        // long double delta_theta = tracker->to_degrees(tracker->get_delta_theta_rad());
        relative_angle += delta_theta;
        prev_abs_angle = tracker->to_degrees(abs_angle);
        
        long double error = args.setpoint1 - relative_angle;
        
        integral = integral + (error * dt);
        if(integral > I_max) {
            integral = I_max;
        } else if (integral < -I_max) {
            integral = -I_max;
        }
        
        double derivative = error - prev_error;
        prev_error = error;
        
        current_time = pros::millis();
        
        // std::cout << "relative angle: " << relative_angle << " | dtheta: " << delta_theta << "\n";
        // std::cout << error << " " << relative_angle << "\n";
        
        double abs_velocity = (kP * error) + (kI * integral) + (kD * derivative);
        double l_velocity = abs_velocity;  
        double r_velocity = -abs_velocity;
        
        // slew rate code
        double delta_velocity_l = l_velocity - prev_velocity_l;
        double delta_velocity_r = r_velocity - prev_velocity_r;
        double slew_rate = args.motor_slew;
        int over_slew = 0;
        if(std::abs(delta_velocity_l) > (dt * slew_rate) && (std::signbit(delta_velocity_l) == std::signbit(l_velocity)) ) {  // ignore deceleration
            int sign = std::abs(delta_velocity_l) / delta_velocity_l;
            std::cout << "l over slew: " << sign << " " << dt << " " << slew_rate << "\n";
            l_velocity = prev_velocity_l + (sign * dt * slew_rate);
            over_slew = 1;
        }
        
        if(std::abs(delta_velocity_r) > (dt * slew_rate) && (std::signbit(delta_velocity_r) == std::signbit(r_velocity))) {
            int sign = std::abs(delta_velocity_r) / delta_velocity_r;
            std::cout << "r over slew: " << sign << " " << dt << " " << slew_rate << "\n";
            r_velocity = prev_velocity_r + (sign * dt * slew_rate);
            over_slew = 1;
        }
        
        
        // cap velocity to max velocity with regard to velocity
        if ( std::abs(l_velocity) > args.max_velocity ) {
            l_velocity = l_velocity > 0 ? args.max_velocity : -args.max_velocity;
        }
        if ( std::abs(r_velocity) > args.max_velocity ) {
            r_velocity = r_velocity > 0 ? args.max_velocity : -args.max_velocity;
        }
        
        // prev_velocity_l = l_velocity;
        // prev_velocity_r = r_velocity;
        // 
        // previous_l_velocities.push_back(l_velocity);
        // previous_r_velocities.push_back(r_velocity);
        // if(previous_l_velocities.size() > velocity_history) {
        //     previous_l_velocities.erase(previous_l_velocities.begin());
        // }
        // 
        // if(previous_r_velocities.size() > velocity_history) {
        //     previous_r_velocities.erase(previous_r_velocities.begin());
        // }
        error_history.push_back(prev_error);
        if(error_history.size() > max_history_length) {
             error_history.erase(error_history.begin());
        }
        
                
        std::cout << l_velocity << " " << r_velocity << " " << relative_angle << " " << error << "\n";    
        // for(int i=0; i < previous_l_velocities.size(); i++) {
        //     std::cout << previous_l_velocities.at(i) << " ";
        // }
        // std::cout << "\n";
        double error_difference = *std::minmax_element(error_history.begin(), error_history.end()).second - *std::minmax_element(error_history.begin(), error_history.end()).first;

        if ( args.log_data ) {
            Logger logger;
            log_entry entry;
            entry.content = (
                "[INFO] " + std::string("CHASSIS_PID_TURN")
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
                + ", Absolute Angle: " + std::to_string(abs_angle)
                + ", error history: " + std::to_string(error_history.size())
                + ", history size: " + std::to_string(max_history_length)
                + ", time out time: " + std::to_string(start_time + args.timeout)
                + ", error difference: " + std::to_string(error_difference)
                + ", over slew: " + std::to_string(over_slew)
                + ", Actual_Vel1: " + std::to_string(front_left_drive->get_actual_velocity())
                + ", Actual_Vel2: " + std::to_string(front_right_drive->get_actual_velocity())
                + ", Actual_Vel3: " + std::to_string(back_left_drive->get_actual_velocity())
                + ", Actual_Vel4: " + std::to_string(back_right_drive->get_actual_velocity())
            );
            entry.stream = "clog";
            logger.add(entry);            
        }

        // settled is when error is almost zero and velocity is minimal
        // double l_difference = *std::minmax_element(previous_l_velocities.begin(), previous_l_velocities.end()).second - *std::minmax_element(previous_l_velocities.begin(), previous_l_velocities.end()).first;
        // double r_difference = *std::minmax_element(previous_r_velocities.begin(), previous_r_velocities.end()).second - *std::minmax_element(previous_r_velocities.begin(), previous_r_velocities.end()).first;
        // std::cout << "difference: " << *std::minmax_element(previous_l_velocities.begin(), previous_l_velocities.end()).second << " " << previous_l_velocities.size() << "\n";
        if (
            std::abs(error_difference) < .007
            && error_history.size() == max_history_length
            && pros::millis() > start_time + 500
            // std::abs(l_difference) < 2 
            // && previous_l_velocities.size() == velocity_history 
            // && std::abs(r_difference) < 2 
            // && previous_r_velocities.size() == velocity_history
            // && l_velocity < 2
            // && r_velocity < 2
        ) {  // velocity change has been minimal, so stop   
            front_left_drive->set_motor_mode(e_voltage);
            front_right_drive->set_motor_mode(e_voltage);
            back_left_drive->set_motor_mode(e_voltage);
            back_right_drive->set_motor_mode(e_voltage);
            
            front_left_drive->set_voltage(0);
            front_right_drive->set_voltage(0);
            back_left_drive->set_voltage(0);
            back_right_drive->set_voltage(0);
            std::cout << "ending\n";
            break; // end before timeout 
        }
        
        front_left_drive->move_velocity(l_velocity);
        front_right_drive->move_velocity(r_velocity);
        back_left_drive->move_velocity(l_velocity);
        back_right_drive->move_velocity(r_velocity);
    

        pros::delay(10);
    } while ( pros::millis() < (start_time + args.timeout) ); 
    
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
    
    right_encoder->forget_position(r_id);  // free up space in the encoders log
    left_encoder->forget_position(l_id);
}



void Chassis::t_move_to_waypoint(chassis_params args, waypoint point) {
    PositionTracker* tracker = PositionTracker::get_instance();
    
    long double dx = point.x - tracker->get_position().x_pos;
    long double dy = point.y - tracker->get_position().y_pos;
    
    // convert end coordinates to polar to find the change in angle
    // long double dtheta = std::fmod((-M_PI / 2) + std::atan2(dy, dx), (2 * M_PI));
    long double dtheta = std::atan2(dy, dx);
    if(dtheta < 0) {  // map to [0, 2pi]
        dtheta += 2 * M_PI;
    }

    // current angle is bounded by [-pi, pi] re map it to [0, 2pi]
    long double current_angle = tracker->get_heading_rad();
    if(current_angle < 0) {
        current_angle += 2 * M_PI;
    }
    current_angle = (-current_angle) + (M_PI / 2);
    
    // calculate how much the robot needs to turn to be at the angle
    long double to_turn_face_forwards = current_angle - dtheta;  // change in robot angle
    long double to_turn_face_backwards = (current_angle - dtheta) - M_PI;
    
    if(to_turn_face_forwards > M_PI) {  // find minimal angle change and direction of change [-PI/2, PI/2]
        to_turn_face_forwards = (-2 * M_PI) + to_turn_face_forwards;  // give negative value to turn left to point
    } else if(to_turn_face_forwards < -M_PI) {
        to_turn_face_forwards = (2 * M_PI) + to_turn_face_forwards;  // give positive value to turn right to point
    }
    
    if(to_turn_face_backwards > M_PI) {  // find minimal angle change and direction of change [-PI/2, PI/2]
        to_turn_face_backwards = (-2 * M_PI) + to_turn_face_backwards;  // give negative value to turn left to point
    } else if(to_turn_face_backwards < -M_PI) {
        to_turn_face_backwards = (2 * M_PI) + to_turn_face_backwards;  // give positive value to turn right to point
    }
    
    
    long double to_turn;
    int direction;
    if(args.explicit_direction == 1) {  // force positive direction
        to_turn = to_turn_face_forwards;
        direction = 1;
    } else if(args.explicit_direction == -1) {  // force negative direction
        to_turn = to_turn_face_backwards;
        direction = -1;
    } else if(std::abs(to_turn_face_forwards) < std::abs(to_turn_face_backwards)) {  // faster to go forwards
        to_turn = to_turn_face_forwards;
        direction = 1;
    } else {  // faster to go backwards
        to_turn = to_turn_face_backwards;
        direction = -1;
    }
    
    to_turn = tracker->to_degrees(to_turn);
    
    
    // set up turn
    chassis_params turn_args;
    turn_args.setpoint1 = to_turn;
    turn_args.max_velocity = args.max_velocity;
    turn_args.timeout = 15000; // TODO: add time estimation
    args.kP = 2.8;
    args.kI = 0.0005;
    args.kD = 50;
    args.I_max = INT32_MAX;
    turn_args.motor_slew = args.motor_slew;
    turn_args.log_data = args.log_data;
    
    // perform turn
    std::cout << "starting turn\n";
    std::cout << to_turn << "\n";
    t_turn(turn_args);
    std::cout << "turn done\n";
    
    // caclulate distance to move to point
    long double distance = std::sqrt((std::pow(dx, 2) + std::pow(dy, 2)));
    long double to_drive = direction * tracker->to_encoder_ticks(distance, wheel_diameter);
    
    // set up straight drive
    chassis_params drive_straight_args;
    drive_straight_args.setpoint1 = to_drive;
    drive_straight_args.setpoint2 = to_drive;
    drive_straight_args.max_velocity = 125;
    drive_straight_args.timeout = 15000;
    drive_straight_args.kP = .77;
    drive_straight_args.kI = 0.000002;
    drive_straight_args.kD = 7;
    drive_straight_args.I_max = INT32_MAX;
    drive_straight_args.motor_slew = args.motor_slew;
    drive_straight_args.correct_heading = args.correct_heading;
    drive_straight_args.log_data = args.log_data;
    // 
    // std::cout << "starting drive\n";
    // std::cout << to_drive << "\n";
    // drive_straight_args.kP = .2;
    // drive_straight_args.kI = .001;
    // drive_straight_args.kD = 0;
    // drive_straight_args.I_max = 2000;
    t_pid_straight_drive(drive_straight_args);
    
    std::cout << "drive finished\n";
    if(args.log_data) {
        Logger logger;
        log_entry entry;
        entry.content = (
            "[INFO] " + std::string("CHASSIS_ODOM")
            + ", Time: " + std::to_string(pros::millis())
            + ", Waypoint: " + point.get_string()
            + ", ToTurnForwards: " + std::to_string(tracker->to_degrees(to_turn_face_forwards))
            + ", ToTurnBackwards: " + std::to_string(tracker->to_degrees(to_turn_face_backwards))
            + ", ToTurn: " + std::to_string(to_turn)
            + ", ToDrive: " + std::to_string(to_drive)
            + ", Direction: " + std::to_string(direction)
            + ", dx: " + std::to_string(dx)
            + ", dy: " + std::to_string(dy)
            + ", X: " + std::to_string(tracker->get_position().x_pos)
            + ", Y: " + std::to_string(tracker->get_position().y_pos)
            + ", Theta: " + std::to_string(tracker->to_degrees(tracker->get_position().theta))
        );
        entry.stream = "clog";
        logger.add(entry);  
    }
}



int Chassis::pid_straight_drive(double encoder_ticks, int relative_heading /*0*/, int max_velocity /*450*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool correct_heading /*true*/, double slew /*0.2*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = encoder_ticks;
    args.setpoint2 = encoder_ticks;
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.kP = .77;
    args.kI = 0.000002;
    args.kD = 7;
    args.I_max = INT32_MAX;
    args.motor_slew = slew;
    args.correct_heading = correct_heading;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(encoder_ticks) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_pid_straight_drive};
    while ( command_start_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    command_start_lock.exchange( false ); //release lock
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}

int Chassis::profiled_straight_drive(double encoder_ticks, int max_velocity  /*450*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, bool correct_heading /*true*/, int relative_heading /*0*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = encoder_ticks;
    args.setpoint2 = relative_heading;
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.kP = 2;
    args.kI = 0.0005;
    args.kD = 0.001;
    args.I_max = INT32_MAX;
    args.correct_heading = correct_heading;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(encoder_ticks) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_profiled_straight_drive};
    while ( command_start_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    command_start_lock.exchange( false ); //release lock
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;    
}



int Chassis::uneven_drive(double l_enc_ticks, double r_enc_ticks, int max_velocity /*450*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, double slew /*10*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = l_enc_ticks;
    args.setpoint2 = r_enc_ticks;
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.kP = .77;
    args.kI = 0.000002;
    args.kD = 7;
    args.I_max = INT32_MAX;
    args.motor_slew = slew;
    args.correct_heading = false;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(l_enc_ticks) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_pid_straight_drive};
    while ( command_start_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    command_start_lock.exchange( false ); //release lock
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}



int Chassis::turn_right(double degrees, int max_velocity /*450*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, double slew /*15*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = degrees;
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.kP = 2.8;
    args.kI = 0.0005;
    args.kD = 50;
    args.I_max = INT32_MAX;
    args.motor_slew = slew;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(degrees) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_turn};
    while ( command_start_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    command_start_lock.exchange( false ); //release lock
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}



int Chassis::turn_left(double degrees, int max_velocity /*450*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, double slew /*15*/, bool log_data /*false*/) {
    chassis_params args;
    args.setpoint1 = -degrees;
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.kP = 2.8;
    args.kI = 0.0005;
    args.kD = 50;
    args.I_max = INT32_MAX;
    args.motor_slew = slew;
    args.log_data = log_data;

    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(degrees) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_turn};
    while ( command_start_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    command_start_lock.exchange( false ); //release lock
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}


int Chassis::drive_to_point(double x, double y, int recalculations /*0*/, int explicit_direction /*0*/, int max_velocity /*450*/, int timeout /*INT32_MAX*/, bool correct_heading /*true*/, bool asynch /*false*/, double slew /*10*/, bool log_data /*true*/) {
    chassis_params args;
    args.setpoint1 = x;
    args.setpoint2 = y;
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.recalculations = recalculations;
    args.explicit_direction = explicit_direction;
    args.motor_slew = slew;
    args.correct_heading = correct_heading;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(x) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_drive_to_point};
    while ( command_start_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    command_start_lock.exchange( false ); //release lock
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}



int Chassis::turn_to_point(double x, double y, int max_velocity /*450*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, double slew /*10*/, bool log_data /*true*/) {
    chassis_params args;
    args.setpoint1 = x;
    args.setpoint2 = y;
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.motor_slew = slew;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(x) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_turn_to_point};
    while ( command_start_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    command_start_lock.exchange( false ); //release lock
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}



int Chassis::turn_to_angle(double theta, int max_velocity /*450*/, int timeout /*INT32_MAX*/, bool asynch /*false*/, double slew /*10*/, bool log_data /*true*/) {
    PositionTracker* tracker = PositionTracker::get_instance();
    chassis_params args;
    args.setpoint1 = tracker->to_radians(theta);
    args.max_velocity = max_velocity;
    args.timeout = timeout;
    args.motor_slew = slew;
    args.log_data = log_data;
    
    // generate a unique id based on time, parameters, and seemingly random value of the voltage of one of the motors
    int uid = pros::millis() * (std::abs(theta) + 1) + max_velocity + front_left_drive->get_actual_voltage();
    
    chassis_action command = {args, uid, e_turn_to_angle};
    while ( command_start_lock.exchange( true ) ); //aquire lock
    command_queue.push(command);
    command_start_lock.exchange( false ); //release lock
    
    if(!asynch) {
        wait_until_finished(uid);
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


void Chassis::wait_until_finished(int uid) {
    while(std::find(commands_finished.begin(), commands_finished.end(), uid) == commands_finished.end()) {
        pros::delay(10);
    }
    while ( command_finish_lock.exchange( true ) ); //aquire lock
    commands_finished.erase(std::remove(commands_finished.begin(), commands_finished.end(), uid), commands_finished.end()); 
    command_finish_lock.exchange( false ); //release lock
}


bool Chassis::is_finished(int uid) {
    if(std::find(commands_finished.begin(), commands_finished.end(), uid) == commands_finished.end()) {
        return false;  // command is not finished because it is not in the list
    }
    
    // remove command because it is in the list
    while ( command_finish_lock.exchange( true ) ); //aquire lock
    commands_finished.erase(std::remove(commands_finished.begin(), commands_finished.end(), uid), commands_finished.end()); 
    command_finish_lock.exchange( false ); //release lock
    
    return true;
}
