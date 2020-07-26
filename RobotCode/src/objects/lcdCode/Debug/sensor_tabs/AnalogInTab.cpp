#include <string>
#include <vector>

#include "main.h"

#include "../../Gimmicks.hpp"
#include "AnalogInTab.hpp"

std::vector<AnalogInSensor*> AnalogInDebugger::sensors;
std::vector<std::string> AnalogInDebugger::names;


AnalogInDebugger::AnalogInDebugger(lv_obj_t *parent, int x_dim, int y_dim, std::vector<AnalogInSensor*> sensors_vec, std::vector<std::string> names_vec)
{
    for( int i = 0; i < sensors_vec.size(); i++ )
    {
        sensors.push_back(sensors_vec.at(i));
        names.push_back(names_vec.at(i));
    }
//init container
    container = lv_cont_create(parent, NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, x_dim);
    lv_obj_set_height(container, y_dim);

//title for columns
    //1
    title1 = lv_label_create(container, NULL);
    lv_obj_set_style(title1, &toggle_tabbtn_pressed);
    lv_obj_set_width(title1, (x_dim));
    lv_obj_set_height(title1, 20);
    lv_label_set_align(title1, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title1, "None");

    //2
    title2 = lv_label_create(container, NULL);
    lv_obj_set_style(title2, &toggle_tabbtn_pressed);
    lv_obj_set_width(title2, (x_dim/3));
    lv_obj_set_height(title2, 20);
    lv_label_set_align(title2, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title2, "None");

    //3
    title3 = lv_label_create(container, NULL);
    lv_obj_set_style(title3, &toggle_tabbtn_pressed);
    lv_obj_set_width(title3, (x_dim/3));
    lv_obj_set_height(title3, 20);
    lv_label_set_align(title3, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title3, "None");

//info for columns
    //1
    info1 = lv_label_create(container, NULL);
    lv_obj_set_style(info1, &toggle_tabbtn_pressed);
    lv_obj_set_width(info1, (x_dim/3));
    lv_obj_set_height(info1, y_dim - 20);
    lv_label_set_align(info1, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info1, "None");

    //2
    info2 = lv_label_create(container, NULL);
    lv_obj_set_style(info2, &toggle_tabbtn_pressed);
    lv_obj_set_width(info2, (x_dim / 3));
    lv_obj_set_height(info2, y_dim - 20);
    lv_label_set_align(info2, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info2, "None");

    //3
    info3 = lv_label_create(container, NULL);
    lv_obj_set_style(info3, &toggle_tabbtn_pressed);
    lv_obj_set_width(info3, (x_dim/3));
    lv_obj_set_height(info3, y_dim - 20);
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

AnalogInDebugger::~AnalogInDebugger()
{

}


/**
 * calibrates potentiometer and adds loading bar show gui doesn't appear to hang
 */
lv_res_t AnalogInDebugger::btn_calibrate_action(lv_obj_t *btn)
{
    Loading load;
    for(int i=0; i < sensors.size(); i++)
    {
        load.show_load(500, lv_scr_act(), 190, 240); //shows loading bar while calibrating
        sensors.at(i)->calibrate();
        load.hide_load();
    }
    
    return LV_RES_OK;
}



/**
 * updates potentiometer data with raw and corrected values
 */
void AnalogInDebugger::update_info()
{
    std::string names_text = "";
    std::string raw_text = "";
    std::string corrected_text = ""; 
    for(int i=0; i < sensors.size(); i++)
    {
        names_text += names.at(i) + "\n";
        raw_text += std::to_string(sensors.at(i)->get_raw_value()) + "\n";
        corrected_text += std::to_string(sensors.at(i)->get_value(false)) + "\n";
    }

    lv_label_set_text(title1, "Sensor");
    lv_label_set_text(title2, "Raw Input");
    lv_label_set_text(title3, "Corrected Input");
    lv_label_set_text(info1, names_text.c_str());
    lv_label_set_text(info2, raw_text.c_str());
    lv_label_set_text(info3, corrected_text.c_str());
}
