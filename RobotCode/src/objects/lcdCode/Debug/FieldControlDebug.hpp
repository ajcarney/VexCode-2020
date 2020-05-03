/**
 * @file: ./RobotCode/src/lcdCode/Debug/FieldControlDebug.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/16/2019
 * @reviewed_by: Aiden Carney
 *
 * contains class with methods that allow the user to see info about the state
 * od the field control
 */

#ifndef __FIELDCONTROLDEBUG_HPP__
#define __FIELDCONTROLDEBUG_HPP__


#include "../../../../include/main.h"

#include "../Styles.hpp"


/**
 * @see: ../Styles.hpp
 *
 * contains methods that show user data about the field control
 */
class FieldControlDebug : private Styles
{
    private:
        lv_obj_t *field_ctrl_screen;
        lv_obj_t *title_label;

        lv_obj_t *labels_label;
        lv_obj_t *info_label;

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

    public:
        static bool cont;

        FieldControlDebug();
        ~FieldControlDebug();


        /**
         * @return: None
         * TODO: use ternary operator to condense and make more readable
         *
         * allows user to see information about the state of field control
         */
        void debug();

};




#endif
