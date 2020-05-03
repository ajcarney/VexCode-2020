/**
 * @file: ./RobotCode/src/lcdCode/DriverControl/LCDTask.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: LCDTask.hpp
 *
 * description of contents line 1
 * description of contents line 2
 * description of contents line 3
 *
 */

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "DriverControlLCD.hpp"
#include "LCDTask.hpp"


/**
 * runs loop to update lcd in a task
 */
void lcd_task(void*)
{
    DriverControlLCD mainLCD;

    while ( 1 )
    {
        mainLCD.updateLabels();
        pros::delay(100); //high number so that the majority of time spent
                          //on this thread will be low so that time can be used
                          //for more useful things like recording
        std::cout << lv_scr_act() << "\n";
        pros::delay(100);
    }
}
