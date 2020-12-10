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

#include "main.h"

#include "../Styles.hpp"


/**
 * @see: ../Styles.hpp
 *
 * contains lcd to be used during driver control
 */
class DriverControlLCD : private Styles
{
    private:
        static bool log_data;
        static bool open_debugger;
        static std::string toggle_logging_text;
        
        lv_obj_t *screen;
        
        lv_obj_t *logo_img;
        
        
        //labels
        lv_obj_t *title_label;
        lv_obj_t *queue_size_label;
        
        //buttons
        lv_obj_t *btn_debugger;
        lv_obj_t *btn_run_auton;
        lv_obj_t *btn_toggle_logging;
        lv_obj_t *btn_flush_queue;

        lv_obj_t *btn_debugger_label;
        lv_obj_t *btn_run_auton_label;
        lv_obj_t *btn_toggle_logging_label;
        lv_obj_t *btn_flush_queue_label;


        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to open debugger
         */
        static lv_res_t btn_debugger_action(lv_obj_t *btn);


        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to run auton
         */
        static lv_res_t btn_run_auton_action(lv_obj_t *btn);


        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to toggle logging of motors and other various items
         */
        static lv_res_t btn_toggle_logging_action(lv_obj_t *btn);
        
        
        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to flush the logging queue
         */
        static lv_res_t btn_flush_queue_action(lv_obj_t *btn);


    public:
        DriverControlLCD();
        ~DriverControlLCD();


        /**
         * @return: None
         *
         * TODO: add actual content to be updated
         *
         * function to be used to update the gui to keep data relevant
         */
        void update_labels();


};

#endif
