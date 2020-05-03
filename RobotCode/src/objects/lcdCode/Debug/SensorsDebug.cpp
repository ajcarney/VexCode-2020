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

//base classes
bool VisionSensorDebugger::cont = true;
bool SensorsDebug::all_cont = true;
lv_obj_t *SensorsDebug::tabview;


IMEsDebugger::IMEsDebugger()
{
//init container
    container = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(container, SENSORS_CONTAINER_HEIGHT);

//default text
    std::string text = (
        "front right   -\n"
        "back right    -\n"
        "front left    -\n"
        "back left     -\n"
        "right lift    -\n"
        "left lift     -\n"
        "intake        -\n"
        "lift          -  "
    );

//init integrated motor encoders label label
    info = lv_label_create(container, NULL);
    lv_obj_set_style(info, &toggle_tabbtn_pressed);
    lv_obj_set_width(info, (SENSORS_CONTAINER_WIDTH));
    lv_obj_set_height(info, SENSORS_CONTAINER_HEIGHT);
    lv_label_set_align(info, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info, text.c_str());


//init tare encoders button
    //button
    btn_tare = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_tare, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_tare, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_tare, LV_BTN_ACTION_CLICK, btn_tare_action);
    lv_obj_set_width(btn_tare, 110);
    lv_obj_set_height(btn_tare, 25);

    //label
    btn_tare_label = lv_label_create(btn_tare, NULL);
    lv_obj_set_style(btn_tare_label, &subheading_text);
    lv_label_set_text(btn_tare_label, "tare encoders");


//align objects on container
    lv_obj_set_pos(info, 10, 0);
    lv_obj_set_pos(btn_tare, 300, (SENSORS_CONTAINER_HEIGHT - 30));
}

IMEsDebugger::~IMEsDebugger()
{

}


/**
 * tares encodes of all motors
 */
lv_res_t IMEsDebugger::btn_tare_action(lv_obj_t *btn)
{
    Motors *motors = Motors::get_instance();
    
    motors->frontRight->tare_position();
    motors->backRight->tare_position();
    motors->frontLeft->tare_position();
    motors->backLeft->tare_position();
    motors->right_intake->tare_position();
    motors->left_intake->tare_position();
    motors->tilter->tare_position();
    motors->lift->tare_position();
}


/**
 * changes parent of objects
 */
void IMEsDebugger::IMEsDebuggerInit(lv_obj_t *parent)
{
    //sets parent of container to pointer of new parent
    //this is to allow seperation of tabs into seperate classes
    //reduce the quantity in one class and to allow for ease of adding
    //new or different tabs

    lv_obj_set_parent(container, parent);
}


/**
 * updates for each motor to current values
 */
void IMEsDebugger::update_imes_info()
{
    Motors *motors = Motors::get_instance();
    
    std::string text = (
        "front right    -  " + std::to_string(motors->frontRight->get_position()) + "\n"
        "back right     -  " + std::to_string(motors->backRight->get_position()) + "\n"
        "front left     -  " + std::to_string(motors->frontLeft->get_position()) + "\n"
        "back left      -  " + std::to_string(motors->backLeft->get_position()) + "\n"
        "right intake   -  " + std::to_string(motors->right_intake->get_position()) + "\n"
        "left intake    -  " + std::to_string(motors->left_intake->get_position()) + "\n"
        "tilter         -  " + std::to_string(motors->tilter->get_position()) + "\n"
        "lift           -  " + std::to_string(motors->lift->get_position()) + "\n"
    );

    lv_label_set_text(info, text.c_str());
}





