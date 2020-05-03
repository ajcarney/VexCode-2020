/**
 * @file: ./RobotCode/src/lcdCode/Debug/Debug.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: move starting of driver control task to this function because that task should be available during the debugger session
 *
 * gives user the option to visit debugger tabs by selecting an option from a button
 * matrix
 */

#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#include "BatteryDebug.hpp"
#include "ControllerDebug.hpp"
#include "FieldControlDebug.hpp"
#include "InternalMotorDebug.hpp"
#include "MotorsDebug.hpp"
#include "SensorsDebug.hpp"
#include "TitleScreen.hpp"
#include "Wiring.hpp"



/**
 * @return: None
 *
 * @see: TitleScreen.hpp
 *
 * loads screens and switches the debugger option based on a what is clicked from
 * a button matrix
 */
void debug();


#endif
