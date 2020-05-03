/**
 * @file: ./RobotCode/src/lcdCode/DriverControl/DriverControlLCD.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: DriverControlLCD.hpp
 *
 * contains methods for driver control lcd
 */

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "../../../Autons.hpp"
#include "DriverControlLCD.hpp"


int DriverControlLCD::auton = 0;

DriverControlLCD::DriverControlLCD()
{
    screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(screen, &gray);
}



DriverControlLCD::~DriverControlLCD()
{
    lv_obj_del(screen);
}




/**
 * updates colors and borders during driver control
 * keeps data relevent
 */
void DriverControlLCD::updateLabels()
{
    Autons auton_data;
    
    //set background
    std::string color = auton_data.AUTONOMOUS_COLORS.at(auton);
    if (color == "blue")
    {
        gray.body.border.color = BLUE_BORDER;
    }
    else if (color == "red")
    {
        gray.body.border.color = RED_BORDER;
    }
    else
    {
        gray.body.border.color = BG;
    }

    lv_scr_load(screen);

    while ( 1 )
    {
        lv_obj_set_style(screen, &gray);
    }
}
