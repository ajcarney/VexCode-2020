/**
 * @file: ./RobotCode/src/lcdCode/AutonSelecton/PrepScreen.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: add actual preparation steps text
 * TODO: decouple initilization string, make it more configurable
 *
 * contains class with methods for showing the things that will occur
 * before the auton is run
 */

#ifndef __PREP_SCREEN__
#define __PREP_SCREEN__


#include "../include/main.h"

#include "../Styles.hpp"
#include "OptionsScreen.hpp"


/**
 * @see: ../Styles.hpp
 * @see: ../gui.hpp
 *
 * final confirmation steps for auton
 * shows the initialization that will occur after the user clicks continue
 */
class PrepScreen : private Styles
{
    private:
        //screen
        lv_obj_t *prep_screen;

        //labels
        lv_obj_t *title_label;

        lv_obj_t *actions_label;

        //buttons
        lv_obj_t *btn_confirm;
        lv_obj_t *btn_back;

        //button labels
        lv_obj_t *btn_confirm_label;
        lv_obj_t *btn_back_label;


    public:
        PrepScreen();
        ~PrepScreen();

        static bool nextScreen;
        static bool confirm;

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
         * button callback function used to set variable so that gui goes
         * to the previous stage
         */
        static lv_res_t btn_back_action(lv_obj_t *btn);


        /**
         * @param: int auton -> auton number selected, used to set border color of gui based on side of color the auton is run on
         * @return: None
         *
         * @see: ../Structs.hpp
         * @see: ../Styles.hpp
         *
         * gunction used to get confirmation from user to continue to next stage
         * of the selection process
         */
        void getConfirmation( int auton );

};
#endif
