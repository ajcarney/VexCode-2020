/**
 * @file: ./RobotCode/src/lcdCode/AutonSelecton/OptionsScreen.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: add correct options when they are decided on, deprecate static options to reduce coupling
 *
 * contains class with methods to decide on options for auton
 *
 */

#ifndef __OPTIONSSCREEN_HPP__
#define __OPTIONSSCREEN_HPP__


#include "../../../../include/main.h"

#include "../../../Autons.hpp"
#include "../Styles.hpp"

/**
 * @see: ../Styles.hpp
 * @see: ./AutonSelection
 * @see: ../gui.hpp
 *
 * contains methods to get options for auton period
 */
class OptionsScreen : private Styles
{
    private:
        //screen
        lv_obj_t *options_screen;

        //labels
        lv_obj_t *title_label;

        lv_obj_t *sw_use_hardcoded_label;
        lv_obj_t *sw_gyro_turn_label;
        lv_obj_t *sw_accelleration_ctrl_label;
        lv_obj_t *sw_check_motor_tmp_label;
        lv_obj_t *sw_use_previous_macros_label;
        lv_obj_t *sw_record_label;

        //buttons
        lv_obj_t *btn_confirm;
        lv_obj_t *btn_back;

        //button labels
        lv_obj_t *btn_confirm_label;
        lv_obj_t *btn_back_label;


        //switches
        lv_obj_t *sw_use_hardcoded;
        lv_obj_t *sw_gyro_turn;
        lv_obj_t *sw_accelleration_ctrl;
        lv_obj_t *sw_check_motor_tmp;
        lv_obj_t *sw_use_previous_macros;
        lv_obj_t *sw_record;


    public:
        OptionsScreen();
        ~OptionsScreen();

        static autonConfig cnfg;
        static bool nextScreen;
        static bool back;


        //button functions

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to set variable so that gui continues
         * to the next stage
         */
        static lv_res_t btn_confirm_action(lv_obj_t *btn);

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to set variable so that gui goes back to
         * the previous stage
         */
        static lv_res_t btn_back_action(lv_obj_t *btn);


        //switch functions

        /**
         * @param: lv_obj_t* sw -> switch object that was selected
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         * TODO: merge with other functions to condense code and make it more expandable
         *
         * sets configuration option for using a compiled auton vs auton written on sd card
         */
        static lv_res_t sw_use_hardcoded_action(lv_obj_t *sw);

        /**
         * @param: lv_obj_t* sw -> switch object that was selected
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         * TODO: merge with other functions to condense code and make it more expandable
         *
         * sets configuration option for using gyro turns in auton
         */
        static lv_res_t sw_gyro_turn_action(lv_obj_t *sw);

        /**
         * @param: lv_obj_t* sw -> switch object that was selected
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         * TODO: merge with other functions to condense code and make it more expandable
         *
         * sets configuration option for using acceleration control code
         */
        static lv_res_t sw_accelleration_ctrl_action(lv_obj_t *sw);

        /**
         * @param: lv_obj_t* sw -> switch object that was selected
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         * TODO: merge with other functions to condense code and make it more expandable
         *
         * sets configuration option for limiting motor output based on temperature during the match
         */
        static lv_res_t sw_check_motor_tmp_action(lv_obj_t *sw);

        /**
         * @param: lv_obj_t* sw -> switch object that was selected
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         * TODO: merge with other functions to condense code and make it more expandable
         *
         * sets configuration option for allowing the use of previously recorded macros
         */
        static lv_res_t sw_use_previous_macros_action(lv_obj_t *sw);

        /**
         * @param: lv_obj_t* sw -> switch object that was selected
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         * TODO: merge with other functions to condense code and make it more expandable
         *
         * sets configuration option for recording match in the macro format
         */
        static lv_res_t sw_record_action(lv_obj_t *sw);



        /**
         * @param: int auton -> number of auton selected, used to set color background of lcd
         * @return: autonConfig -> configuration struct with options based on how the switches were set
         *
         * @see: ../Structs.hpp
         * @see: ../Gui.hpp
         *
         * allows user to interact with the switches to set configuration options
         * user can choose to go back or continue with the options selected
         */
        autonConfig getOptions( int auton );

};


#endif