AccelerometerDebugger::AccelerometerDebugger()
{
//init container
    container = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(container, SENSORS_CONTAINER_HEIGHT);

//title for columns
    //1
    title1 = lv_label_create(container, NULL);
    lv_obj_set_style(title1, &toggle_tabbtn_pressed);
    lv_obj_set_width(title1, (SENSORS_CONTAINER_WIDTH));
    lv_obj_set_height(title1, 20);
    lv_label_set_align(title1, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title1, "None");

    //2
    title2 = lv_label_create(container, NULL);
    lv_obj_set_style(title2, &toggle_tabbtn_pressed);
    lv_obj_set_width(title2, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(title2, 20);
    lv_label_set_align(title2, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title2, "None");

    //3
    title3 = lv_label_create(container, NULL);
    lv_obj_set_style(title3, &toggle_tabbtn_pressed);
    lv_obj_set_width(title3, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(title3, 20);
    lv_label_set_align(title3, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title3, "None");

//info for columns
    //1
    info1 = lv_label_create(container, NULL);
    lv_obj_set_style(info1, &toggle_tabbtn_pressed);
    lv_obj_set_width(info1, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(info1, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info1, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info1, "None");

    //2
    info2 = lv_label_create(container, NULL);
    lv_obj_set_style(info2, &toggle_tabbtn_pressed);
    lv_obj_set_width(info2, (SENSORS_CONTAINER_WIDTH / 3));
    lv_obj_set_height(info2, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info2, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info2, "None");

    //3
    info3 = lv_label_create(container, NULL);
    lv_obj_set_style(info3, &toggle_tabbtn_pressed);
    lv_obj_set_width(info3, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(info3, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info3, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info3, "None");

//calibrate button
    //button
    btn_calibrate = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_calibrate, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_calibrate, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_calibrate, LV_BTN_ACTION_CLICK, btn_calibrate_action);
    lv_obj_set_width(btn_calibrate, 110);
    lv_obj_set_height(btn_calibrate, 25);

    //label
    btn_calibrate_label = lv_label_create(btn_calibrate, NULL);
    lv_obj_set_style(btn_calibrate_label, &subheading_text);
    lv_label_set_text(btn_calibrate_label, "Calibrate");

//set positions relative to container
    lv_obj_align(title1, container, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_align(info1, container, LV_ALIGN_IN_TOP_LEFT, 10, 30);

    lv_obj_align(title2, container, LV_ALIGN_IN_TOP_MID, -15, 10);
    lv_obj_align(info2, container, LV_ALIGN_IN_TOP_MID, -15, 30);

    lv_obj_align(title3, container, LV_ALIGN_IN_TOP_RIGHT, -100, 10);
    lv_obj_align(info3, container, LV_ALIGN_IN_TOP_RIGHT, -100, 30);

    lv_obj_align(btn_calibrate, container, LV_ALIGN_IN_BOTTOM_RIGHT, -50, 0);
}


AccelerometerDebugger::~AccelerometerDebugger()
{

}


/**
 * calibrates accelerometer and adds a loading bar so the gui doesn't appear to
 * hang for no reason
 */
lv_res_t AccelerometerDebugger::btn_calibrate_action(lv_obj_t *btn)
{
    Loading load;
    load.show_load(1800, lv_scr_act(), 190, 240); //shows loading bar while calibrating
    Sensors::calibrateAccel();
    load.hide_load();

    return LV_RES_OK;
}


/**
 * changes parent of objects
 */
void AccelerometerDebugger::AccelerometerDebuggerInit(lv_obj_t *parent)
{
    //sets parent of container to pointer of new parent
    //this is to allow seperation of tabs into seperate classes
    //reduce the quantity in one class and to allow for ease of adding
    //new or different tabs

    lv_obj_set_parent(container, parent);
}


/**
 * updates data for each axis, shows raw data and corrected
 */
void AccelerometerDebugger::update_accelerometer_info()
{
    std::string names_title = "Accelerometer Axis";
    std::string raw_title = "Raw Input";
    std::string corrected_title = "Corrected Input";

    std::string names = "X Axis\nY Axis\nZ Axis";

    accelValues raw_vals = getRawAccelerometer();
    accelValues corrected_vals = getCorrectedAccelerometer();

    std::string raw = (
        std::to_string(raw_vals.Xaxis) + "\n" +
        std::to_string(raw_vals.Yaxis) + "\n" +
        std::to_string(raw_vals.Zaxis) + "\n"
    );

    std::string corrected = (
        std::to_string(corrected_vals.Xaxis) + "\n" +
        std::to_string(corrected_vals.Yaxis) + "\n" +
        std::to_string(corrected_vals.Zaxis) + "\n"
    );

    lv_label_set_text(title1, names_title.c_str());
    lv_label_set_text(title2, raw_title.c_str());
    lv_label_set_text(title3, corrected_title.c_str());
    lv_label_set_text(info1, names.c_str());
    lv_label_set_text(info2, raw.c_str());
    lv_label_set_text(info3, corrected.c_str());


}





GyrosDebugger::GyrosDebugger()
{
//init container
    container = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(container, SENSORS_CONTAINER_HEIGHT);

//title for columns
    //1
    title1 = lv_label_create(container, NULL);
    lv_obj_set_style(title1, &toggle_tabbtn_pressed);
    lv_obj_set_width(title1, (SENSORS_CONTAINER_WIDTH));
    lv_obj_set_height(title1, 20);
    lv_label_set_align(title1, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title1, "None");

    //2
    title2 = lv_label_create(container, NULL);
    lv_obj_set_style(title2, &toggle_tabbtn_pressed);
    lv_obj_set_width(title2, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(title2, 20);
    lv_label_set_align(title2, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title2, "None");

    //3
    title3 = lv_label_create(container, NULL);
    lv_obj_set_style(title3, &toggle_tabbtn_pressed);
    lv_obj_set_width(title3, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(title3, 20);
    lv_label_set_align(title3, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title3, "None");

//info for columns
    //1
    info1 = lv_label_create(container, NULL);
    lv_obj_set_style(info1, &toggle_tabbtn_pressed);
    lv_obj_set_width(info1, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(info1, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info1, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info1, "None");

    //2
    info2 = lv_label_create(container, NULL);
    lv_obj_set_style(info2, &toggle_tabbtn_pressed);
    lv_obj_set_width(info2, (SENSORS_CONTAINER_WIDTH / 3));
    lv_obj_set_height(info2, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info2, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info2, "None");

    //3
    info3 = lv_label_create(container, NULL);
    lv_obj_set_style(info3, &toggle_tabbtn_pressed);
    lv_obj_set_width(info3, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(info3, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info3, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info3, "None");

//calibrate button
    //button
    btn_calibrate = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_calibrate, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_calibrate, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_calibrate, LV_BTN_ACTION_CLICK, btn_calibrate_action);
    lv_obj_set_width(btn_calibrate, 110);
    lv_obj_set_height(btn_calibrate, 25);

    //label
    btn_calibrate_label = lv_label_create(btn_calibrate, NULL);
    lv_obj_set_style(btn_calibrate_label, &subheading_text);
    lv_label_set_text(btn_calibrate_label, "Calibrate");

//set positions relative to container
    lv_obj_align(title1, container, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_align(info1, container, LV_ALIGN_IN_TOP_LEFT, 10, 30);

    lv_obj_align(title2, container, LV_ALIGN_IN_TOP_MID, -15, 10);
    lv_obj_align(info2, container, LV_ALIGN_IN_TOP_MID, -15, 30);

    lv_obj_align(title3, container, LV_ALIGN_IN_TOP_RIGHT, -100, 10);
    lv_obj_align(info3, container, LV_ALIGN_IN_TOP_RIGHT, -100, 30);

    lv_obj_align(btn_calibrate, container, LV_ALIGN_IN_BOTTOM_RIGHT, -50, 0);
}

GyrosDebugger::~GyrosDebugger()
{

}


/**
 * calibrates gyro(s) and adds loading bar so gui doesn't appear to hang for no reason
 */
lv_res_t GyrosDebugger::btn_calibrate_action(lv_obj_t *btn)
{
    Loading load;
    load.show_load(1000, lv_scr_act(), 190, 240); //shows loading bar while calibrating
    Sensors::calibrateGyro();
    load.hide_load();

    return LV_RES_OK;
}


/**
 * changes parent of objects
 */
void GyrosDebugger::GyrosDebuggerInit(lv_obj_t *parent)
{
    //sets parent of container to pointer of new parent
    //this is to allow seperation of tabs into seperate classes
    //reduce the quantity in one class and to allow for ease of adding
    //new or different tabs

    lv_obj_set_parent(container, parent);
}


/**
 * updates data and shows raw and corrected
 */
void GyrosDebugger::update_gyro_info()
{
    std::string names_title = "Gyro";
    std::string raw_title = "Raw Input";
    std::string corrected_title = "Corrected Input";

    std::string names = "Chassis Gyro 1\nChassis Gyro 2";

    gyroValues raw_vals = getRawGyro();
    gyroValues corrected_vals = getCorrectedGyro();

    std::string raw = (
        std::to_string(raw_vals.gyro1) + "\n" +
        std::to_string(raw_vals.gyro2)
    );

    std::string corrected = (
        std::to_string(corrected_vals.gyro1) + "\n" +
        std::to_string(corrected_vals.gyro2)
    );

    lv_label_set_text(title1, names_title.c_str());
    lv_label_set_text(title2, raw_title.c_str());
    lv_label_set_text(title3, corrected_title.c_str());
    lv_label_set_text(info1, names.c_str());
    lv_label_set_text(info2, raw.c_str());
    lv_label_set_text(info3, corrected.c_str());
}






PotentiometerDebugger::PotentiometerDebugger()
{
//init container
    container = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(container, SENSORS_CONTAINER_HEIGHT);

//title for columns
    //1
    title1 = lv_label_create(container, NULL);
    lv_obj_set_style(title1, &toggle_tabbtn_pressed);
    lv_obj_set_width(title1, (SENSORS_CONTAINER_WIDTH));
    lv_obj_set_height(title1, 20);
    lv_label_set_align(title1, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title1, "None");

    //2
    title2 = lv_label_create(container, NULL);
    lv_obj_set_style(title2, &toggle_tabbtn_pressed);
    lv_obj_set_width(title2, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(title2, 20);
    lv_label_set_align(title2, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title2, "None");

    //3
    title3 = lv_label_create(container, NULL);
    lv_obj_set_style(title3, &toggle_tabbtn_pressed);
    lv_obj_set_width(title3, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(title3, 20);
    lv_label_set_align(title3, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title3, "None");

//info for columns
    //1
    info1 = lv_label_create(container, NULL);
    lv_obj_set_style(info1, &toggle_tabbtn_pressed);
    lv_obj_set_width(info1, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(info1, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info1, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info1, "None");

    //2
    info2 = lv_label_create(container, NULL);
    lv_obj_set_style(info2, &toggle_tabbtn_pressed);
    lv_obj_set_width(info2, (SENSORS_CONTAINER_WIDTH / 3));
    lv_obj_set_height(info2, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info2, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info2, "None");

    //3
    info3 = lv_label_create(container, NULL);
    lv_obj_set_style(info3, &toggle_tabbtn_pressed);
    lv_obj_set_width(info3, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(info3, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info3, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info3, "None");

//calibrate button
    //button
    btn_calibrate = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_calibrate, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_calibrate, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_calibrate, LV_BTN_ACTION_CLICK, btn_calibrate_action);
    lv_obj_set_width(btn_calibrate, 110);
    lv_obj_set_height(btn_calibrate, 25);

    //label
    btn_calibrate_label = lv_label_create(btn_calibrate, NULL);
    lv_obj_set_style(btn_calibrate_label, &subheading_text);
    lv_label_set_text(btn_calibrate_label, "Calibrate");

//set positions relative to container
    lv_obj_align(title1, container, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_align(info1, container, LV_ALIGN_IN_TOP_LEFT, 10, 30);

    lv_obj_align(title2, container, LV_ALIGN_IN_TOP_MID, -15, 10);
    lv_obj_align(info2, container, LV_ALIGN_IN_TOP_MID, -15, 30);

    lv_obj_align(title3, container, LV_ALIGN_IN_TOP_RIGHT, -100, 10);
    lv_obj_align(info3, container, LV_ALIGN_IN_TOP_RIGHT, -100, 30);

    lv_obj_align(btn_calibrate, container, LV_ALIGN_IN_BOTTOM_RIGHT, -50, 0);
}

PotentiometerDebugger::~PotentiometerDebugger()
{

}


/**
 * calibrates potentiometer and adds loading bar show gui doesn't appear to hang
 */
lv_res_t PotentiometerDebugger::btn_calibrate_action(lv_obj_t *btn)
{
    Loading load;
    load.show_load(500, lv_scr_act(), 190, 240); //shows loading bar while calibrating
    Sensors::calibratePot();
    load.hide_load();

    return LV_RES_OK;
}


/**
 * changes parent of objects
 */
void PotentiometerDebugger::PotentiometerDebuggerInit(lv_obj_t *parent)
{
    //sets parent of container to pointer of new parent
    //this is to allow seperation of tabs into seperate classes
    //reduce the quantity in one class and to allow for ease of adding
    //new or different tabs

    lv_obj_set_parent(container, parent);
}


/**
 * updates potentiometer data with raw and corrected values
 */
void PotentiometerDebugger::update_pot_info()
{
    std::string names_title = "Potentiometer";
    std::string raw_title = "Raw Input";
    std::string corrected_title = "Corrected Input";

    std::string names = "Lift Potentiometer";

    std::string raw = (
        std::to_string(getRawPot())
    );

    std::string corrected = (
        std::to_string(getCorrectedPot())
    );

    lv_label_set_text(title1, names_title.c_str());
    lv_label_set_text(title2, raw_title.c_str());
    lv_label_set_text(title3, corrected_title.c_str());
    lv_label_set_text(info1, names.c_str());
    lv_label_set_text(info2, raw.c_str());
    lv_label_set_text(info3, corrected.c_str());
}




LimitSwitchDebugger::LimitSwitchDebugger()
{
//init container
    container = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(container, SENSORS_CONTAINER_HEIGHT);

//title for columns
    //1
    title1 = lv_label_create(container, NULL);
    lv_obj_set_style(title1, &toggle_tabbtn_pressed);
    lv_obj_set_width(title1, (SENSORS_CONTAINER_WIDTH));
    lv_obj_set_height(title1, 20);
    lv_label_set_align(title1, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title1, "None");

    //2
    title2 = lv_label_create(container, NULL);
    lv_obj_set_style(title2, &toggle_tabbtn_pressed);
    lv_obj_set_width(title2, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(title2, 20);
    lv_label_set_align(title2, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title2, "None");

//info for columns
    //1
    info1 = lv_label_create(container, NULL);
    lv_obj_set_style(info1, &toggle_tabbtn_pressed);
    lv_obj_set_width(info1, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(info1, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info1, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info1, "None");

    //2
    info2 = lv_label_create(container, NULL);
    lv_obj_set_style(info2, &toggle_tabbtn_pressed);
    lv_obj_set_width(info2, (SENSORS_CONTAINER_WIDTH/3));
    lv_obj_set_height(info2, SENSORS_CONTAINER_HEIGHT - 20);
    lv_label_set_align(info2, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info2, "None");


//set positions relative to container
    lv_obj_align(title1, container, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_align(info1, container, LV_ALIGN_IN_TOP_LEFT, 10, 30);

    lv_obj_align(title2, container, LV_ALIGN_IN_TOP_RIGHT, -100, 10);
    lv_obj_align(info2, container, LV_ALIGN_IN_TOP_RIGHT, -100, 30);

}

LimitSwitchDebugger::~LimitSwitchDebugger()
{

}


/**
 * changes parent of objects
 */
void LimitSwitchDebugger::LimitSwitchDebuggerInit(lv_obj_t *parent)
{
    //sets parent of container to pointer of new parent
    //this is to allow seperation of tabs into seperate classes
    //reduce the quantity in one class and to allow for ease of adding
    //new or different tabs

    lv_obj_set_parent(container, parent);
}


/**
 * shows value of limit switch as either 0 or 1
 */
void LimitSwitchDebugger::update_limit_switch_info()
{
    std::string names_title = "Limit Switch";
    std::string val_title = "State";

    std::string names = "Limit Switch 1";

    std::string val = (
        std::to_string(getLimitSwitch())
    );


    lv_label_set_text(title1, names_title.c_str());
    lv_label_set_text(title2, val_title.c_str());
    lv_label_set_text(info1, names.c_str());
    lv_label_set_text(info2, val.c_str());
}




LEDDebugger::LEDDebugger()
{
//init container
    container = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(container, SENSORS_CONTAINER_HEIGHT);

//set button
    //button
    btn_set = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_set, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_set, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_set, LV_BTN_ACTION_CLICK, btn_set_action);
    lv_obj_set_width(btn_set, 160);
    lv_obj_set_height(btn_set, 40);

    //label
    btn_set_label = lv_label_create(btn_set, NULL);
    lv_obj_set_style(btn_set_label, &subheading_text);
    lv_label_set_text(btn_set_label, "Set");

//clear button
    //button
    btn_clear = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_clear, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_clear, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_clear, LV_BTN_ACTION_CLICK, btn_clear_action);
    lv_obj_set_width(btn_clear, 160);
    lv_obj_set_height(btn_clear, 40);

    //label
    btn_clear_label = lv_label_create(btn_clear, NULL);
    lv_obj_set_style(btn_clear_label, &subheading_text);
    lv_label_set_text(btn_clear_label, "Clear");

//set relative positions
    lv_obj_align(btn_set, container, LV_ALIGN_IN_TOP_LEFT, 40, 20);
    lv_obj_align(btn_clear, container, LV_ALIGN_IN_TOP_RIGHT, -40, 20);


}

LEDDebugger::~LEDDebugger()
{

}


/**
 * changes state of LED to on
 */
lv_res_t LEDDebugger::btn_set_action(lv_obj_t *btn)
{
    led.set_value(true);
    led_status = true;
    return LV_RES_OK;
}


/**
 * changes state of LED to off
 */
lv_res_t LEDDebugger::btn_clear_action(lv_obj_t *btn)
{
    led.set_value(false);
    led_status = false;
    return LV_RES_OK;
}


/**
 * changes parent of objects
 */
void LEDDebugger::LEDDebuggerInit(lv_obj_t *parent)
{
    //sets parent of container to pointer of new parent
    //this is to allow seperation of tabs into seperate classes
    //reduce the quantity in one class and to allow for ease of adding
    //new or different tabs

    lv_obj_set_parent(container, parent);
}





VisionSensorDebugger::VisionSensorDebugger()
{
//init screen
    vision_sensor_screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(vision_sensor_screen, &gray);

//init title label
    title_label = lv_label_create(vision_sensor_screen, NULL);
    lv_label_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, SENSORS_CONTAINER_WIDTH);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Vision Sensor - Debug");

//init back button
    //button
    btn_back = lv_btn_create(vision_sensor_screen, NULL);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, btn_back_action);
    lv_obj_set_width(btn_back, 75);
    lv_obj_set_height(btn_back, 25);

    //label
    btn_back_label = lv_label_create(btn_back, NULL);
    lv_obj_set_style(btn_back_label, &heading_text);
    lv_label_set_text(btn_back_label, "Back");

//set positions
    lv_obj_set_pos(btn_back, 30, 210);

    lv_obj_set_pos(title_label, 180, 5);
}

VisionSensorDebugger::~VisionSensorDebugger()
{
    lv_obj_del(vision_sensor_screen);
}

lv_res_t VisionSensorDebugger::btn_back_action(lv_obj_t *btn)
{
    cont = false;
    return LV_RES_OK;
}


/**
 * loads page for sensor and waits for user to hit the back button for
 * loop to break
 */
void VisionSensorDebugger::load_vision_sensor_page()
{
    cont = true;

    lv_scr_load(vision_sensor_screen);

    while ( cont )
    {
        pros::delay(200);
    }

}




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
    imes_tab = lv_tabview_add_tab(tabview, "Encoders");
    acclr_tab = lv_tabview_add_tab(tabview, "Accel");
    gyros_tab = lv_tabview_add_tab(tabview, "Gyros");
    pot_tab = lv_tabview_add_tab(tabview, "Pot");
    limit_tab = lv_tabview_add_tab(tabview, "Limit\nSwitch");
    led_tab = lv_tabview_add_tab(tabview, "LED");
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
    IMEsDebuggerInit(imes_tab);
    AccelerometerDebuggerInit(acclr_tab);
    GyrosDebuggerInit(gyros_tab);
    PotentiometerDebuggerInit(pot_tab);
    LimitSwitchDebuggerInit(limit_tab);
    LEDDebuggerInit(led_tab);

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
    lv_obj_del(acclr_tab);
    lv_obj_del(gyros_tab);
    lv_obj_del(pot_tab);
    lv_obj_del(limit_tab);
    lv_obj_del(led_tab);
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

    lv_tabview_set_tab_act(tabview, 0, NULL);
    lv_scr_load(sensors_debug_screen);

    while ( all_cont )
    {
        switch ( lv_tabview_get_tab_act(tabview) ) //switches to tab user wants to go to
        {
            case 0:
                update_imes_info();
                break;

            case 1:
                if ( !(accelCalibrated) ) //checks for sensor being
                                          //calibrated. If not warning
                                          //will appear
                {
                    lv_tabview_set_sliding(tabview, false); //dissallows changing
                                                            //tab until user
                                                            //has selected a
                                                            //calibrate option

                    std::string msg = (
                        "Accelerometer has not been calibrated.\n"
                        "Click continue to calibrate, or back to\n"
                        "return to a previous screen\n\n"
                        "(Please keep sensor still while calibrating)\n"
                    );

                    WarningMessage warnmsg;
                    bool calibrated = warnmsg.warn(msg, sensors_debug_screen);

                    lv_tabview_set_sliding(tabview, true); //re-enables switching
                                                           //tabs

                    if ( calibrated )
                    {
                        Loading load;
                        load.show_load(1800, sensors_debug_screen, 190, 125); //shows loading circle while calibrating
                        Sensors::calibrateAccel();
                        load.hide_load();

                        update_accelerometer_info();
                    }

                    else
                    {
                        lv_tabview_set_tab_act(tabview, 0, NULL);
                        //tab_loaded = 0;
                    }
                }

                else //if Accelerometer is already calibrated
                {
                    update_accelerometer_info();
                }

                break;


            case 2:
                if ( !(gyroCalibrated) )  //checks for sensor being
                                          //calibrated. If not warning
                                          //will appear
                {
                    lv_tabview_set_sliding(tabview, false); //dissallows changing
                                                            //tab until user
                                                            //has selected a
                                                            //calibrate option

                    std::string msg = (
                        "Gyros have not been calibrated.\n"
                        "Click continue to calibrate, or back to\n"
                        "return to a previous screen\n\n"
                        "(Please keep sensor still while calibrating)\n"
                    );

                    WarningMessage warnmsg;
                    bool calibrated = warnmsg.warn(msg, sensors_debug_screen);

                    lv_tabview_set_sliding(tabview, true); //re-enables switching
                                                           //tabs

                    if ( calibrated )
                    {
                        Loading load;
                        load.show_load(1000, sensors_debug_screen, 190, 125); //shows loading bar while calibrating
                        Sensors::calibrateGyro();
                        load.hide_load();

                        update_gyro_info();
                    }

                    else
                    {
                        lv_tabview_set_tab_act(tabview, 0, NULL);
                        //tab_loaded = 0;
                    }
                }

                else //if Accelerometer is already calibrated
                {
                    update_gyro_info();
                }

                break;

            case 3:
                if ( !(potCalibrated) )   //checks for sensor being
                                          //calibrated. If not warning
                                          //will appear
                {
                    lv_tabview_set_sliding(tabview, false); //dissallows changing
                                                            //tab until user
                                                            //has selected a
                                                            //calibrate option

                    std::string msg = (
                        "Potentiometer has not been calibrated.\n"
                        "Click continue to calibrate, or back to\n"
                        "return to a previous screen\n\n"
                        "(Please keep sensor still while calibrating)\n"
                    );

                    WarningMessage warnmsg;
                    bool calibrated = warnmsg.warn(msg, sensors_debug_screen);

                    lv_tabview_set_sliding(tabview, true); //re-enables switching
                                                           //tabs

                    if ( calibrated )
                    {
                        Loading load;
                        load.show_load(500, sensors_debug_screen, 190, 125); //shows loading circle while calibrating
                        Sensors::calibratePot();
                        load.hide_load();

                        update_pot_info();
                    }

                    else
                    {
                        lv_tabview_set_tab_act(tabview, 0, NULL);
                        //tab_loaded = 0;
                    }
                }

                else //if Accelerometer is already calibrated
                {
                    update_pot_info();
                }

                break;

            case 4:
                update_limit_switch_info();
                break;
            case 5:
                //update led info
                //no function needed because it is self contained in the led
                //class
                break;
            case 6:
                load_vision_sensor_page();
                lv_scr_load(sensors_debug_screen);

                //switch to a different tab or user will be unable to leave
                //vision sensor debugger
                lv_tabview_set_tab_act(tabview, 0, NULL);
                //tab_loaded = 0;
                break;
        }

        pros::delay(200);
    }
}
