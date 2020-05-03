/**
 * @file: ./RobotCode/src/lcdCode/DriverControl/DriverControlLCD.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: add actual content instead of blank screen
 *
 * contains the lcd screen used during driver control
 *
 */
#ifndef __DRIVERCONTROLLCD_HPP__
#define __DRIVERCONTROLLCD_HPP__

#include "../../../../include/main.h"

#include "../Styles.hpp"


/**
 * @see: ../Styles.hpp
 *
 * contains lcd to be used during driver control
 */
class DriverControlLCD : private Styles
{
    private:
        lv_obj_t *screen;

        //labels
        lv_obj_t *title_label;


    public:
        DriverControlLCD();
        ~DriverControlLCD();

        static int auton;

        /**
         * @return: None
         *
         * TODO: add actual content to be updated
         *
         * function to be used to update the gui to keep data relevant
         */
        void updateLabels();


};

#endif
