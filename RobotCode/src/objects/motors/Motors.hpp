/**
 * @file: ./RobotCode/src/motors/Motors.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * contains global struct for all motors
 */

#ifndef __MOTORS_HPP__
#define __MOTORS_HPP__

#include "main.h"

#include "../../Configuration.hpp"
#include "Motor.hpp"


namespace Motors
{
    extern Motor front_right;
    extern Motor front_left;
    extern Motor back_right;
    extern Motor back_left;
    extern Motor main_intake;
    extern Motor hoarding_intake;
    extern Motor lift;
    
    void enable_driver_control();
    void disable_driver_control();
    void set_log_level(int log_level);
    void register_motors();
    void unregister_motors();
};


#endif
