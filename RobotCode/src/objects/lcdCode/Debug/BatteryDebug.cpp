/**
 * @file: ./RobotCode/src/lcdCode/Debug/BatteryDebug.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/16/2019
 * @reviewed_by: Aiden Carney
 *
 * @see BatteryDebug.hpp
 *
 * contains implementation for class for debugging the battery
 */

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "../Styles.hpp"
#include "BatteryDebug.hpp"


bool BatteryDebug::cont = true;

BatteryDebug::BatteryDebug()
{
    cont = true;

//screen
    battery_screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(battery_screen, &gray);

//init back button
    //button
    btn_back = lv_btn_create(battery_screen, NULL);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, btn_back_action);
    lv_obj_set_width(btn_back, 75);
    lv_obj_set_height(btn_back, 25);

    //label
    btn_back_label = lv_label_create(btn_back, NULL);
    lv_obj_set_style(btn_back_label, &heading_text);
    lv_label_set_text(btn_back_label, "Back");

//init title label
    title_label = lv_label_create(battery_screen, NULL);
    lv_label_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, 440);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Battery - Debug");

//init headers label
    labels_label = lv_label_create(battery_screen, NULL);
    lv_label_set_style(labels_label, &subheading_text);
    lv_obj_set_width(labels_label, 220);
    lv_obj_set_height(labels_label, 200);
    lv_label_set_align(labels_label, LV_LABEL_ALIGN_LEFT);

    std::string labels_label_text = (
            "battery percentage\n"
            "current\n"
            "voltage\n"
            "temperature"
    );

    lv_label_set_text(labels_label, labels_label_text.c_str());

//init values label
    info_label = lv_label_create(battery_screen, NULL);
    lv_label_set_style(info_label, &subheading_text);
    lv_obj_set_width(info_label, 220);
    lv_obj_set_height(info_label, 200);
    lv_label_set_align(info_label, LV_LABEL_ALIGN_LEFT);

    std::string info_label_text = (
        "None\n"
        "None\n"
        "None\n"
        "None"
    );

    lv_label_set_text(info_label, info_label_text.c_str());

//set positions
    lv_obj_set_pos(btn_back, 30, 210);

    lv_obj_align(title_label, battery_screen, LV_ALIGN_IN_TOP_MID, 0, 10);

    lv_obj_set_pos(labels_label, 20, 40);
    lv_obj_set_pos(info_label, 360, 40);


}



BatteryDebug::~BatteryDebug()
{
    lv_obj_del(battery_screen);
}



/**
 * sets cont to false to break main loop so main function returns
 */
lv_res_t BatteryDebug::btn_back_action(lv_obj_t *btn)
{
    cont = false;
    return LV_RES_OK;
}



/**
 * main loop that updates the battery information
 */
void BatteryDebug::debug()
{
    cont = true;

    lv_scr_load(battery_screen);

    while ( cont )
    {
        std::string info_label_text = (
            std::to_string(pros::battery::get_capacity()) + "%\n"
            + std::to_string(pros::battery::get_current()) + "\n"
            + std::to_string(pros::battery::get_voltage()) + "\n"
            + std::to_string(pros::battery::get_temperature()) + "\n"
        );

        lv_label_set_text(info_label, info_label_text.c_str());


        pros::delay(100);
    }
}
