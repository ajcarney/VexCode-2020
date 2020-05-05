/**
 * @file: ./RobotCode/src/DriverControl.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * Contains robot move functions. Meant to be run in pros task
 *
 */

#ifndef __DRIVERCONTROL_HPP__
#define __DRIVERCONTROL_HPP__

#include <cstdlib>

#include "../include/main.h"

#include "objects/subsystems/chassis.hpp"
#include "objects/controller/controller.hpp"
#include "objects/motors/Motors.hpp"

#define AUTON_DEBUG


/**
 * @param: void* -> not used
 * @return: None
 *
 * @see: Motors.hpp
 * @see: Controller.hpp
 *
 * meant to be run on task
 * function cycles through and allows user to controll robot
 *
 */
void driver_control(void*);


#endif
