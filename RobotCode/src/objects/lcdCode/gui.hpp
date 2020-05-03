/**
 * @file: ./RobotCode/src/objects/lcdCode/gui.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: clean up conditionals, add config file
 *
 * contains auton selector gui selection all put together in one function
 *
 */
#ifndef __GUI_HPP__
#define __GUI_HPP__


#include "../include/main.h"

#include "AutonSelection/SelectionScreen.hpp"
#include "AutonSelection/OptionsScreen.hpp"
#include "AutonSelection/PrepScreen.hpp"
#include "AutonSelection/ActionsScreen.hpp"
#include "DriverControl/DriverControlLCD.hpp"
#include "../../DriverControl.hpp"
#include "Debug/Debug.hpp"
#include "TemporaryScreen.hpp"


/**
 * @return: int -> number of auton selected
 *
 * @see: ./AutonSelection
 * @see: ./Debug
 *
 * TODO: add more meaningful config options, clean up conditionals
 *
 * iterates and interacts with user to find final auton choice, and config options
 *
 */
int chooseAuton();





#endif
