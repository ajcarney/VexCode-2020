/**
 * @file: ./RobotCode/src/controller/controller.cpp
 * @author: Aiden Carney
 * @reviewed_on: 11/8/19
 * @reviewed_by: Aiden Carney
 *
 * @see: controller.hpp
 *
 * contains definitions for static members of class
 */

#include <unordered_map>
#include <string>

#include "../../../include/main.h"
#include "../../../include/api.h"
#include "../../../include/pros/rtos.hpp"
#include "../../../include/pros/motors.hpp"

#include "controller.hpp"



pros::Controller Controller::master(pros::E_CONTROLLER_MASTER);
pros::Controller Controller::partner(pros::E_CONTROLLER_PARTNER);

//mappings for each controller
std::unordered_map <pros::controller_analog_e_t, std::string> Controller::MASTER_CONTROLLER_ANALOG_MAPPINGS = {
    {pros::E_CONTROLLER_ANALOG_LEFT_X, "None"},
    {pros::E_CONTROLLER_ANALOG_LEFT_Y, "Left Side Chassis"},
    {pros::E_CONTROLLER_ANALOG_RIGHT_X, "None"},
    {pros::E_CONTROLLER_ANALOG_RIGHT_Y, "Right Side Chassis"}
};

std::unordered_map <pros::controller_analog_e_t, std::string> Controller::PARTNER_CONTROLLER_ANALOG_MAPPINGS = {
    {pros::E_CONTROLLER_ANALOG_LEFT_X, "None"},
    {pros::E_CONTROLLER_ANALOG_LEFT_Y, "None"},
    {pros::E_CONTROLLER_ANALOG_RIGHT_X, "None"},
    {pros::E_CONTROLLER_ANALOG_RIGHT_Y, "None"}
};

std::unordered_map <pros::controller_digital_e_t, std::string> Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS = {
    {pros::E_CONTROLLER_DIGITAL_L1, "lift up"},
    {pros::E_CONTROLLER_DIGITAL_L2, "lift dwon"},
    {pros::E_CONTROLLER_DIGITAL_R2, "Outake"},
    {pros::E_CONTROLLER_DIGITAL_R1, "Intake"},
    {pros::E_CONTROLLER_DIGITAL_UP, "Tilter up"},
    {pros::E_CONTROLLER_DIGITAL_DOWN, "Tilter down"},
    {pros::E_CONTROLLER_DIGITAL_LEFT, "None"},
    {pros::E_CONTROLLER_DIGITAL_RIGHT, "None"},
    {pros::E_CONTROLLER_DIGITAL_X, "auto deploy"},
    {pros::E_CONTROLLER_DIGITAL_B, "Toggle brakes"},
    {pros::E_CONTROLLER_DIGITAL_Y, "auto dump"},
    {pros::E_CONTROLLER_DIGITAL_A,  "run auton"}
};

std::unordered_map <pros::controller_digital_e_t, std::string>  Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS = {
    {pros::E_CONTROLLER_DIGITAL_L1, "None"},
    {pros::E_CONTROLLER_DIGITAL_L2, "None"},
    {pros::E_CONTROLLER_DIGITAL_R2, "None"},
    {pros::E_CONTROLLER_DIGITAL_R1, "None"},
    {pros::E_CONTROLLER_DIGITAL_UP, "None"},
    {pros::E_CONTROLLER_DIGITAL_DOWN, "None"},
    {pros::E_CONTROLLER_DIGITAL_LEFT, "None"},
    {pros::E_CONTROLLER_DIGITAL_RIGHT, "None"},
    {pros::E_CONTROLLER_DIGITAL_X, "None"},
    {pros::E_CONTROLLER_DIGITAL_B, "None"},
    {pros::E_CONTROLLER_DIGITAL_Y, "None"},
    {pros::E_CONTROLLER_DIGITAL_A,  "None"}
};




Controller::Controller()
{

}

Controller::~Controller()
{

}
