/**
 * @file: ./RobotCode/src/lcdCode/Debug/MotorsDebug.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/16/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: MotorsDebug.hpp
 *
 * contains classes and methods implementation that allow the gui to show
 * the user information about groups of motors seperated into tabs
 */

#include <cstdint>
#include <cmath>
#include <vector>

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "../Styles.hpp"
#include "../../motors/Motors.hpp"
#include "MotorsDebug.hpp"


//declare static members of all classes
bool MotorsDebug::cont = true;
int MotorsDebug::target_velocity = 0;
lv_obj_t *MotorsDebug::tabview;
pros::motor_brake_mode_e_t MotorsDebug::current_brake_mode = pros::E_MOTOR_BRAKE_COAST;
uint16_t MotorsDebug::tab_loaded = 0;



MotorsDebugTab::MotorsDebugTab( std::vector<pros::Motor*> motors_vec, std::vector<std::string> titles_vec, lv_obj_t *parent)
{
    for( int i = 0; i < motors_vec.size(); i++ )
    {
        motors.push_back(motors_vec.at(i));
        titles.push_back(titles_vec.at(i));
    }
//init container
    container = lv_cont_create(parent, NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, MOTORS_CONTAINER_WIDTH);
    lv_obj_set_height(container, MOTORS_CONTAINER_HEIGHT);

//init motor 1 label
    motor1_label = lv_label_create(container, NULL);
    lv_obj_set_style(motor1_label, &toggle_tabbtn_pressed);
    lv_obj_set_width(motor1_label, (MOTORS_CONTAINER_WIDTH/2));
    lv_obj_set_height(motor1_label, 20);
    lv_label_set_align(motor1_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(motor1_label, titles.at(0).c_str());

//init motor 1 info label
    motor1_info = lv_label_create(container, NULL);
    lv_obj_set_style(motor1_info, &toggle_tabbtn_pressed);
    lv_obj_set_width(motor1_info, (MOTORS_CONTAINER_WIDTH/2));
    lv_obj_set_height(motor1_info, 20);
    lv_label_set_align(motor1_info, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(motor1_info, "None\nNone\nNone\nNone\nNone\nNone");

//init motor 2 label
    if( motors.size() > 1 )
    {
        motor2_label = lv_label_create(container, NULL);
        lv_obj_set_style(motor2_label, &toggle_tabbtn_pressed);
        lv_obj_set_width(motor2_label, (MOTORS_CONTAINER_WIDTH/2));
        lv_obj_set_height(motor2_label, 20);
        lv_label_set_align(motor2_label, LV_LABEL_ALIGN_CENTER);
        lv_label_set_text(motor2_label, titles.at(1).c_str());

    //init motor 2 info label
        motor2_info = lv_label_create(container, NULL);
        lv_obj_set_style(motor2_info, &toggle_tabbtn_pressed);
        lv_obj_set_width(motor2_info, (MOTORS_CONTAINER_WIDTH/2));
        lv_obj_set_height(motor2_info, 20);
        lv_label_set_align(motor2_info, LV_LABEL_ALIGN_LEFT);
        lv_label_set_text(motor2_info, "None");
    }

//align objects on container
    lv_obj_set_pos(motor1_label, 60, 0);
    lv_obj_set_pos(motor1_info, 10, 15);
    if( motors.size() > 1 )
    {
        lv_obj_set_pos(motor2_label, 315, 0);
        lv_obj_set_pos(motor2_info, 255, 15);
    }
}


/**
 * function to be called in main loop so that data about motors will be updated
 * sets velocity, updates data, and updates velocity label
 */
void MotorsDebugTab::update_label(int target_velocity, lv_obj_t *velocity_label)
{
    std::string info1 = "";
    std::string info2 = "";

//set velocity to move at
    std::int32_t vel = target_velocity;
    motors.at(0)->move_velocity(vel);
    if ( motors.size() > 1 )
    {
        motors.at(1)->move_velocity(vel);
    }

//info for first motor
    info1 += "Current Draw: " + std::to_string(motors.at(0)->get_current_draw()) + "\n";
    info1 += "Voltage (mV): " + std::to_string(motors.at(0)->get_voltage()) + "\n";
    info1 += "State: ";
    info1 += motors.at(0)->is_reversed() ? "reversed\n" : "not reversed\n";
    info1 += "Temperature: " + std::to_string(motors.at(0)->get_temperature()) + "\n";
    info1 += "Encoder Position: " + std::to_string(motors.at(0)->get_position()) + "\n";
    info1 += "Torque (Nm): " + std::to_string(motors.at(0)->get_torque()) + "\n";

//info for second motor if it exists
    if ( motors.size() > 1 )
    {
        info2 += "Current Draw: " + std::to_string(motors.at(1)->get_current_draw()) + "\n";
        info2 += "Voltage (mV): " + std::to_string(motors.at(1)->get_voltage()) + "\n";
        info2 += "State: ";
        info2 += motors.at(1)->is_reversed() ? "reversed\n" : "not reversed\n";
        info2 += "Temperature: " + std::to_string(motors.at(1)->get_temperature()) + "\n";
        info2 += "Encoder Position: " + std::to_string(motors.at(1)->get_position()) + "\n";
        info2 += "Torque (Nm): " + std::to_string(motors.at(1)->get_torque()) + "\n";
    }
//info for velocity label
    std::string velocity;
    velocity += titles.at(0) + ": " + std::to_string(motors.at(0)->get_actual_velocity()) + "\n";
    if ( motors.size() > 1 )
    {
        velocity += titles.at(1) + ": " + std::to_string(motors.at(1)->get_actual_velocity());
    }

//set labels
    //casts info strings to c strings to make them compatible with lvgl
    lv_label_set_text(motor1_info, info1.c_str());
    if ( motors.size() > 1 )
    {
        lv_label_set_text(motor2_info, info2.c_str());
    }
    lv_label_set_text(velocity_label, velocity.c_str());

}

MotorsDebugTab::~MotorsDebugTab()
{

}




MotorsDebug::MotorsDebug()
{
    //set default for statics
    cont = true;
    target_velocity = 0;
    tab_loaded = 0;

//init screen
    motor_debug_screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(motor_debug_screen, &gray);

//init title label
    title_label = lv_label_create(motor_debug_screen, NULL);
    lv_label_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, MOTORS_CONTAINER_WIDTH);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Motors - Debug");

//init tabview
    tabview = lv_tabview_create(motor_debug_screen, NULL);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BG, &gray);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_REL, &toggle_tabbtn_released);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_PR, &toggle_tabbtn_pressed);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_INDIC, &sw_indic);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, &toggle_tabbtn_pressed);
    lv_tabview_set_tab_load_action(tabview, tab_load_action);
    lv_obj_set_width(tabview, MOTORS_CONTAINER_WIDTH);
    lv_obj_set_height(tabview, 200);

