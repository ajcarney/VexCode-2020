/**
 * @file: ./RobotCode/src/DriverControl.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: add more robot functions
 *
 * Contains robot move functions. Meant to be run in pros task
 *
 */

#ifndef __DRIVERCONTROL_HPP__
#define __DRIVERCONTROL_HPP__

#include <cstdlib>

#include "../include/main.h"

#include "objects/robotChassis/chassis.hpp"
#include "objects/controller/controller.hpp"
#include "objects/motors/Motors.hpp"



/**
 * @param: void* -> not used
 * @return: None
 *
 * @see: Tilter.hpp
 *
 * gives the tilter holding strength for a set potentiometer reading
 * useful because the tilter is rubber banded and needs to be held backwards
 */
void tilter_holding_strength(void*);


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
