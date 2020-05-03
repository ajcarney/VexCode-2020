/**
 * @file: ./RobotCode/src/lcdCode/AutonSelecton/SelectionScreen.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: add ability for controller to make selections
 *
 * contains first stage of auton selection--selecting the auton number
 *
 */

#ifndef __SELECTIONSCREEN_HPP__
#define __SELECTIONSCREEN_HPP__


#include "../../../../include/main.h"

#include "../Styles.hpp"




/**
 * @see: ../Styles.hpp
 *
 * contains methods for going through each auton and providing description
 * for user
 */
class SelectionScreen : private Styles
{
    private:

        //labels
        lv_obj_t *title_label;
        lv_obj_t *description_label;
        lv_obj_t *auton_number_label;

        //buttons
        lv_obj_t *btn_right;
        lv_obj_t *btn_left;
        lv_obj_t *btn_select;

        //button labels
        lv_obj_t *btn_right_label;
        lv_obj_t *btn_left_label;
        lv_obj_t *btn_select_label;


    public:
        //screens
        static lv_obj_t *selection_screen;
        //variables
        static int auton_choice;
        static int final_choice;
        static bool update;

        //button action functions

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to set variable so that gui moves
         * autons to the right (increasing auton number by one and looping at end)
         */
        static lv_res_t btn_right_action(lv_obj_t *btn);


        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to set variable so that gui moves
         * autons to the left (decreasing auton number by one and looping at end)
         */
        static lv_res_t btn_left_action(lv_obj_t *btn);


        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to select the auton so that the gui continues
         * to the next stage
         */
        static lv_res_t btn_select_action(lv_obj_t *btn);

        SelectionScreen();
        ~SelectionScreen();



        /**
         * @return: None
         *
         * @see: int selectAuton()
         * @see: ../Structs.hpp
         *
         * sets the description, color, title, and auton number on the screen
         * used to update the auton selection based on the current number
         */
        void showSelection();


        /**
         * @param: int auton -> auton number to start the screen at
         * @return: int -> auton number that the screen was on when the user hit the select button
         *
         * loops through waiting for the auton number to change to update the screen
         */
        int selectAuton( int auton );


};



#endif
