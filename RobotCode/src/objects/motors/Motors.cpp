/**
 * @file: ./RobotCode/src/motors/Motors.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * contains definition of global struct
 */
 
#include "Motors.hpp"

namespace Motors
{
    Motor front_right {Configuration::get_instance()->front_right_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->front_right_reversed};
    Motor front_left {Configuration::get_instance()->front_left_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->front_left_reversed};
    Motor back_right {Configuration::get_instance()->back_right_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->back_right_reversed};
    Motor back_left {Configuration::get_instance()->back_left_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->back_left_reversed};
    Motor right_intake {Configuration::get_instance()->right_intake_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->right_intake_reversed};
    Motor left_intake {Configuration::get_instance()->left_intake_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->left_intake_reversed};
    Motor tilter {Configuration::get_instance()->tilter_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->tilter_reversed};
    Motor lift {Configuration::get_instance()->lift_port, pros::E_MOTOR_GEARSET_36, Configuration::get_instance()->lift_reversed};
};
