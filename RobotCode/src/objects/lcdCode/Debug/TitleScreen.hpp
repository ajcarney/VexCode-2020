/**
 * @file: ./RobotCode/src/lcdCode/Debug/TitleScreen.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * contains class that allows user to select a debug tab
 *
 */

#ifndef __TITLESCREEN_HPP__
#define __TITLESCREEN_HPP__


#include "../../../../include/main.h"

#include "../Styles.hpp"


/**
 * @see: ../Styles.hpp
 * @see: Debug.hpp
 *
 * contains button matrix that has different debug tabs on it
 */
class TitleScreen : private Styles
{
    private:
        //screen
        lv_obj_t *title_screen;

        lv_obj_t *title_label;

        lv_obj_t *btn_back;
        lv_obj_t *btn_back_label;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to set the debug option to -1 meaning the
         * user wants to go to the previous screen
         */
        static lv_res_t btn_back_action(lv_obj_t *btn);


        lv_obj_t *button_matrix; //button matrix object
        static const char* btnm_map[]; //map for button matrix

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to set option of debug screen that user wants to go to
         */
        static lv_res_t button_matrix_action(lv_obj_t *btnm, const char *btn_txt);


    public:
        TitleScreen();
        ~TitleScreen();

        static int option;

        /**
         * @return: None
         *
         * @see: btn_back_action
         * @see: button_matrix_action
         *
         * loads screen and waits in a loop with a delay for user to select
         * a button
         */
        void chooseOption();

};

#endif
