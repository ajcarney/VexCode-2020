/**
 * @file: ./RobotCode/src/controller/controller.hpp
 * @author: Aiden Carney
 * @reviewed_on: 11/8/19
 * @reviewed_by: Aiden Carney
 * TODO: remove static memebers and replace with singleton class
 *
 * contains class that has data about controllers
 */

#include <unordered_map>
#include <string>

#include "../../../include/main.h"
#include "../../../include/api.h"
#include "../../../include/pros/rtos.hpp"
#include "../../../include/pros/motors.hpp"

#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

/**
 * contains controller objects as well as unordered maps that hold information
 * about what the controller does
 * TODO: move unordere maps to configuration and make more robust so it does not have to be updated
 */
class Controller
{
    private:


    public:
        Controller();
        ~Controller();

        static pros::Controller master;
        static pros::Controller partner;

        static std::unordered_map <pros::controller_analog_e_t, std::string> MASTER_CONTROLLER_ANALOG_MAPPINGS;
        static std::unordered_map <pros::controller_analog_e_t, std::string> PARTNER_CONTROLLER_ANALOG_MAPPINGS;
        static std::unordered_map <pros::controller_digital_e_t, std::string> MASTER_CONTROLLER_DIGITAL_MAPPINGS;
        static std::unordered_map <pros::controller_digital_e_t, std::string>  PARTNER_CONTROLLER_DIGITAL_MAPPINGS;

};




#endif
