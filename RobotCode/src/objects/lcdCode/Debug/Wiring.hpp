/**
 * @file: ./RobotCode/src/lcdCode/Debug/Wiring.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * contains class that shows the current wiring of the robot
 */

#ifndef __WIRING_HPP__
#define __WIRING_HPP__

#include "../Styles.hpp"

#include "../../motors/Motors.hpp"
#include "../../sensors/Sensors.hpp"


/**
 * @see: ../Styles.hpp
 * @see: ../../motors/Motors.hpp
 * @see: ../../sensors/Sensors.hpp
 *
 * shows the ports that each motor or sensor is located on
 * purpose is to make it easier and more companct to wire the robot than having
 * to read off of separate computer screen
 */
class Wiring : private Styles
{
    private:
        lv_obj_t *wiring_screen;
        lv_obj_t *title_label;

        lv_obj_t *motor_info;
        lv_obj_t *sensors_info;

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

        Wiring();
        ~Wiring();

        /**
         * @return: None
         *
         * passive screen -- loads text and wait for user to go back
         */
        void debug();

};



#endif