//init tabs
    l_chassis_tab = lv_tabview_add_tab(tabview, "Chassis (L)");
    r_chassis_tab = lv_tabview_add_tab(tabview, "Chassis (R)");
    tilter_tab = lv_tabview_add_tab(tabview, "Tilter");
    intake_tab = lv_tabview_add_tab(tabview, "Intake");
    lift_tab = lv_tabview_add_tab(tabview, "Lift");

//init back button
    //button
    btn_back = lv_btn_create(motor_debug_screen, NULL);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, btn_back_action);
    lv_obj_set_width(btn_back, 75);
    lv_obj_set_height(btn_back, 25);

    //label
    btn_back_label = lv_label_create(btn_back, NULL);
    lv_obj_set_style(btn_back_label, &heading_text);
    lv_label_set_text(btn_back_label, "Back");

//init velocity label
    velocity_label = lv_label_create(motor_debug_screen, NULL);
    lv_obj_set_style(velocity_label, &subheading_text);
    lv_obj_set_width(velocity_label, 100);
    lv_obj_set_height(velocity_label, 40);
    lv_label_set_align(velocity_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(velocity_label, "Velocity: ");

//init velocity increase button
    //button
    btn_pos_increase =  lv_btn_create(motor_debug_screen, NULL);
    lv_btn_set_style(btn_pos_increase, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_pos_increase, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_pos_increase, LV_BTN_ACTION_CLICK, btn_pos_increase_action);
    lv_obj_set_width(btn_pos_increase, 40);
    lv_obj_set_height(btn_pos_increase, 25);

    //label
    btn_pos_increase_label = lv_label_create(btn_pos_increase, NULL);
    lv_obj_set_style(btn_pos_increase_label, &heading_text);
    lv_label_set_text(btn_pos_increase_label, SYMBOL_RIGHT);

//init velocity decrease button
    //button
    btn_neg_increase =  lv_btn_create(motor_debug_screen, NULL);
    lv_btn_set_style(btn_neg_increase, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_neg_increase, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_neg_increase, LV_BTN_ACTION_CLICK, btn_neg_increase_action);
    lv_obj_set_width(btn_neg_increase, 40);
    lv_obj_set_height(btn_neg_increase, 25);

    //label
    btn_neg_increase_label = lv_label_create(btn_neg_increase, NULL);
    lv_obj_set_style(btn_neg_increase_label, &heading_text);
    lv_label_set_text(btn_neg_increase_label, SYMBOL_LEFT);

//init zero velocity button
    //button
    btn_stp =  lv_btn_create(motor_debug_screen, NULL);
    lv_btn_set_style(btn_stp, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_stp, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_stp, LV_BTN_ACTION_CLICK, btn_stp_action);
    lv_obj_set_width(btn_stp, 40);
    lv_obj_set_height(btn_stp, 25);

    //label
    btn_stp_label = lv_label_create(btn_stp, NULL);
    lv_obj_set_style(btn_stp_label, &heading_text);
    lv_label_set_text(btn_stp_label, SYMBOL_STOP);

//init brake mode label
    brake_mode_label = lv_label_create(motor_debug_screen, NULL);
    lv_obj_set_style(brake_mode_label, &heading_text);
    lv_obj_set_width(brake_mode_label, 100);
    lv_obj_set_height(brake_mode_label, 20);
    lv_label_set_align(brake_mode_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(brake_mode_label, "Brakemode: ");

//init drop down list
    ddlist_brake_mode = lv_ddlist_create(motor_debug_screen, NULL);
    lv_ddlist_set_options(ddlist_brake_mode, "Coast\n"
                                             "Brake\n"
                                             "PID Hold");
    lv_obj_set_style(ddlist_brake_mode, &subheading_text);
    lv_obj_set_width(ddlist_brake_mode, 125);
    lv_obj_set_height(ddlist_brake_mode, 18);
    lv_ddlist_set_action(ddlist_brake_mode, ddlist_brake_mode_action);

//set positions
    lv_obj_set_pos(btn_back, 30, 210);
    lv_obj_set_pos(btn_pos_increase, 270, 210);
    lv_obj_set_pos(btn_stp, 220, 210);
    lv_obj_set_pos(btn_neg_increase, 170, 210);

    lv_obj_set_pos(velocity_label, 330, 177);

    lv_obj_set_pos(brake_mode_label, 60, 177);
    lv_obj_set_pos(ddlist_brake_mode, 170, 177);

    lv_obj_set_pos(title_label, 180, 5);

    lv_obj_set_pos(tabview, 20, 25);

}


MotorsDebug::~MotorsDebug()
{
    Motors *motors = Motors::get_instance();
    
    //sets motors to off
    motors->frontLeft->move_velocity(0);
    motors->backLeft->move_velocity(0);
    motors->frontRight->move_velocity(0);
    motors->backRight->move_velocity(0);
    motors->right_intake->move_velocity(0);
    motors->left_intake->move_velocity(0);
    motors->tilter->move_velocity(0);
    motors->lift->move_velocity(0);

    //allow motor to go to zero for driver control if it is not set
    //already
    motors->allow_left_chassis = true;
    motors->allow_right_chassis = true;
    motors->allow_intake = true;
    motors->allow_tilter = true;
    motors->allow_lift = true;

    //deletes widgets instantiated by class
    lv_obj_del(title_label);

    lv_obj_del(btn_back_label);
    lv_obj_del(btn_back);

    lv_obj_del(l_chassis_tab);
    lv_obj_del(r_chassis_tab);
    lv_obj_del(tilter_tab);
    lv_obj_del(intake_tab);
    lv_obj_del(lift_tab);
    lv_obj_del(tabview);

    lv_obj_del(velocity_label);

    lv_obj_del(btn_pos_increase_label);
    lv_obj_del(btn_neg_increase_label);
    lv_obj_del(btn_stp_label);
    lv_obj_del(btn_pos_increase);
    lv_obj_del(btn_neg_increase);
    lv_obj_del(btn_stp);

    lv_obj_del(brake_mode_label);
    lv_obj_del(ddlist_brake_mode);


    lv_obj_del(motor_debug_screen);
}


/**
 * set cont to false to break main loop
 */
lv_res_t MotorsDebug::btn_back_action(lv_obj_t *btn)
{
    cont = false;
    return LV_RES_OK;
}


/**
 * callback function for when a new tab is selected
 * used to set motor to default ie. brakemode, velocity
 */
lv_res_t MotorsDebug::tab_load_action(lv_obj_t *tabview, uint16_t act_id)
{
    Motors *motors = Motors::get_instance();
    
    //loads a new tab and sets default values for velocity of motors
    //and if motor is allowed to hit zero velocity

    tab_loaded = act_id;
    target_velocity = 0;

    motors->frontLeft->move_velocity(0);
    motors->backLeft->move_velocity(0);
    motors->frontRight->move_velocity(0);
    motors->backRight->move_velocity(0);

    motors->allow_left_chassis = true;
    motors->allow_right_chassis = true;
    motors->allow_intake = true;
    motors->allow_tilter = true;
    motors->allow_lift = true;

    return LV_RES_OK;

}



/**
 * looks at the current tab loaded to decide on max velocity because the motor
 * can be determined from that
 * gets the step percent by looking at what STEP_PERCENT is defined as
 */
std::tuple<int, int> MotorsDebug::get_velocity_step()
{
    int index = tab_loaded;    // 0 = left chassis - 200RPM
                               // 1 = right chassis - 200RPM
                               // 2 = tilter - 100RPM
                               // 3 = intake - 100RPM
                               // 4 = lift - 100RPM
    int max;

    if ( index == 0 || index == 1 )
    {
        max = 250;
    }

    else if (index == 2 || index == 3 || index == 4)
    {
        max = 130;
    }

    else
    {
        max = 650;
    }

    int step = static_cast<int>(max / STEP_PERCENT);
    return std::make_tuple(step, max);
}


/**
 * increses velocity of motor by calling get_velocity_step but limits it to
 * the max velocity
 */
lv_res_t MotorsDebug::btn_pos_increase_action(lv_obj_t *btn)
{
    //increases velocity by user defined percent
    int step;
    int max;

    std::tie(step, max) = get_velocity_step();
    if ( target_velocity < max )
    {
        target_velocity = target_velocity + step;
    }

    return LV_RES_OK;
}


/**
 * decreases velocity of motor by calling get_velocity_step but limits it to
 * the max velocity in the negative direction
 */
lv_res_t MotorsDebug::btn_neg_increase_action(lv_obj_t *btn)
{
    //decreases velocity by user defined percent
    int step;
    int max;

    std::tie(step, max) = get_velocity_step();
    if ( target_velocity > 0-max )
    {
        target_velocity = target_velocity - step;
    }

    return LV_RES_OK;
}


/**
 * sets velocity of motor to zero, used so that user does not have to click
 * many times to stop the motor
 */
lv_res_t MotorsDebug::btn_stp_action(lv_obj_t *btn)
{
    target_velocity = 0;
    return LV_RES_OK;
}


/**
 * looks at the string of the current drop down list option and compares it to
 * a string to see what brakemode the user wants
 * sets brake mode to this value
 */
lv_res_t MotorsDebug::ddlist_brake_mode_action(lv_obj_t * ddlist)
{
    //checks what the drop down list string is
    char sel_cstr[32];
    lv_ddlist_get_selected_str(ddlist, sel_cstr);

    std::string sel_str = std::string(sel_cstr);  //convert to std::string so
                                                  //that the strings can be
                                                  //compared

    //sets brake mode for motor
    if ( sel_str == "PID Hold" )
    {
        current_brake_mode = pros::E_MOTOR_BRAKE_HOLD;
    }

    else if ( sel_str == "Brake" )
    {
        current_brake_mode = pros::E_MOTOR_BRAKE_BRAKE;
    }

    else
    {
        current_brake_mode = pros::E_MOTOR_BRAKE_COAST;
    }

    return LV_RES_OK; //Return OK if the drop down list is not deleted
}




/**
 * has a main loop that updates internal data as user cycles through tabs
 * to keep data relevent and motors following the function they are supposed to
 * loads tabs for each motor set
 */
void MotorsDebug::debug()
{
    Motors *motors = Motors::get_instance();
    
    //used to check if user wants to continue cycling through
    //tabs. Will be set to zero and loop will break if user hits
    //the back button
    cont = true;

    lv_tabview_set_tab_act(tabview, 0, NULL);
    lv_scr_load(motor_debug_screen);

    MotorsDebugTab l_chassis_tab_debug( {motors->frontLeft, motors->backLeft}, {"Front Left", "Back Left"}, l_chassis_tab );
    MotorsDebugTab r_chassis_tab_debug( {motors->frontRight, motors->backRight}, {"Front Right", "Back Right"}, r_chassis_tab );
    MotorsDebugTab intake_tab_debug( {motors->right_intake, motors->left_intake}, {"Right Intake", "Left Intake"}, intake_tab );
    MotorsDebugTab tilter_tab_debug( {motors->tilter}, {"Tilter"}, intake_tab );
    MotorsDebugTab lift_tab_debug( {motors->lift}, {"Lift"}, lift_tab );

    while ( cont )
    {

        switch ( tab_loaded ) //switches to tab user wants to go to
        {
            case 0:
                l_chassis_tab_debug.update_label(target_velocity, velocity_label);
                motors->allow_left_chassis = motors->frontLeft->get_target_velocity() != 0 ? false : true;
                break;
            case 1:
                r_chassis_tab_debug.update_label(target_velocity, velocity_label);
                motors->allow_right_chassis = motors->frontRight->get_target_velocity() != 0 ? false : true;
                break;
            case 2:
                intake_tab_debug.update_label(target_velocity, velocity_label);
                motors->allow_intake = motors->right_intake->get_target_velocity() != 0 ? false : true;
                break;
            case 3:
                tilter_tab_debug.update_label(target_velocity, velocity_label);
                motors->allow_tilter = motors->tilter->get_target_velocity() != 0 ? false : true;
                break;
            case 4:
                lift_tab_debug.update_label(target_velocity, velocity_label);
                motors->allow_lift = motors->lift->get_target_velocity() != 0 ? false : true;
                break;
        }

        motors->frontLeft->set_brake_mode(current_brake_mode);
        motors->backLeft->set_brake_mode(current_brake_mode);
        motors->frontRight->set_brake_mode(current_brake_mode);
        motors->backRight->set_brake_mode(current_brake_mode);
        motors->right_intake->set_brake_mode(current_brake_mode);
        motors->left_intake->set_brake_mode(current_brake_mode);
        motors->tilter->set_brake_mode(current_brake_mode);
        motors->lift->set_brake_mode(current_brake_mode);

/*        std::cout << current_brake_mode << "\n";
        std::cout << motors->frontLeft.get_brake_mode() << "\n";
        std::cout << motors->backLeft.get_brake_mode() << "\n";
        std::cout << motors->frontRight.get_brake_mode() << "\n";
        std::cout << motors->backRight.get_brake_mode() << "\n";
        std::cout << motors->right_intake.get_brake_mode() << "\n";
        std::cout << motors->left_intake.get_brake_mode() << "\n";
        std::cout << motors->tilter.get_brake_mode() << "\n";
        std::cout << motors->lift.get_brake_mode() << "\n\n\n";*/

        pros::delay(200);
    }

    //reallow motor to hit zero velocity for driver controll
    motors->allow_left_chassis = true;
    motors->allow_right_chassis = true;
    motors->allow_intake = true;
    motors->allow_tilter = true;
    motors->allow_lift = true;
}
