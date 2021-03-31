/**
 * @file: ./RobotCode/src/motors/Motors.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * contains definition of global struct
 */
 
#include "Motors.hpp"
#include "MotorThread.hpp"

namespace Motors
{
    Motor front_right {Configuration::get_instance()->front_right_port, pros::E_MOTOR_GEARSET_06, Configuration::get_instance()->front_right_reversed};
    Motor front_left {Configuration::get_instance()->front_left_port, pros::E_MOTOR_GEARSET_06, Configuration::get_instance()->front_left_reversed};
    Motor back_right {Configuration::get_instance()->back_right_port, pros::E_MOTOR_GEARSET_06, Configuration::get_instance()->back_right_reversed};
    Motor back_left {Configuration::get_instance()->back_left_port, pros::E_MOTOR_GEARSET_06, Configuration::get_instance()->back_left_reversed};
    Motor left_intake {Configuration::get_instance()->left_intake_port, pros::E_MOTOR_GEARSET_06, Configuration::get_instance()->left_intake_reversed};
    Motor right_intake {Configuration::get_instance()->right_intake_port, pros::E_MOTOR_GEARSET_06, Configuration::get_instance()->right_intake_reversed};
    Motor upper_indexer {Configuration::get_instance()->upper_indexer_port, pros::E_MOTOR_GEARSET_06, Configuration::get_instance()->upper_indexer_reversed};
    Motor lower_indexer {Configuration::get_instance()->lower_indexer_port, pros::E_MOTOR_GEARSET_06, Configuration::get_instance()->lower_indexer_reversed};

    double chassis_gear_ratio = 3 / 5;

    std::array<Motor*, 8> motor_array = {
        &front_right,
        &front_left,
        &back_right,
        &back_left,
        &left_intake,
        &right_intake,
        &upper_indexer,
        &lower_indexer,        
    };
    
    std::array<std::string, 8> motor_names_array = {
        "Front Left",
        "Front Right",
        "Back Left",
        "Back Right",
        "Left Intake",
        "Right Intake",
        "Upper Indexer",
        "Lower Indexer"    
    };

    void enable_driver_control() {
        Motors::front_left.enable_driver_control();
        Motors::front_left.enable_driver_control();
        Motors::back_right.enable_driver_control();
        Motors::back_left.enable_driver_control();
        Motors::left_intake.enable_driver_control();
        Motors::right_intake.enable_driver_control();
        Motors::upper_indexer.enable_driver_control();
        Motors::lower_indexer.enable_driver_control();
    }
    
    void disable_driver_control() {
        Motors::front_left.disable_driver_control();
        Motors::front_left.disable_driver_control();
        Motors::back_right.disable_driver_control();
        Motors::back_left.disable_driver_control();
        Motors::left_intake.disable_driver_control();
        Motors::right_intake.disable_driver_control();
        Motors::upper_indexer.disable_driver_control();
        Motors::lower_indexer.disable_driver_control();
    }
    
    void set_brake_mode(pros::motor_brake_mode_e_t new_brakemode) {
        Motors::front_left.set_brake_mode(new_brakemode);
        Motors::front_left.set_brake_mode(new_brakemode);
        Motors::back_right.set_brake_mode(new_brakemode);
        Motors::back_left.set_brake_mode(new_brakemode);
        Motors::left_intake.set_brake_mode(new_brakemode);
        Motors::right_intake.set_brake_mode(new_brakemode);
        Motors::upper_indexer.set_brake_mode(new_brakemode);    
        Motors::lower_indexer.set_brake_mode(new_brakemode);        
    }
    
    void stop_all_motors() {
        Motors::front_left.move(0);
        Motors::front_left.move(0);
        Motors::back_right.move(0);
        Motors::back_left.move(0);
        Motors::left_intake.move(0);
        Motors::right_intake.move(0);
        Motors::upper_indexer.move(0);  
        Motors::lower_indexer.move(0);         
    }
    
    void set_log_level(int log_level) {
        Motors::front_right.set_log_level(log_level);
        Motors::front_left.set_log_level(log_level);
        Motors::back_right.set_log_level(log_level);
        Motors::back_left.set_log_level(log_level);
        Motors::left_intake.set_log_level(log_level);
        Motors::right_intake.set_log_level(log_level);
        Motors::upper_indexer.set_log_level(log_level);
        Motors::lower_indexer.set_log_level(log_level);
    }
    
    void register_motors() {
        MotorThread* motor_thread = MotorThread::get_instance();
        motor_thread->register_motor(Motors::front_right);
        motor_thread->register_motor(Motors::front_left);
        motor_thread->register_motor(Motors::back_right);
        motor_thread->register_motor(Motors::back_left);
        motor_thread->register_motor(Motors::left_intake);
        motor_thread->register_motor(Motors::right_intake);
        motor_thread->register_motor(Motors::upper_indexer);
        motor_thread->register_motor(Motors::lower_indexer);
    }
    
    void unregister_motors() {
        MotorThread* motor_thread = MotorThread::get_instance();
        motor_thread->unregister_motor(Motors::front_right);
        motor_thread->unregister_motor(Motors::front_left);
        motor_thread->unregister_motor(Motors::back_right);
        motor_thread->unregister_motor(Motors::back_left);
        motor_thread->unregister_motor(Motors::left_intake);
        motor_thread->unregister_motor(Motors::right_intake);
        motor_thread->unregister_motor(Motors::upper_indexer);
        motor_thread->unregister_motor(Motors::lower_indexer);
    }
    
};
