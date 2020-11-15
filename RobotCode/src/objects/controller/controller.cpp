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



std::unordered_map <pros::controller_digital_e_t, std::deque<bool>* > Controller::master_btn_history = {
    {pros::E_CONTROLLER_DIGITAL_L1, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_L2, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_R2, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_R1, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_UP, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_DOWN, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_LEFT, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_RIGHT, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_X, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_B, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_Y, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_A, new std::deque<bool>}
};

std::unordered_map <pros::controller_digital_e_t, std::deque<bool>* > Controller::partner_btn_history = {
    {pros::E_CONTROLLER_DIGITAL_L1, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_L2, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_R2, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_R1, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_UP, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_DOWN, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_LEFT, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_RIGHT, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_X, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_B, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_Y, new std::deque<bool>},
    {pros::E_CONTROLLER_DIGITAL_A, new std::deque<bool>}
};

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
    {pros::E_CONTROLLER_DIGITAL_L2, "lift down"},
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


void Controller::update_button_history()
{
    for (std::pair<pros::controller_digital_e_t, std::deque<bool>*> element : master_btn_history) {
        element.second->push_back(master.get_digital(element.first));
        if(element.second->size() > 3) {
            element.second->pop_front();
        } 
        
        while(element.second->size() < 3) {  // forces deque size to be no less than 3
            element.second->push_back(master.get_digital(element.first));
        }
    }
    
    for (std::pair<pros::controller_digital_e_t, std::deque<bool>*> element : partner_btn_history) {
        element.second->push_back(master.get_digital(element.first));
        if(element.second->size() > 3) {
            element.second->pop_front();
        }
        
        while(element.second->size() < 3) {  // forces deque size to be no less than 3
            element.second->push_back(master.get_digital(element.first));
        }
    }
}


bool Controller::btn_get_release(pros::controller_digital_e_t btn, int controller /** 0 **/) {
    bool pressed_and_released = false;
    if(!controller) {
        if(master_btn_history.at(btn)->at(1) and !master_btn_history.at(btn)->at(2)) {
            pressed_and_released = true;
        }
    }
    else {
        if(partner_btn_history.at(btn)->at(1) and !partner_btn_history.at(btn)->at(2)) {
            pressed_and_released = true;
        }
    }
    return pressed_and_released;
}


bool Controller::btn_get_start_press(pros::controller_digital_e_t btn, int controller /** 0 **/) {
    bool press_start = false;
    if(!controller) {
        if(!master_btn_history.at(btn)->at(1) and master_btn_history.at(btn)->at(2)) {
            press_start = true;
        }
    }
    else {
        if(!partner_btn_history.at(btn)->at(1) and partner_btn_history.at(btn)->at(2)) {
            press_start = true;
        }
    }

    return press_start;
}


bool Controller::btn_is_pressing(pros::controller_digital_e_t btn, int controller /** 0 **/) {
    bool pressing = false;
    if(!controller) {
        pressing = master_btn_history.at(btn)->at(2);
    }
    else {
        pressing = partner_btn_history.at(btn)->at(2);
    }

    return pressing;
}
