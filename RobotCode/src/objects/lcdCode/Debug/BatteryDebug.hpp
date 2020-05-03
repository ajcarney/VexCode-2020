/**
 * @file: ./RobotCode/src/lcdCode/Debug/BatteryDebug.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/16/2019
 * @reviewed_by: Aiden Carney
 *
 * contains class for debugging the battery
 */

#ifndef __BATTERYDEBUG_HPP__
#define __BATTERYDEBUG_HPP__


#include "../../../../include/main.h"

#include "../Styles.hpp"


/**
 * @see: ../Styles.hpp
 *
 * contains methods that show user data about the battery
 */
class BatteryDebug : private Styles
{
    private:
        lv_obj_t *battery_screen;
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

        BatteryDebug();
        ~BatteryDebug();


        /**
         * @return: None
         *
         * allows user to see information about the state of field control
         */
        void debug();

};




#endif
