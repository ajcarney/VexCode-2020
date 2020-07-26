#include <string>
#include <vector>

#include "main.h"

#include "../../Gimmicks.hpp"
#include "IMUTab.hpp"

pros::Imu *IMUDebugger::imu;

IMUDebugger::IMUDebugger(lv_obj_t *parent, int x_dim, int y_dim, pros::Imu *imu_sensor)
{
    imu = imu_sensor;
    
//init container
    container = lv_cont_create(parent, NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, x_dim);
    lv_obj_set_height(container, y_dim);

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
    lv_obj_align(info1, container, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_align(info2, container, LV_ALIGN_IN_TOP_MID, -15, 10);

    lv_obj_align(btn_calibrate, container, LV_ALIGN_IN_BOTTOM_RIGHT, -50, 0);
}

IMUDebugger::~IMUDebugger()
{

}


/**
 * calibrates potentiometer and adds loading bar show gui doesn't appear to hang
 */
lv_res_t IMUDebugger::btn_calibrate_action(lv_obj_t *btn)
{
    Loading load;
    
    load.show_load(2000, lv_scr_act(), 190, 240); //shows loading bar while calibrating
    if(!(imu->get_status() && 0xFF))
    {
        imu->reset();
        while(imu->is_calibrating())
        {
            pros::delay(25);
        }
        load.hide_load();
    }
    return LV_RES_OK;
}



/**
 * updates potentiometer data with raw and corrected values
 */
void IMUDebugger::update_info()
{
    std::string names_text;
    std::string data_text; 

    names_text += "heading\nrotation\npitch\nroll\nyaw\naccel x\naccel y\naccel z";
    data_text += std::to_string(imu->get_heading()) + "\n";
    data_text += std::to_string(imu->get_rotation()) + "\n";
    data_text += std::to_string(imu->get_pitch()) + "\n";
    data_text += std::to_string(imu->get_roll()) + "\n";
    data_text += std::to_string(imu->get_yaw()) + "\n";
    data_text += std::to_string(imu->get_accel().x) + "\n";
    data_text += std::to_string(imu->get_accel().y) + "\n";
    data_text += std::to_string(imu->get_accel().z) + "\n";
    
    lv_label_set_text(info1, names_text.c_str());
    lv_label_set_text(info2, data_text.c_str());
}
