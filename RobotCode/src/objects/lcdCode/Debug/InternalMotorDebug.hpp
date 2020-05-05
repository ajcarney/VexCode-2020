/**
 * @file: ./RobotCode/src/lcdCode/Debug/InternalMotorDebug.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * contains class that for debugging the internal Motor PID constants
 * from the lcd without having to recompile and upload code
 */

#ifndef __INTERNALMOTORDEBUG_HPP__
#define __INTERNALMOTORDEBUG_HPP__

#include "main.h"

#include "../Styles.hpp"
#include "../../../Configuration.hpp"
#include "../../motors/Motor.hpp"


/**
 * @see: ../../motors/Motor.hpp
 *
 * allows user to tune pid constants by logging data and running unit tests
 * with given parameters
 */
class InternalMotorDebug : private Styles
{
    private:
        static bool cont;
        static bool run;
        
        Motor motor;
        
        lv_obj_t *main_screen;
        lv_obj_t *title_label;

    //parameter labels side one
        lv_obj_t *kp_label;
        lv_obj_t *kp_text_area;
        
        lv_obj_t *ki_label;
        lv_obj_t *ki_text_area;
        
        lv_obj_t *kd_label;
        lv_obj_t *kd_text_area;
        
        lv_obj_t *I_max_label;
        lv_obj_t *I_max_text_area;
        
        lv_obj_t *slew_label;
        lv_obj_t *slew_text_area;
        
        lv_obj_t *setpoint_label;
        lv_obj_t *setpoint_text_area;
        
        lv_obj_t *duration_label;
        lv_obj_t *duration_text_area;
        
    //parameter labels side two
        //port
        lv_obj_t *port_label;
        lv_obj_t *port_text_area;
    
        //gearset
        static pros::motor_gearset_e_t current_gearset;
        lv_obj_t *gearset_label;
        lv_obj_t *ddlist_gearset;
        
        /**
         * @param: lv_obj_t* ddlist -> the dropdown list object for the callback function
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * sets the brake mode for the motor set which will be updated in the main loop
         */
        static lv_res_t ddlist_gearset_action(lv_obj_t *ddlist);
        
        //brake mode
        static pros::motor_brake_mode_e_t current_brake_mode;
        lv_obj_t *brakemode_label;
        lv_obj_t *ddlist_brake_mode;
        
        /**
         * @param: lv_obj_t* ddlist -> the dropdown list object for the callback function
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * sets the brake mode for the motor set which will be updated in the main loop
         */
        static lv_res_t ddlist_brake_mode_action(lv_obj_t *ddlist);
        
    //information label 
        lv_obj_t *information_label;

    //keyboard 
        
        lv_obj_t *keyboard;
        

    //back button
        lv_obj_t *btn_back;
        lv_obj_t *btn_back_label;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to set cont to false meaning the
         * user wants to go to the title screen
         */
        static lv_res_t btn_back_action(lv_obj_t *btn);
        
        
    //run unit test button
        lv_obj_t *btn_run;
        lv_obj_t *btn_run_label;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button to run unit_test and log data with the given parameters
         */
        static lv_res_t btn_run_action(lv_obj_t *btn);
        
    //actual unit test function   
        /**
         * @return: int -> 1 if motor was successfully tested, 0 otherwise
         *
         * reads values from text areas and performs a unit test with the 
         * given parameters
         */
        int run_unit_test( ); 
        

    public:
        InternalMotorDebug();
        ~InternalMotorDebug();

        /**
         * @return: None
         *
         * start debugger
         */
        void debug();

};



#endif
