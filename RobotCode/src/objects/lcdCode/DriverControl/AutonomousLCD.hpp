/**
 * @file: ./RobotCode/src/lcdCode/DriverControl/AutonomousLCD.hpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 *
 * contains the lcd screen used during auton
 *
 */
#ifndef __AUTONOMOUSLCD_HPP__
#define __AUTONOMOUSLCD_HPP__

#include "main.h"

#include "../Styles.hpp"


/**
 * @see: ../Styles.hpp
 *
 * contains lcd to be used during driver control
 */
class AutonomousLCD : private Styles
{
    private:
        lv_obj_t *screen;
        
        lv_obj_t *logo_img;
        
        
        //labels
        lv_obj_t *title_label;
        lv_obj_t *auton_label;
        lv_obj_t *description_label;


    public:
        AutonomousLCD();
        ~AutonomousLCD();


        /**
         * @param: int auton_number -> the autonomous number
         * @return: None
         *
         * TODO: add actual content to be updated
         *
         * function to be used to update the gui to keep data relevant
         */
        void update_labels(int auton_number);
        
        
        void log_to_lcd(std::string msg);


};

#endif
