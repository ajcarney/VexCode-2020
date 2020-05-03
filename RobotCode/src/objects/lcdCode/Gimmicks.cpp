/**
 * @file: ./RobotCode/src/objects/lcdCode/Gimmicks.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: Gimmicks.hpp
 *
 * contains implementation for header file
 *
 */

#include "../../../include/main.h"
#include "../../../include/api.h"

#include "Styles.hpp"
#include "Gimmicks.hpp"



const char* WarningMessage::buttons[] = {"Back", "Continue", ""};

//base classes
int WarningMessage::option = 0;

WarningMessage::WarningMessage()
{
    option = 0;

    warn_box = lv_mbox_create(lv_scr_act(), NULL);
    lv_mbox_set_text(warn_box, "None");
    lv_mbox_add_btns(warn_box, buttons, NULL);
    lv_mbox_set_action(warn_box, mbox_apply_action);

    lv_mbox_set_style(warn_box, LV_MBOX_STYLE_BG, &warn_box_bg);
    lv_mbox_set_style(warn_box, LV_MBOX_STYLE_BTN_REL, &warn_box_released);
    lv_mbox_set_style(warn_box, LV_MBOX_STYLE_BTN_PR, &warn_box_pressed);

    lv_obj_set_width(warn_box, 400);
    lv_obj_set_height(warn_box, 140);

    lv_obj_align(warn_box, NULL, LV_ALIGN_CENTER, 0, -50);


}

WarningMessage::~WarningMessage()
{
    lv_obj_del(warn_box);
}

/**
 * compares text of message to set option to positive or negative
 */
lv_res_t WarningMessage::mbox_apply_action(lv_obj_t * mbox, const char * txt)
{
    if ( txt == "Continue" )
    {
        option = 1;
    }

    else if ( txt == "Back" )
    {
        option = -1;
    }

    return LV_RES_OK;
}

/**
 * dislays a message box and sets the text
 * user can choose "continue" or "back"
 * how function works line 3
 */
bool WarningMessage::warn( std::string warn_msg, lv_obj_t *parent )
{
    option = 0;

    lv_obj_set_hidden(warn_box, false);
    lv_obj_set_parent(warn_box, parent);
    lv_mbox_set_text(warn_box, warn_msg.c_str());

    while ( !(option) )
    {
        pros::delay(50);
    }

    if ( option == 1 )
    {
        lv_obj_set_hidden(warn_box, true);
        return true;
    }

    else
    {
        lv_obj_set_hidden(warn_box, true);
        return false;
    }

}






Loading::Loading()
{
    loader = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size(loader, 100, 20);
    lv_bar_set_value(loader, 1);
}

Loading::~Loading()
{
    lv_obj_del(loader);
}

/**
 * loader is shown on a parent at specified location
 * animation time is set by user, so this function only works if user knows
 * about how long the function will take
 * this function is meant as a filler so that if some initialization occurs
 * the gui does not appear like its hanging for no reason
 */
void Loading::show_load(int estimated_duration, lv_obj_t *parent, int x, int y)
{
    lv_obj_set_hidden(loader, false);
    lv_bar_set_value(loader, 1);
    lv_obj_set_parent(loader, parent);
    lv_obj_set_top(loader, true);

    lv_obj_set_pos(loader, x, y);

    lv_bar_set_value_anim(loader, 100, estimated_duration);
}

/**
 * hides the loader for when the initialization by user is finished
 */
void Loading::hide_load()
{
    lv_obj_set_hidden(loader, true);
}
