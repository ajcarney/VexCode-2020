/**
 * @file: ./RobotCode/src/lcdCode/Debug/SensorsDebug.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: condense, there are several classes that could be combined so that their is not so many
 *
 * contains classes for tabs of the sensors debugger tab
 */

#ifndef __SENSORDEBUG_HPP__
#define __SENSORDEBUG_HPP__

#include <string>

#include "../../../../include/main.h"

#include "../Styles.hpp"
#include "../Gimmicks.hpp"
#include "../../motors/Motors.hpp"
#include "../../sensors/Sensors.hpp"


//user defines

//sets size of container
#define SENSORS_CONTAINER_WIDTH 440
#define SENSORS_CONTAINER_HEIGHT 120


//Base classes
//base classes are the tabs that will be loaded by the derived class
//this makes it easy to add new tabs while keeping the amount that has
//to go in one class to a minimum, especially since lvgl is not light



/**
 * @see: ../Styles.hpp
 *
 * shows tab of IMEs and allows user to tare encoders and see values
 */
class IMEsDebugger :
    virtual Styles
{
    private:
        lv_obj_t *container;
        lv_obj_t *title;
        lv_obj_t *info;

        lv_obj_t *btn_tare;
        lv_obj_t *btn_tare_label;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to tare all IMEs
         */
        static lv_res_t btn_tare_action(lv_obj_t *btn);


    protected:
        /**
         * @return: None
         *
         * updates values for IMEs
         */
        void update_imes_info();

    public:
        IMEsDebugger();
        virtual ~IMEsDebugger();

        /**
         * @param: lv_obj_t* parent -> parent of the tab
         * @return: None
         *
         * objects are initially loaded onto a NULL parent to be updated later
         * this sets it so that the parent of the objects is now the tab
         */
        void IMEsDebuggerInit(lv_obj_t *parent);

};



/**
 * @see: ../Styles.
 *
 * show value for potentiometer
 */
class PotentiometerDebugger :
    virtual Styles
{
    private:
        lv_obj_t *container;

        lv_obj_t *title1;
        lv_obj_t *title2;
        lv_obj_t *title3;

        lv_obj_t *info1;
        lv_obj_t *info2;
        lv_obj_t *info3;

        lv_obj_t *btn_calibrate;
        lv_obj_t *btn_calibrate_label;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to calibrate sensor
         */
        static lv_res_t btn_calibrate_action(lv_obj_t *btn);

    protected:
        /**
         * @return: None
         *
         * updates value of potentiometer
         */
        void update_pot_info();

    public:
        PotentiometerDebugger();
        virtual ~PotentiometerDebugger();

        /**
         * @param: lv_obj_t* parent -> parent of the tab
         * @return: None
         *
         * objects are initially loaded onto a NULL parent to be updated later
         * this sets it so that the parent of the objects is now the tab
         */
        void PotentiometerDebuggerInit(lv_obj_t *parent);
};



/**
 * @see: ../Styles.
 *
 * show value for limit switch
 */
class LimitSwitchDebugger :
    virtual Styles
{
private:
    lv_obj_t *container;

    lv_obj_t *title1;
    lv_obj_t *title2;

    lv_obj_t *info1;
    lv_obj_t *info2;

    protected:
        /**
         * @return: None
         *
         * updates value of limit switch
         */
        void update_limit_switch_info();

    public:
        LimitSwitchDebugger();
        virtual ~LimitSwitchDebugger();

        /**
         * @param: lv_obj_t* parent -> parent of the tab
         * @return: None
         *
         * objects are initially loaded onto a NULL parent to be updated later
         * this sets it so that the parent of the objects is now the tab
         */
        void LimitSwitchDebuggerInit(lv_obj_t *parent);
};



/**
 * @see: ../Styles.
 *
 * starts new page with debugger info for vision sensor because it needs more room
 */
class VisionSensorDebugger : virtual Styles
{
    private:
        lv_obj_t *title_label;

        lv_obj_t *vision_sensor_screen;

        //back button
        lv_obj_t *btn_back;
        lv_obj_t *btn_back_label;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to go back from the new screen loaded by
         * this tab because it is predicted to need more space
         */
        static lv_res_t btn_back_action(lv_obj_t *btn);

        static bool cont;

    protected:
        /**
         * @return: None
         *
         * loads a new page with debug info
         */
        void load_vision_sensor_page();

    public:
        VisionSensorDebugger();
        virtual ~VisionSensorDebugger();
};




//derived class


/**
 * @see: ../Styles.
 *
 * starts tab object with all the sensor tabs that the user
 * can switch between
 */
class SensorsDebug :
    virtual private Styles,
    private IMEsDebugger,
    private PotentiometerDebugger,
    private LimitSwitchDebugger,
    private VisionSensorDebugger
{
    private:
        //screen
        lv_obj_t *sensors_debug_screen;

        //title label
        lv_obj_t *title_label;

        //back button
        lv_obj_t *btn_back;
        lv_obj_t *btn_back_label;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to go back from the debug screen to
         * the title screen
         */
        static lv_res_t btn_back_action(lv_obj_t *btn);


        static lv_obj_t *tabview; //tabview object

        lv_obj_t *imes_tab;    //individual tabs
        lv_obj_t *acclr_tab;   //content will come from base classes
        lv_obj_t *gyros_tab;
        lv_obj_t *pot_tab;
        lv_obj_t *limit_tab;
        lv_obj_t *led_tab;
        lv_obj_t *vision_sensor_tab;


    public:
        SensorsDebug();
        ~SensorsDebug();

        static bool all_cont; //checks whether to allow user to
                              //cycle through tabs or not


        /**
         * @return: None
         *
         * contains methods for transition between tabs with checking sensors
         * for if they are calibrated or not
         * waits for user to go back in a loop while also switching tabs
         *
         */
        void debug();

};




#endif
