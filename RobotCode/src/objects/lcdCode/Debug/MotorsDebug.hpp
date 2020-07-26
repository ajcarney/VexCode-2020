/**
 * @file: ./RobotCode/src/lcdCode/Debug/MotorsDebug.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/16/2019
 * @reviewed_by: Aiden Carney
 *
 * contains class that loads tabs to debug motors
 */

#ifndef __MOTORDEBUG_HPP__
#define __MOTORDEBUG_HPP__

#include <vector>

#include "../../../../include/main.h"

#include "../Styles.hpp"
#include "../../motors/Motors.hpp"
#include "../../motors/Motor.hpp"

//user defines

//sets size of container
#define MOTORS_CONTAINER_WIDTH 440
#define MOTORS_CONTAINER_HEIGHT 100

//sets percent at which to step velocity at
//10 is reasonable because anything higher gives
//less control and anything lower will make it
//difficult to ramp up or down
#define STEP_PERCENT 10



/**
 * @see: ../Styles.hpp
 *
 * general tab for one or two motors max
 * contains methods to show data and set velocity of motors
 * on this tab
 */
class MotorsDebugTab : virtual Styles
{
    private:
        lv_obj_t *container;
        lv_obj_t *motor1_label;
        lv_obj_t *motor2_label;

        lv_obj_t *motor1_info;
        lv_obj_t *motor2_info;

        std::vector<Motor*> motors;
        std::vector<std::string> titles;

    public:
        MotorsDebugTab( std::vector<Motor*> motors_vec, std::vector<std::string> titles_vec, lv_obj_t *parent);
        ~MotorsDebugTab();

        /**
         * @param: int target_velocity -> velocity the motor should be set to
         * @param: lv_obj_t* velocity_label -> label that current veolicty will be written to
         * @return: None
         *
         * @see: ../Styles.hpp
         * @see: ../../motors/Motors.hpp
         *
         * updates text for the motors that the class was instatiated with
         * also sets the velocity of the motor to int target_velocity
         * data shown is current drawn, voltage, reversed or not, temperature, encoder value,
         * and tourque
         */
        void update_label(int target_velocity, lv_obj_t *velocity_label);
};




/**
 * @see: class MotorsDebugTab
 * @see: ../Styles.hpp
 *
 * contatins debugger for motors
 * gives data for each motor set ie. left chassis, right chassis, intake, etc.
 */
class MotorsDebug : virtual Styles
{
    private:
        //screen
        lv_obj_t *motor_debug_screen;

        //title label
        lv_obj_t *title_label;

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


        static lv_obj_t *tabview;   //tabview object

        lv_obj_t *l_chassis_tab;    //individual tabs
        lv_obj_t *r_chassis_tab;    //content will come from base classes
        lv_obj_t *main_intake_tab;
        lv_obj_t *hoarding_intake_tab;
        lv_obj_t *lift_tab;

        static uint16_t tab_loaded;  // 0 = left chassis
                                     // 1 = right chassis
                                     // 2 = lift
                                     // 3 = intake
                                     // 4 = lift

        /**
         * @param: lv_obj_t* tabview -> tabview object for callback function
         * @param: uint16_t -> id of active tab
         * @return: lv_res_t -> return LV_RES_OK since object was not deleted
         *
         * funtion to stop motor movements and set the ability for other threads
         * to limit the speed of the motor ie. set it to zero in driver control
         * also updates target velocity and the tab loaded
         */
        static lv_res_t tab_load_action(lv_obj_t *tabview, uint16_t act_id);


        //velocity setting buttons
        lv_obj_t *velocity_label;

        lv_obj_t *btn_pos_increase;
        lv_obj_t *btn_neg_increase;
        lv_obj_t *btn_stp;

        lv_obj_t *btn_pos_increase_label;
        lv_obj_t *btn_neg_increase_label;
        lv_obj_t *btn_stp_label;


        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * @see: std::tuple<int, int> get_velocity_step()
         *
         * button callback function used to decrease the target velocity
         */
        static lv_res_t btn_pos_increase_action(lv_obj_t *btn);


        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * @see: std::tuple<int, int> get_velocity_step()
         *
         * button callback function used to increase the target velocity
         */
        static lv_res_t btn_neg_increase_action(lv_obj_t *btn);


        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * @see: std::tuple<int, int> get_velocity_step()
         *
         * button callback function used to set target velocity to zero
         */
        static lv_res_t btn_stp_action(lv_obj_t *btn);


        /**
         * @return: std::tuple<int, int> -> tuple of step, a percentage of max velocity
         *          based on STEP_PERCENT, and max velocity of the motor
         * TOOD: update max velocity for motors and make more adaptable to changing motors
         *
         * gets the amount the step should be and the max velocity for the motor
         * the max velocity is higher than actual because the motor can go faster
         * than the specified RPM
         */
        static std::tuple<int, int> get_velocity_step();


        //static vars to help keep velocity
        //need to be static because they will be modified by
        //static function
        static int target_velocity;


        //brake mode option widgets
        lv_obj_t *brake_mode_label;
        lv_obj_t *ddlist_brake_mode;


        /**
         * @param: lv_obj_t* ddlist -> the dropdown list object for the callback function
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * sets the brake mode for the motor set which will be updated in the main loop
         */
        static lv_res_t ddlist_brake_mode_action(lv_obj_t *ddlist);

        static pros::motor_brake_mode_e_t current_brake_mode;

    public:
        MotorsDebug();
        ~MotorsDebug();

        static bool cont; //checks whether to keep letting user
                          //cycle through tabs

        /**
         * @return: None
         *
         * allows user to interact with tabs for each motor set that display
         * data about those motors
         */
        void debug();

};


#endif
