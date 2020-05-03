/**
 * @file: ./RobotCode/src/lcdCode/Debug/ControllerDebug.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/16/2019
 * @reviewed_by: Aiden Carney
 * TODO:
 *
 * contains classes to show data about controller on gui
 */

#ifndef __CONTROLLERDEBUG_HPP__
#define __CONTROLLERDEBUG_HPP__

#include <unordered_map>

#include "../../../../include/main.h"

#include "../Styles.hpp"

//user defines

//sets size of container
#define CONTROLLER_CONTAINER_WIDTH 440
#define CONTROLLER_CONTAINER_HEIGHT 120

//Base classes
//base classes are the tabs that will be loaded by the derived class
//this makes it easy to add new tabs while keeping the amount that has
//to go in one class to a minimum, especially since lvgl is not light



/**
 * @see: ../Styels.hpp
 * @see: ../../controller/controller.hpp
 *
 * contains general information about controllers and also allow
 * to test communication by sending rumbles and test strings
 */
class GeneralControllerDebug : virtual Styles
{
    private:
        lv_obj_t *container;

        lv_obj_t *controller_column;
        lv_obj_t *connected_column;
        lv_obj_t *capacity_column;
        lv_obj_t *level_column;



        lv_obj_t *btn_test_string;
        lv_obj_t *btn_test_string_label;

        /**
         * @param: lv_obj_t* btn -> button that called the function
         * @return: lv_res_t -> return LV_RES_OK because object still exists
         * TODO: sometimes string does not send to the write location, fix
         *
         * sends a simple string to the controller lcd to test where output is
         * when the user clicks a button
         */
        static lv_res_t btn_test_string_action(lv_obj_t *btn);



        lv_obj_t *btn_clear_scr;
        lv_obj_t *btn_clear_scr_label;

        /**
         * @param: lv_obj_t* btn -> button that called the function
         * @return: lv_res_t -> return LV_RES_OK because object still exists
         * TODO: sometimes doesn't actually work
         *
         * clears any output on the lcd controller when user clicks a button
         */
        static lv_res_t btn_clear_scr_action(lv_obj_t *btn);




        lv_obj_t *btn_test_rumble;
        lv_obj_t *btn_test_rumble_label;

        /**
         * @param: lv_obj_t* btn -> button that called the function
         * @return: lv_res_t -> return LV_RES_OK because object still exists
         *
         * tells controller to rumble
         */
        static lv_res_t btn_test_rumble_action(lv_obj_t *btn);



        lv_obj_t *controls_info;
        lv_obj_t *motor2_info;

    protected:
        /**
         * @return: None
         *
         * @see: ../../controller/controller.hpp
         *
         * updates data for the controllers such as connected or not, and battery
         */
        void update_general_info();

    public:
        GeneralControllerDebug();
        virtual ~GeneralControllerDebug();

        /**
         * @param: lv_obj_t* parent -> new parent for the main container
         * @return: None
         * TODO: depracate and fix method of inheritence, current method is not implemented well
         *
         * changes parent of containter
         */
        void GeneralControllerDebugInit(lv_obj_t *parent);

};




/**
 * @see: ../Styels.hpp
 * @see: ../../controller/controller.hpp
 *
 * generic class for showing the functions or values of the controller for each button
 */
class ControllerTab : virtual Styles
{
    private:
        lv_obj_t *container;

        //separated into two columns because LCD is not big enough
        //column one widgets
        lv_obj_t *button_names_one;
        lv_obj_t *button_col_one;

        //column two widgets
        lv_obj_t *button_names_two;
        lv_obj_t *button_col_two;

    public:
        ControllerTab(lv_obj_t *parent);
        virtual ~ControllerTab();


        /**
         * @param: pros::controller_id_e_t controller -> controller that the tab is looking at
         * @param: bool showing_values -> bool for if values should be shown or not
         * @return: None
         * TODO: make controller a member so that controller does not have to be a parameter
         *
         * shows either the values or the function the controller calls on the gui
         */
        void update(pros::controller_id_e_t controller, bool showing_values);
};





//derived class


/**
 * @see: ../Styles.hpp
 *
 * tab for showing data about controllers
 */
class ControllerDebug :
    virtual private Styles,
    private GeneralControllerDebug
{
    private:
        static bool showing_values;

        //screen
        lv_obj_t *controller_debug_screen;

        //title label
        lv_obj_t *title_label;



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



        lv_obj_t *btn_show_values;
        static lv_obj_t *btn_show_values_label;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function switch between showing the functions or values of
         * for the controller
         */
        static lv_res_t btn_show_values_action(lv_obj_t *btn);



        static lv_obj_t *tabview; //tabview object

        //individual tabs
        //content will come from base classes
        lv_obj_t *general_tab;
        lv_obj_t *master_tab;
        lv_obj_t *partner_tab;


    public:
        ControllerDebug();
        ~ControllerDebug();

        static bool cont; //checks whether to keep letting user
                          //cycle through tabs


        /**
        * @return: None
        *
        * allows user to see information about the controller
        */
        void debug();
};




#endif
