#include <string>
#include <vector> 

#include "main.h"

#include "../../../motors/Motor.hpp"
#include "../../Styles.hpp"
#include "DigitalInTab.hpp"


std::vector<pros::ADIDigitalIn*> DigitalInDebugger::sensors;
std::vector<std::string> DigitalInDebugger::names;


DigitalInDebugger::DigitalInDebugger(lv_obj_t *parent, int x_dim, int y_dim, std::vector<pros::ADIDigitalIn*> sensors_vec, std::vector<std::string> names_vec)
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
    lv_obj_set_width(info2, (x_dim/3));
    lv_obj_set_height(info2, y_dim - 20);
    lv_label_set_align(info2, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(info2, "None");


//set positions relative to container
    lv_obj_align(title1, container, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_align(info1, container, LV_ALIGN_IN_TOP_LEFT, 10, 30);

    lv_obj_align(title2, container, LV_ALIGN_IN_TOP_RIGHT, -100, 10);
    lv_obj_align(info2, container, LV_ALIGN_IN_TOP_RIGHT, -100, 30);

}

DigitalInDebugger::~DigitalInDebugger()
{

}


/**
 * shows value of limit switch as either 0 or 1
 */
void DigitalInDebugger::update_info()
{    
    std::string names_text;
    std::string val_text;
    for(int i=0; i < sensors.size(); i++)
    {
        names_text += names.at(i) + "\n";
        val_text += std::to_string(sensors.at(i)->get_value()) + "\n";
    }

    lv_label_set_text(title1, "Limit Switch");
    lv_label_set_text(title2, "State");
    lv_label_set_text(info1, names_text.c_str());
    lv_label_set_text(info2, val_text.c_str());
}
