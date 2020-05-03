/**
 * @file: ./RobotCode/src/lcdCode/AutonSelecton/PrepScreen.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: PrepScreen.hpp
 *
 * contains class methods seeking confirmation from user
 */

#include <sstream>
#include <string>

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "../../../Autons.hpp"
#include "../../controller/controller.hpp"
#include "PrepScreen.hpp"


bool PrepScreen::nextScreen = false;
bool PrepScreen::confirm = false;



PrepScreen::PrepScreen()
{
    nextScreen = false;
    confirm = false;

    prep_screen = lv_obj_create(NULL, NULL);

//actions_label
    actions_label = lv_label_create(prep_screen, NULL);
    lv_obj_set_style(actions_label, &heading_text);
    lv_obj_set_width(actions_label, 300);
    lv_obj_set_height(actions_label, 160);
    lv_label_set_align(actions_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(actions_label, "actions");



//confirm button
    //button
    btn_confirm = lv_btn_create(prep_screen, NULL);
    lv_btn_set_style(btn_confirm, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_confirm, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_confirm, LV_BTN_ACTION_CLICK, btn_confirm_action);
    lv_obj_set_width(btn_confirm, 300);
    lv_obj_set_height(btn_confirm, 25);

    //label
    btn_confirm_label = lv_label_create(btn_confirm, NULL);
    lv_obj_set_style(btn_confirm_label, &heading_text);
    lv_label_set_text(btn_confirm_label, "Confirm");

//back button
    //button
    btn_back = lv_btn_create(prep_screen, NULL);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, btn_back_action);
    lv_obj_set_width(btn_back, 50);
    lv_obj_set_height(btn_back, 25);

    //label
    btn_back_label = lv_label_create(btn_back, NULL);
    lv_obj_set_style(btn_back_label, &heading_text);
    lv_label_set_text(btn_back_label, "Back");


//title label
    title_label = lv_label_create(prep_screen, NULL);
    lv_obj_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, 300);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(title_label, "Auton");


    lv_obj_set_pos(btn_back, 40, 200);
    lv_obj_set_pos(btn_confirm, 100, 200);
    lv_obj_set_pos(title_label, 210, 20);
    lv_obj_set_pos(actions_label, 40, 50);
}




PrepScreen::~PrepScreen()
{
    lv_obj_del(btn_back_label);
    lv_obj_del(btn_confirm_label);
    lv_obj_del(title_label);

    lv_obj_del(btn_back);
    lv_obj_del(btn_confirm);

    lv_obj_del(prep_screen);
}




/**
 * sets nextScreen so that main loop will break and go to next stage
 */
lv_res_t PrepScreen::btn_confirm_action(lv_obj_t *btn)
{
    nextScreen = true;
    confirm = true;

    return LV_RES_OK;
}

/**
 * sets nextScreen so that main loop will break and go to the previous stage
 */
lv_res_t PrepScreen::btn_back_action(lv_obj_t *btn)
{
    nextScreen = true;
    confirm = false;

    return LV_RES_OK;
}



/**
 * runs loop where user can see what operations will be performed
 * loop breaks when user clicks the back or continue button
 * if user click the back button then the back flag is set
 */
void PrepScreen::getConfirmation( int auton )
{
    Controller controllers;
    Autons auton_data;
    
    lv_label_set_text(title_label, auton_data.AUTONOMOUS_NAMES.at(auton));

    //load screen
    lv_scr_load(prep_screen);

    //set color of border
    std::string color = auton_data.AUTONOMOUS_COLORS.at(auton);
    if (color == "blue")
    {
        gray.body.border.color = BLUE_BORDER;
    }
    else if (color == "red")
    {
        gray.body.border.color = RED_BORDER;
    }
    else
    {
        gray.body.border.color = BG;
    }

    lv_obj_set_style(prep_screen, &gray);

    nextScreen = false;


    std::string label =
        "Initialize and Calibrate Gyro\n"
        "Initialize Other Sensors\n"
        "Initialize Motors\n"
        "Zero Motor Encoders\n"
        "Initialize Controllers\n";
    if ( OptionsScreen::cnfg.record )
    {
        label = label + "Start Recording Thread";
    }

    //cast std::string to const char* and set text
    std::ostringstream text;
    text << label;
    lv_label_set_text(actions_label, text.str().c_str());

    pros::delay( 100 );  //add delay so that button press from previous stage does not register

    while ( !(nextScreen) )
    {
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_A) )
        {
            btn_confirm_action( NULL );
            pros::delay(200);
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_B) )
        {
            btn_back_action( NULL );
            pros::delay(200);
        }
        pros::delay(20);
    }

}
