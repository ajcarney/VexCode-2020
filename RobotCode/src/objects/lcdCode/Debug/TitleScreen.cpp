/**
 * @file: ./RobotCode/src/lcdCode/Debug/TitleScreen.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: TitleScreen.hpp
 *
 * contains class for selecting a debug screen or going to previous stage
 */

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "TitleScreen.hpp"
#include "../../controller/controller.hpp"


int TitleScreen::option = 0;
const char* TitleScreen::btnm_map[] = {
        "Motors", "Sensors", "Controller", "Battery", 
        "\n", "Field Control", "Wiring", "Internal\nMotor PID", ""
        };

TitleScreen::TitleScreen()
{
    option = 0;

    title_screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(title_screen, &gray);

//init button matrix
    button_matrix = lv_btnm_create(title_screen, NULL);
    lv_btnm_set_map(button_matrix, btnm_map);
    lv_btnm_set_action(button_matrix, button_matrix_action);
    lv_obj_set_width(button_matrix, 440);
    lv_obj_set_height(button_matrix, 140);

    //set styles of button matrix
    lv_btnm_set_style(button_matrix, LV_BTNM_STYLE_BTN_REL, &toggle_btn_released);
    lv_btnm_set_style(button_matrix, LV_BTNM_STYLE_BTN_PR, &toggle_btn_pressed);

//init title label
    title_label = lv_label_create(title_screen, NULL);
    lv_obj_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, 300);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Debugger");

//init back button
    //button
    btn_back = lv_btn_create(title_screen, NULL);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, btn_back_action);
    lv_obj_set_width(btn_back, 75);
    lv_obj_set_height(btn_back, 25);

    //label
    btn_back_label = lv_label_create(btn_back, NULL);
    lv_obj_set_style(btn_back_label, &heading_text);
    lv_label_set_text(btn_back_label, "Back");


//set postitions of widgets
    lv_obj_set_pos(btn_back, 210, 200);
    lv_obj_set_pos(title_label, 210, 20);
    lv_obj_set_pos(button_matrix, 20, 50);

}


TitleScreen::~TitleScreen()
{
    lv_obj_del(btn_back_label);
    lv_obj_del(btn_back);
    lv_obj_del(title_label);
    lv_obj_del(button_matrix);

    lv_obj_del(title_screen);
}



/**
 * compares text of button to text of label to see what button was clicked
 * sets int option to value based on the button clicked
 */
lv_res_t TitleScreen::button_matrix_action(lv_obj_t *btnm, const char *btn_txt)
{
    if (btn_txt == "Motors")
    {
        option = 1;
    }
    else if (btn_txt == "Sensors")
    {
        option = 2;
    }
    else if (btn_txt == "Controller")
    {
        option = 3;
    }
    else if (btn_txt == "Battery")
    {
        option = 4;
    }
    else if (btn_txt == "Field Control")
    {
        option = 5;
    }
    else if (btn_txt == "Wiring")
    {
        option = 6;
    }
    else if (btn_txt == "Internal\nMotor PID")
    {
        option = 7;
    }
    return LV_RES_OK;
}



/**
 * sets option to -1 which is to be interpreted as user wanting to go back
 */
lv_res_t TitleScreen::btn_back_action(lv_obj_t *btn)
{
    option = -1;
    return LV_RES_OK;
}



/**
 * waits for option to be non zero
 * this will happen once any button is clicked
 */
void TitleScreen::chooseOption()
{
    Controller controllers;
    option = 0;

    lv_scr_load(title_screen);
    while ( !option )
    {
        //allow controller to press the buttons as well
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_B) )
        {
            btn_back_action( NULL );
            pros::delay(100);
        }
        pros::delay(20);
    }
}
