/**
 * @file: ./RobotCode/src/lcdCode/AutonSelecton/SelectionScreen.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: SelectionScreen.hpp
 *
 * contains methods for class that give user the ability to select an auton number
 */

#include <sstream>
#include <string>

#include "../../../../include/main.h"

#include "../../../Autons.hpp"
#include "SelectionScreen.hpp"
#include "../../controller/controller.hpp"

//init static vars
lv_obj_t *SelectionScreen::selection_screen;
int SelectionScreen::auton_choice = 1;
int SelectionScreen::final_choice = 0;
bool SelectionScreen::update = false;


//constructor
SelectionScreen::SelectionScreen()

{
    auton_choice = 1;
    final_choice = 0;
    update = false;

    selection_screen = lv_obj_create(NULL, NULL);


    //init buttons and labels
    title_label = lv_label_create(selection_screen, NULL);
    description_label = lv_label_create(selection_screen, NULL);
    auton_number_label = lv_label_create(selection_screen, NULL);

    btn_right = lv_btn_create(selection_screen, NULL);
    btn_left = lv_btn_create(selection_screen, NULL);
    btn_select = lv_btn_create(selection_screen, NULL);

    btn_right_label = lv_label_create(btn_right, NULL);
    btn_left_label = lv_label_create(btn_left, NULL);
    btn_select_label = lv_label_create(btn_select, NULL);


    //sets style for widgets
    lv_obj_set_style(selection_screen, &gray);

    lv_btn_set_style(btn_right, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_left, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_select, LV_BTN_STYLE_REL, &toggle_btn_released);

    lv_btn_set_style(btn_right, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_style(btn_left, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_style(btn_select, LV_BTN_STYLE_PR, &toggle_btn_pressed);

    lv_obj_set_style(btn_right_label, &heading_text);
    lv_obj_set_style(btn_left_label, &heading_text);
    lv_obj_set_style(btn_select_label, &heading_text);

    lv_label_set_style(title_label, &heading_text);
    lv_label_set_style(description_label, &heading_text);
    lv_label_set_style(auton_number_label, &heading_text);

    lv_label_set_long_mode(description_label, LV_LABEL_LONG_BREAK);
    lv_label_set_align(auton_number_label, LV_LABEL_ALIGN_CENTER);

    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_align(description_label, LV_LABEL_ALIGN_CENTER);


    //set size of widgets
    lv_obj_set_width(btn_right, 80);
    lv_obj_set_width(btn_left, 80);
    lv_obj_set_width(btn_select, 120);
    lv_obj_set_width(auton_number_label, 40);
    lv_obj_set_width(title_label, 400);
    lv_obj_set_width(description_label, 400);

    lv_obj_set_height(btn_right, 80);
    lv_obj_set_height(btn_left, 80);
    lv_obj_set_height(btn_select, 40);
    lv_obj_set_height(auton_number_label, 40);
    lv_obj_set_height(title_label, 30);
    lv_obj_set_height(description_label, 80);


    //set default text and move widgets to start location
    lv_label_set_text(btn_right_label, SYMBOL_RIGHT);
    lv_label_set_text(btn_left_label, SYMBOL_LEFT);
    lv_label_set_text(btn_select_label, "Select");

    lv_obj_set_pos(btn_right, 390, 150);
    lv_obj_set_pos(btn_left, 10, 150);
    lv_obj_set_pos(btn_select, 180, 180);
    lv_obj_set_pos(auton_number_label, 440, 20);
    lv_obj_set_pos(title_label, 210, 20);
    lv_obj_set_pos(description_label, 40, 60);


    //set action for buttons
    lv_btn_set_action(btn_right, LV_BTN_ACTION_CLICK, btn_right_action);
    lv_btn_set_action(btn_left, LV_BTN_ACTION_CLICK, btn_left_action);
    lv_btn_set_action(btn_select, LV_BTN_ACTION_CLICK, btn_select_action);

}


//destructor
SelectionScreen::~SelectionScreen()
{
    lv_obj_del(auton_number_label);
    lv_obj_del(title_label);
    lv_obj_del(description_label);
    lv_obj_del(btn_right);
    lv_obj_del(btn_left);
    lv_obj_del(btn_select);

    lv_obj_del(selection_screen);
}



//button action functions

/**
 * called when left button is clicked
 * decrements auton_choice and loops it back in range if not in range
 */
lv_res_t SelectionScreen::btn_left_action(lv_obj_t *btn)
{
    Autons auton_data;

    auton_choice -= 1;
    if (auton_choice < 1)
    {
        auton_choice = auton_data.AUTONOMOUS_NAMES.size();
    }

    update = true;

    return LV_RES_OK;
}


/**
 * called when left button is clicked
 * increments auton_choice and loops it back in range if not in range
 */
lv_res_t SelectionScreen::btn_right_action(lv_obj_t *btn)
{
    std::cout << "function called\n";
    Autons auton_data;

    auton_choice += 1;
    if (auton_choice > auton_data.AUTONOMOUS_NAMES.size())
    {
        auton_choice = 1;
    }

    update = true;

    return LV_RES_OK;
}


/**
 * breaks main loop by setting the final auton choice so that gui continues
 */
lv_res_t SelectionScreen::btn_select_action(lv_obj_t *btn)
{
    final_choice = auton_choice;
    update = true;

    return LV_RES_OK;
}




//other functions

/**
 * updates background color by looking at std::unordered_map
 * updates auton number label
 * waits for there to be an update to be implemented by the buttons before exiting
 */
void SelectionScreen::showSelection()
{
    Controller controllers;
    Autons auton_data;

    lv_label_set_text(title_label, auton_data.AUTONOMOUS_NAMES.at(auton_choice));
    lv_label_set_text(description_label, auton_data.AUTONOMOUS_DESCRIPTIONS.at(auton_choice));


    //get color
    std::string color = auton_data.AUTONOMOUS_COLORS.at(auton_choice);
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

    lv_obj_set_style(selection_screen, &gray); //update background

    //cast int of auton choice to string
    std::string str_auton_choice;
    str_auton_choice = std::to_string(auton_choice);
    lv_label_set_text(auton_number_label, str_auton_choice.c_str());


    controllers.master.print( 0, 0, "                           ");
    pros::delay(50);
    controllers.master.print( 0, 0, auton_data.AUTONOMOUS_NAMES.at(auton_choice) );

    while ( !(update) && !(final_choice) ) //waits for screen to change
                                           //so that time is not wasted
    {
        //allow controller to press the buttons as well
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_R1) )
        {
            btn_right_action( NULL );
            pros::delay(200);
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_L1) )
        {
            btn_left_action( NULL );
            pros::delay(200);
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_A) )
        {
            btn_select_action( NULL );
            pros::delay(200);
        }
        pros::delay(100);
    }
    update = false;
}




/**
 * waits in a loop for there to be an update to the gui implemented by
 * button callback functions
 * in the loop, the gui is updated until a final selection is made
 * everytime there is a change ie. when a button is clicked
 */
int SelectionScreen::selectAuton( int auton )
{
    auton_choice = auton;
    final_choice = 0;
    update = false;

    //load screen
    lv_scr_load(selection_screen);

    while ( !(final_choice) ) //waits for user to select an auton
    {                         //before going to next screen
        showSelection(); //showSelection contains delay
    }

    gray.body.border.color = BG; //reset gray style

    return final_choice;

}
