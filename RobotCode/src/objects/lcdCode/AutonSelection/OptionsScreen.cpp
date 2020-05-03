/**
 * @file: ./RobotCode/src/lcdCode/AutonSelecton/OptionsScreen.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: OptionsScreen.hpp
 *
 * contains class with methods for getting a configuration struct for how
 * auton will be run
 */

#include <unordered_map>

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "../../../Autons.hpp"
#include "../../controller/controller.hpp"
#include "OptionsScreen.hpp"


autonConfig OptionsScreen::cnfg;
bool OptionsScreen::nextScreen = false;
bool OptionsScreen::back = false;



OptionsScreen::OptionsScreen()
{
    nextScreen = false;
    back = false;

    options_screen = lv_obj_create(NULL, NULL);

//use hard coded autonomous
    //switch
    sw_use_hardcoded = lv_sw_create(options_screen, NULL); //switch template
    lv_sw_set_style(sw_use_hardcoded, LV_SW_STYLE_BG, &sw_bg);
    lv_sw_set_style(sw_use_hardcoded, LV_SW_STYLE_INDIC, &sw_indic);
    lv_sw_set_style(sw_use_hardcoded, LV_SW_STYLE_KNOB_ON, &sw_toggled);
    lv_sw_set_style(sw_use_hardcoded, LV_SW_STYLE_KNOB_OFF, &sw_off);

    lv_sw_set_action(sw_use_hardcoded, sw_use_hardcoded_action); //map action
    lv_obj_set_width(sw_use_hardcoded, 40); //witdth
    lv_obj_set_height(sw_use_hardcoded, 20); //height

    //label
    sw_use_hardcoded_label = lv_label_create(options_screen, NULL);
    lv_label_set_style(sw_use_hardcoded_label, &heading_text);
    lv_obj_set_width(sw_use_hardcoded_label, 10);
    lv_obj_set_height(sw_use_hardcoded_label, 20);
    lv_label_set_align(sw_use_hardcoded_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(sw_use_hardcoded_label, "Use Hardcoded Auton");



//gyro turns
    //switch
    sw_gyro_turn = lv_sw_create(options_screen, sw_use_hardcoded);
    lv_sw_set_action(sw_gyro_turn, sw_gyro_turn_action);
    lv_obj_set_width(sw_gyro_turn, 40);
    lv_obj_set_height(sw_gyro_turn, 20);

    //label
    sw_gyro_turn_label = lv_label_create(options_screen, NULL);
    lv_label_set_style(sw_gyro_turn_label, &heading_text);
    lv_obj_set_width(sw_gyro_turn_label, 300);
    lv_obj_set_height(sw_gyro_turn_label, 20);
    lv_label_set_align(sw_gyro_turn_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(sw_gyro_turn_label, "Use Gyro Turns");


//acceleration control
    //switch
    sw_accelleration_ctrl = lv_sw_create(options_screen, sw_use_hardcoded);
    lv_sw_set_action(sw_accelleration_ctrl, sw_accelleration_ctrl_action);
    lv_obj_set_width(sw_accelleration_ctrl, 40);
    lv_obj_set_height(sw_accelleration_ctrl, 20);

    //label
    sw_accelleration_ctrl_label = lv_label_create(options_screen, NULL);
    lv_label_set_style(sw_accelleration_ctrl_label, &heading_text);
    lv_obj_set_width(sw_accelleration_ctrl_label, 300);
    lv_obj_set_height(sw_accelleration_ctrl_label, 20);
    lv_label_set_align(sw_accelleration_ctrl_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(sw_accelleration_ctrl_label, "Use Acceleration Control");


//check motor temp
    //switch
    sw_check_motor_tmp = lv_sw_create(options_screen, sw_use_hardcoded);
    lv_sw_set_action(sw_check_motor_tmp, sw_check_motor_tmp_action);
    lv_obj_set_width(sw_check_motor_tmp, 40);
    lv_obj_set_height(sw_check_motor_tmp, 20);

    //label
    sw_check_motor_tmp_label = lv_label_create(options_screen, NULL);
    lv_label_set_style(sw_check_motor_tmp_label, &heading_text);
    lv_obj_set_width(sw_check_motor_tmp_label, 300);
    lv_obj_set_height(sw_check_motor_tmp_label, 20);
    lv_label_set_align(sw_check_motor_tmp_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(sw_check_motor_tmp_label, "Limit Motor Temp");


//use previous macros
    //switch
    sw_use_previous_macros = lv_sw_create(options_screen, sw_use_hardcoded);
    lv_sw_set_action(sw_use_previous_macros, sw_use_previous_macros_action);
    lv_obj_set_width(sw_use_previous_macros, 40);
    lv_obj_set_height(sw_use_previous_macros, 20);

    //label
    sw_use_previous_macros_label = lv_label_create(options_screen, NULL);
    lv_label_set_style(sw_use_previous_macros_label, &heading_text);
    lv_obj_set_width(sw_use_previous_macros_label, 300);
    lv_obj_set_height(sw_use_previous_macros_label, 20);
    lv_label_set_align(sw_use_previous_macros_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(sw_use_previous_macros_label, "Use Previously Recorded Macros");


//record
    //switch
    sw_record = lv_sw_create(options_screen, sw_use_hardcoded);
    lv_sw_set_action(sw_record, sw_record_action);
    lv_obj_set_width(sw_record, 40);
    lv_obj_set_height(sw_record, 20);

    //label
    sw_record_label = lv_label_create(options_screen, NULL);
    lv_label_set_style(sw_record_label, &heading_text);
    lv_obj_set_width(sw_record_label, 300);
    lv_obj_set_height(sw_record_label, 20);
    lv_label_set_align(sw_record_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(sw_record_label, "Record Motor Movements");


//confirm button
    //button
    btn_confirm = lv_btn_create(options_screen, NULL);
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
    btn_back = lv_btn_create(options_screen, NULL);
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
    title_label = lv_label_create(options_screen, NULL);
    lv_obj_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, 300);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(title_label, "Auton");

//set postition of widgets
    lv_obj_set_pos(sw_use_hardcoded, 400, 40);
    lv_obj_set_pos(sw_gyro_turn, 400, 65);
    lv_obj_set_pos(sw_accelleration_ctrl, 400, 90);
    lv_obj_set_pos(sw_check_motor_tmp, 400, 115);
    lv_obj_set_pos(sw_use_previous_macros, 400, 140);
    lv_obj_set_pos(sw_record, 400, 165);

    lv_obj_set_pos(sw_use_hardcoded_label, 20, 40);
    lv_obj_set_pos(sw_gyro_turn_label, 20, 65);
    lv_obj_set_pos(sw_accelleration_ctrl_label, 20, 90);
    lv_obj_set_pos(sw_check_motor_tmp_label, 20, 115);
    lv_obj_set_pos(sw_use_previous_macros_label, 20, 140);
    lv_obj_set_pos(sw_record_label, 20, 165);

    lv_obj_set_pos(btn_back, 40, 200);
    lv_obj_set_pos(btn_confirm, 100, 200);
    lv_obj_set_pos(title_label, 210, 20);
}


OptionsScreen::~OptionsScreen()
{
    lv_obj_del(sw_use_hardcoded_label);
    lv_obj_del(sw_gyro_turn_label);
    lv_obj_del(sw_accelleration_ctrl_label);
    lv_obj_del(sw_check_motor_tmp_label);
    lv_obj_del(sw_use_previous_macros_label);
    lv_obj_del(sw_record_label);

    lv_obj_del(sw_use_hardcoded);
    lv_obj_del(sw_gyro_turn);
    lv_obj_del(sw_accelleration_ctrl);
    lv_obj_del(sw_check_motor_tmp);
    lv_obj_del(sw_use_previous_macros);
    lv_obj_del(sw_record);

    lv_obj_del(btn_back_label);
    lv_obj_del(btn_confirm_label);
    lv_obj_del(title_label);

    lv_obj_del(btn_back);
    lv_obj_del(btn_confirm);

    lv_obj_del(options_screen);

}



/**
 * sets nextScreen so that main loop will break and go to next stage
 */
lv_res_t OptionsScreen::btn_confirm_action(lv_obj_t *btn)
{
    nextScreen = true;
    back = false;

    return LV_RES_OK;
}

/**
 * sets nextScreen so that main loop will break and go to the previous stage
 */
lv_res_t OptionsScreen::btn_back_action(lv_obj_t *btn)
{
    nextScreen = true;
    back = true;

    return LV_RES_OK;
}




/**
 * sets or clears config option for using hard coded autons based on the
 * switches previous state
 */
lv_res_t OptionsScreen::sw_use_hardcoded_action(lv_obj_t *sw)
{
    cnfg.use_hardcoded = !(cnfg.use_hardcoded);
    return LV_RES_OK;
}

/**
 * sets or clears config option for using gyro turns based on the
 * switches previous state
 */
lv_res_t OptionsScreen::sw_gyro_turn_action(lv_obj_t *sw)
{
    cnfg.gyro_turn = !(cnfg.gyro_turn);
    return LV_RES_OK;
}


/**
 * sets or clears config option for using acceleration control code based on the
 * switches previous state
 */
lv_res_t OptionsScreen::sw_accelleration_ctrl_action(lv_obj_t *sw)
{
    cnfg.accelleration_ctrl = !(cnfg.accelleration_ctrl);
    return LV_RES_OK;
}


/**
 * sets or clears config option for limiting motor output based on the
 * switches previous state
 */
lv_res_t OptionsScreen::sw_check_motor_tmp_action(lv_obj_t *sw)
{
    cnfg.check_motor_tmp = !(cnfg.check_motor_tmp);
    return LV_RES_OK;
}


/**
 * sets or clears config option for allowing use of previously recorded macros based on the
 * switches previous state
 */
lv_res_t OptionsScreen::sw_use_previous_macros_action(lv_obj_t *sw)
{
    cnfg.use_previous_macros = !(cnfg.use_previous_macros);
    return LV_RES_OK;
}


/**
 * sets or clears config option for recorded the match as a macro based on the
 * switches previous state
 */
lv_res_t OptionsScreen::sw_record_action(lv_obj_t *sw)
{
    cnfg.record = !(cnfg.record);
    return LV_RES_OK;
}



/**
 * runs loop where user can set auton configuration options with digital switches
 * loop breaks when user clicks the back or continue button
 * if user click the back button then the back flag is set 
 */
autonConfig OptionsScreen::getOptions( int auton )
{
    Controller controllers;
    Autons auton_data;
    
    lv_sw_off(sw_use_hardcoded); //reset switches and values
    lv_sw_on(sw_gyro_turn);
    lv_sw_on(sw_accelleration_ctrl);
    lv_sw_off(sw_check_motor_tmp);
    lv_sw_on(sw_use_previous_macros);
    lv_sw_off(sw_record);

    cnfg.use_hardcoded = 0;
    cnfg.gyro_turn = 1;
    cnfg.accelleration_ctrl = 1;
    cnfg.check_motor_tmp = 0;
    cnfg.use_previous_macros = 1;
    cnfg.record = 0;


    lv_label_set_text(title_label, auton_data.AUTONOMOUS_NAMES.at(auton));

    //load screen
    lv_scr_load(options_screen);

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

    lv_obj_set_style(options_screen, &gray);


    back = false;
    nextScreen = false;
    
    pros::delay( 100 );  //add delay so that previous button clicks do not register

    while ( !(nextScreen) )
    {
        //allow controller to press the buttons as well
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_A) )
        {
            btn_confirm_action( NULL );
            pros::delay(100);
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_B) )
        {
            btn_back_action( NULL );
            pros::delay(100);
        }
        
        pros::delay(20);
    }

    return cnfg;

}
