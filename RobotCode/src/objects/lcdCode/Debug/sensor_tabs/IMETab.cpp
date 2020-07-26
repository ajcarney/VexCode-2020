#include <string>
#include <vector> 

#include "main.h"

#include "../../../motors/Motor.hpp"
#include "../../Styles.hpp"
#include "IMETab.hpp"


std::vector<Motor*> IMEsDebugger::motors;
std::vector<std::string> IMEsDebugger::names;

IMEsDebugger::IMEsDebugger(lv_obj_t *parent, int x_dim, int y_dim, std::vector<Motor*> motors_vec, std::vector<std::string> names_vec)
{
    for( int i = 0; i < motors_vec.size(); i++ )
    {
        motors.push_back(motors_vec.at(i));
        names.push_back(names_vec.at(i));
    }
    
//init container
    container = lv_cont_create(parent, NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, x_dim);
    lv_obj_set_height(container, y_dim);

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
    lv_obj_set_width(info, (x_dim));
    lv_obj_set_height(info, y_dim);
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
    lv_obj_set_pos(btn_tare, 300, (y_dim - 30));
}

IMEsDebugger::~IMEsDebugger()
{

}


/**
 * tares encodes of all motors
 */
lv_res_t IMEsDebugger::btn_tare_action(lv_obj_t *btn)
{    
    for( int i = 0; i < motors.size(); i++ )
    {
        motors.at(i)->tare_encoder();
    }
    
    return LV_RES_OK;
}


/**
 * updates for each motor to current values
 */
void IMEsDebugger::update_info()
{
    int max_characters = 0;
    for( int i = 0; i < names.size(); i++ )
    {
        if(names.at(i).length() > max_characters)
        {
            max_characters = names.at(i).length();
        }
    }
    
    std::string text;
    for( int i = 0; i < names.size(); i++ )
    {
        std::string spaces = "";
        for(int j = names.at(i).length(); j < max_characters + 3; j++)
        {
            spaces += " ";
        }
        text += names.at(i) + spaces + "-  " + std::to_string(motors.at(i)->get_encoder_position()) + "\n";
    }

    lv_label_set_text(info, text.c_str());
}
