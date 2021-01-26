/**
 * @file: ./RobotCode/src/lcdCode/Debug/SensorsDebug.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: SensorsDebug.hpp
 *
 * contains all methods for tabs that contain ways to debug and check sensors
 */

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "../Styles.hpp"
#include "../Gimmicks.hpp"
#include "../../motors/Motors.hpp"
#include "../../sensors/Sensors.hpp"
#include "SensorsDebug.hpp"
#include "sensor_tabs/AnalogInTab.hpp"
#include "sensor_tabs/DigitalInTab.hpp"
#include "sensor_tabs/EncoderTab.hpp"
#include "sensor_tabs/IMETab.hpp"
#include "sensor_tabs/IMUTab.hpp"
#include "sensor_tabs/VisionSensorTab.hpp"

//base classes
// bool VisionSensorDebugger::cont = true;
bool SensorsDebug::all_cont = true;
lv_obj_t *SensorsDebug::tabview;


SensorsDebug::SensorsDebug()
{
//set default for statics
    all_cont = true;

//init screen
    sensors_debug_screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(sensors_debug_screen, &gray);

//init title label
    title_label = lv_label_create(sensors_debug_screen, NULL);
    lv_label_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Sensors - Debug");

//init tabview
    tabview = lv_tabview_create(sensors_debug_screen, NULL);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BG, &gray);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_REL, &toggle_tabbtn_released);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_PR, &toggle_tabbtn_pressed);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_INDIC, &sw_indic);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, &toggle_tabbtn_pressed);
    //lv_tabview_set_tab_load_action(tabview, tab_load_action);
    lv_obj_set_width(tabview, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(tabview, 200);

//init tabs
    imes_tab = lv_tabview_add_tab(tabview, "IMEs");
    analog_in_tab = lv_tabview_add_tab(tabview, "Analog In");
    digital_in_tab = lv_tabview_add_tab(tabview, "Digital In");
    imu_tab = lv_tabview_add_tab(tabview, "IMU");
    encoders_tab = lv_tabview_add_tab(tabview, "Encoders");
    vision_sensor_tab = lv_tabview_add_tab(tabview, "Vision\nSensor");


//init back button
    //button
    btn_back = lv_btn_create(sensors_debug_screen, NULL);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, btn_back_action);
    lv_obj_set_width(btn_back, 75);
    lv_obj_set_height(btn_back, 25);

    //label
    btn_back_label = lv_label_create(btn_back, NULL);
    lv_obj_set_style(btn_back_label, &heading_text);
    lv_label_set_text(btn_back_label, "Back");

//init tabs from other classes
    //IMEsDebuggerInit(imes_tab);
    //PotentiometerDebuggerInit(pot_tab);
    //LimitSwitchDebuggerInit(limit_tab);

//set positions
    lv_obj_set_pos(btn_back, 30, 210);

    lv_obj_set_pos(title_label, 180, 5);

    lv_obj_set_pos(tabview, 20, 25);
}


SensorsDebug::~SensorsDebug()
{
    //deletes widgets instantiated by class
    lv_obj_del(title_label);

    lv_obj_del(btn_back_label);
    lv_obj_del(btn_back);

    lv_obj_del(imes_tab);
    lv_obj_del(analog_in_tab);
    lv_obj_del(digital_in_tab);
    lv_obj_del(vision_sensor_tab);

    lv_obj_del(tabview);

    lv_obj_del(sensors_debug_screen);
}


/**
 * callback funciton that exits main loop when button is pressed
 */
lv_res_t SensorsDebug::btn_back_action(lv_obj_t *btn)
{
    all_cont = 0;
    return LV_RES_OK;
}



/**
 * switches on tab loaded, this corresponds to a sensor tab
 * if this sensor needs to be calibrated then there is a warning box that
 * lets the user choosed to calibrate the sensor, and will not allow the user
 * to access the tab until the sensor is calibrated
 */
