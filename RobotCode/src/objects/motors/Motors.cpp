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
    Motor front_right {Configuration::get_instance()->front_right_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->front_right_reversed};
    Motor front_left {Configuration::get_instance()->front_left_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->front_left_reversed};
    Motor back_right {Configuration::get_instance()->back_right_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->back_right_reversed};
    Motor back_left {Configuration::get_instance()->back_left_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->back_left_reversed};
    Motor main_intake {Configuration::get_instance()->main_intake_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->main_intake_reversed};
    Motor hoarding_intake {Configuration::get_instance()->hoarding_intake_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->hoarding_intake_reversed};
    Motor lift {Configuration::get_instance()->lift_port, pros::E_MOTOR_GEARSET_36, Configuration::get_instance()->lift_reversed};

    std::array<Motor*, 7> motor_array = {
        &front_right,
        &front_left,
        &back_right,
        &back_left,
        &main_intake,
        &hoarding_intake,
        &lift        
    };

    void enable_driver_control() {
        Motors::front_left.enable_driver_control();
        Motors::front_left.enable_driver_control();
        Motors::back_right.enable_driver_control();
        Motors::back_left.enable_driver_control();
        Motors::main_intake.enable_driver_control();
        Motors::hoarding_intake.enable_driver_control();
        Motors::lift.enable_driver_control();
    }
    
    void disable_driver_control() {
        Motors::front_left.disable_driver_control();
        Motors::front_left.disable_driver_control();
        Motors::back_right.disable_driver_control();
        Motors::back_left.disable_driver_control();
        Motors::main_intake.disable_driver_control();
        Motors::hoarding_intake.disable_driver_control();
        Motors::lift.disable_driver_control();
    }
    
    void set_log_level(int log_level) {
        Motors::front_right.set_log_level(log_level);
        Motors::front_left.set_log_level(log_level);
        Motors::back_right.set_log_level(log_level);
        Motors::back_left.set_log_level(log_level);
        Motors::main_intake.set_log_level(log_level);
        Motors::hoarding_intake.set_log_level(log_level);
        Motors::lift.set_log_level(log_level);
    }
    
    void register_motors() {
        MotorThread::get_instance()->register_motor(Motors::front_right);
        MotorThread::get_instance()->register_motor(Motors::front_left);
        MotorThread::get_instance()->register_motor(Motors::back_right);
        MotorThread::get_instance()->register_motor(Motors::back_left);
        MotorThread::get_instance()->register_motor(Motors::main_intake);
        MotorThread::get_instance()->register_motor(Motors::hoarding_intake);
        MotorThread::get_instance()->register_motor(Motors::lift);
    }
    
    void unregister_motors() {
        MotorThread::get_instance()->unregister_motor(Motors::front_right);
        MotorThread::get_instance()->unregister_motor(Motors::front_left);
        MotorThread::get_instance()->unregister_motor(Motors::back_right);
        MotorThread::get_instance()->unregister_motor(Motors::back_left);
        MotorThread::get_instance()->unregister_motor(Motors::main_intake);
        MotorThread::get_instance()->unregister_motor(Motors::hoarding_intake);
        MotorThread::get_instance()->unregister_motor(Motors::lift);
    }
};
