/**
 * @file: ./RobotCode/src/lcdCode/DriverControl/LCDTask.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: deprecate or update, lcd will not run on pros task which is what this is designed to do
 *
 * function to run task for lcd during driver control to update
 *
 */

#ifndef __LCDTASK_HPP__
#define __LCDTASK_HPP__

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "DriverControlLCD.hpp"

/**
 * @param: void* -> not used, required to implement wiht pros task api
 * @return: None
 *
 * @see: DriverControlLCD.hpp
 *
 * task that updates lcd during driver control
 *
 */
void lcd_task(void*);

#endif