void SensorsDebug::debug()
{
    //used to check if user wants to continue cycling through
    //tabs. Will be set to zero and loop will break if user hits
    //the back button
    all_cont = 1;

    std::vector<Motor*> v1(Motors::motor_array.begin(), Motors::motor_array.end());
    std::vector<std::string> v2(Motors::motor_names_array.begin(), Motors::motor_names_array.end());
    
    IMEsDebugger imes_debug(
        imes_tab, 
        SENSORS_CONTAINER_WIDTH, 
        SENSORS_CONTAINER_HEIGHT, 
        v1,
        v2
    );
    AnalogInDebugger analog_in_debug(analog_in_tab, SENSORS_CONTAINER_WIDTH, SENSORS_CONTAINER_HEIGHT, {&Sensors::line_tracker_top, &Sensors::line_tracker_middle, &Sensors::line_tracker_bottom}, {"Tracker Top", "Tracker Middle", "Tracker Bottom"});
    IMUDebugger imu_debug(imu_tab, SENSORS_CONTAINER_WIDTH, SENSORS_CONTAINER_HEIGHT, &Sensors::imu);
    EncoderDebugger encoder_debug(
        encoders_tab, 
        SENSORS_CONTAINER_WIDTH, 
        SENSORS_CONTAINER_HEIGHT,
        {
            &Sensors::right_encoder,
            &Sensors::left_encoder,
            &Sensors::strafe_encoder
        }, 
        {
            "R Encoder",
            "L Encoder",
            "S Encoder"
        }
    );

    lv_tabview_set_tab_act(tabview, 0, NULL);
    lv_scr_load(sensors_debug_screen);

    while ( all_cont )
    {
        switch ( lv_tabview_get_tab_act(tabview) ) //switches to tab user wants to go to
        {
            case 0:
                imes_debug.update_info();
                break;

            
            case 1:
                analog_in_debug.update_info();
                // if ( !(Sensors::potentiometer.is_calibrated()) )   //checks for sensor being
                //                                                    //calibrated. If not warning
                //                                                    //will appear
                // {
                //     lv_tabview_set_sliding(tabview, false); //dissallows changing
                //                                             //tab until user
                //                                             //has selected a
                //                                             //calibrate option
                // 
                //     std::string msg = (
                //         "Potentiometer has not been calibrated.\n"
                //         "Click continue to calibrate, or back to\n"
                //         "return to a previous screen\n\n"
                //         "(Please keep sensor still while calibrating)\n"
                //     );
                // 
                //     WarningMessage warnmsg;
                //     bool calibrated = warnmsg.warn(msg, sensors_debug_screen);
                // 
                //     lv_tabview_set_sliding(tabview, true); //re-enables switching
                //                                            //tabs
                // 
                //     if ( calibrated )
                //     {
                //         Loading load;
                //         load.show_load(500, sensors_debug_screen, 190, 125); //shows loading circle while calibrating
                //         Sensors::potentiometer.calibrate();
                //         load.hide_load();
                // 
                //         update_pot_info();
                //     }
                // 
                //     else
                //     {
                //         lv_tabview_set_tab_act(tabview, 0, NULL);
                //         //tab_loaded = 0;
                //     }
                // }
                // 
                // else //if Accelerometer is already calibrated
                // {
                //     update_pot_info();
                // }

                break;

            // case 2:
            //     digital_in_debug.update_info();
            //     break;
            // 
            // case 3:
            //     load_vision_sensor_page();
            //     lv_scr_load(sensors_debug_screen);
            // 
            //     //switch to a different tab or user will be unable to leave
            //     //vision sensor debugger
            //     lv_tabview_set_tab_act(tabview, 0, NULL);
            //     //tab_loaded = 0;
            //     break;
            case 2:
                imu_debug.update_info();
                
            case 3:
                encoder_debug.update_info();
            
        }

        pros::delay(200);
    }
}
