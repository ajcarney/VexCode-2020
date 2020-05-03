/**
 * @file: ./RobotCode/src/lcdCode/Debug/ControllerDebug.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/16/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: ControllerDebug.hpp
 *
 * contains implementation for classes that show information about the controller
 */

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include <unordered_map>

#include "../Styles.hpp"
#include "../../controller/controller.hpp"
#include "ControllerDebug.hpp"


//declare static members of all classes
bool ControllerDebug::showing_values = 0;
bool ControllerDebug::cont = true;
lv_obj_t *ControllerDebug::tabview;
lv_obj_t *ControllerDebug::btn_show_values_label;



GeneralControllerDebug::GeneralControllerDebug()
{
//init container
    container = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, CONTROLLER_CONTAINER_WIDTH);
    lv_obj_set_height(container, CONTROLLER_CONTAINER_HEIGHT);

//default text for each column
    std::string text1 = (
        "Controller\n"
        "Master\n"
        "Partner"
    );

    std::string text2 = (
        "Connected\n"
        "no\n"
        "no"
    );

    std::string text3 = (
        "Battery Capacity\n"
        "0\n"
        "0"
    );

    std::string text4 = (
        "Battery Percent\n"
        "0\n"
        "0"
    );

//init controller column label
    controller_column = lv_label_create(container, NULL);
    lv_obj_set_style(controller_column, &toggle_tabbtn_pressed);
    lv_obj_set_width(controller_column, (CONTROLLER_CONTAINER_WIDTH / 4));
    lv_obj_set_height(controller_column, CONTROLLER_CONTAINER_HEIGHT);
    lv_label_set_align(controller_column, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(controller_column, text1.c_str());

//init connected column label
    connected_column = lv_label_create(container, NULL);
    lv_obj_set_style(connected_column, &toggle_tabbtn_pressed);
    lv_obj_set_width(connected_column, (CONTROLLER_CONTAINER_WIDTH / 4));
    lv_obj_set_height(connected_column, CONTROLLER_CONTAINER_HEIGHT);
    lv_label_set_align(connected_column, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(connected_column, text2.c_str());

//init capacity column label
    capacity_column = lv_label_create(container, NULL);
    lv_obj_set_style(capacity_column, &toggle_tabbtn_pressed);
    lv_obj_set_width(capacity_column, (CONTROLLER_CONTAINER_WIDTH / 4));
    lv_obj_set_height(capacity_column, CONTROLLER_CONTAINER_HEIGHT);
    lv_label_set_align(capacity_column, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(capacity_column, text3.c_str());

//init battery percentage column label
    level_column = lv_label_create(container, NULL);
    lv_obj_set_style(level_column, &toggle_tabbtn_pressed);
    lv_obj_set_width(level_column, (CONTROLLER_CONTAINER_WIDTH / 4));
    lv_obj_set_height(level_column, CONTROLLER_CONTAINER_HEIGHT);
    lv_label_set_align(level_column, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(level_column, text4.c_str());


//init send test string button
    //button
    btn_test_string = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_test_string, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_test_string, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_test_string, LV_BTN_ACTION_CLICK, btn_test_string_action);
    lv_obj_set_width(btn_test_string, 130);
    lv_obj_set_height(btn_test_string, 25);

    //label
    btn_test_string_label = lv_label_create(btn_test_string, NULL);
    lv_obj_set_style(btn_test_string_label, &subheading_text);
    lv_label_set_text(btn_test_string_label, "Send Test String");


//init clear screen button
    //button
    btn_clear_scr = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_clear_scr, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_clear_scr, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_clear_scr, LV_BTN_ACTION_CLICK, btn_clear_scr_action);
    lv_obj_set_width(btn_clear_scr, 130);
    lv_obj_set_height(btn_clear_scr, 25);

    //label
    btn_clear_scr_label = lv_label_create(btn_clear_scr, NULL);
    lv_obj_set_style(btn_clear_scr_label, &subheading_text);
    lv_label_set_text(btn_clear_scr_label, "Clear Screen");


//init send test rumble button
    //button
    btn_test_rumble = lv_btn_create(container, NULL);
    lv_btn_set_style(btn_test_rumble, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_test_rumble, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_test_rumble, LV_BTN_ACTION_CLICK, btn_test_rumble_action);
    lv_obj_set_width(btn_test_rumble, 130);
    lv_obj_set_height(btn_test_rumble, 25);

    //label
    btn_test_rumble_label = lv_label_create(btn_test_rumble, NULL);
    lv_obj_set_style(btn_test_rumble_label, &subheading_text);
    lv_label_set_text(btn_test_rumble_label, "Send Test Rumble");


//set positions relative to container
    lv_obj_align(controller_column, container, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_align(connected_column, container, LV_ALIGN_IN_TOP_MID, -80, 10);
    lv_obj_align(capacity_column, container, LV_ALIGN_IN_TOP_MID, 15, 10);
    lv_obj_align(level_column, container, LV_ALIGN_IN_TOP_RIGHT, -30, 10);


    lv_obj_align(btn_test_string, container, LV_ALIGN_IN_BOTTOM_LEFT, 20, -30);
    lv_obj_align(btn_clear_scr, container, LV_ALIGN_IN_BOTTOM_MID, 0, -30);
    lv_obj_align(btn_test_rumble, container, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -30);

}


GeneralControllerDebug::~GeneralControllerDebug()
{

}



/**
 * sends test string to controller
 */
lv_res_t GeneralControllerDebug::btn_test_string_action(lv_obj_t *btn)
{
    Controller::master.print(0, 0, "This is a test message");
    Controller::partner.print(0, 0, "This is a test message");
    return LV_RES_OK;
}


/**
 * clears the screen on the controller
 */
lv_res_t GeneralControllerDebug::btn_clear_scr_action(lv_obj_t *btn)
{
    Controller::master.print(0, 0, "");
    Controller::partner.print(0, 0, "");
    Controller::master.clear_line(0);
    Controller::partner.clear_line(0);
    return LV_RES_OK;
}


/**
 * sends a test rumble to the controller
 */
lv_res_t GeneralControllerDebug::btn_test_rumble_action(lv_obj_t *btn)
{
    Controller::master.rumble(". - . - ");
    Controller::partner.rumble(". - . - ");
    return LV_RES_OK;
}




/**
 * updates data on the tab
 */
void GeneralControllerDebug::update_general_info()
{
    std::string text1 = (
        "Controller\n"
        "Master\n"
        "Partner"
    );

    std::string master_text = "no";
    std::string partner_text = "no";
    if ( Controller::master.is_connected() )
    {
        master_text = "yes";
    }
    if ( Controller::partner.is_connected() )
    {
        partner_text = "yes";
    }

    std::string text2 = (
        "Connected\n"
        + master_text + "\n"
        + partner_text
    );


    std::string text3 = (
        "Battery Capacity\n"
        + std::to_string(Controller::master.get_battery_level()) + "\n"
        + std::to_string(Controller::partner.get_battery_level())
    );

    std::string text4 = (
        "Battery Percent\n"
        + std::to_string(Controller::master.get_battery_capacity()) + "\n"
        + std::to_string(Controller::partner.get_battery_capacity())
    );

    lv_label_set_text(controller_column, text1.c_str());
    lv_label_set_text(connected_column, text2.c_str());
    lv_label_set_text(capacity_column, text3.c_str());
    lv_label_set_text(level_column, text4.c_str());
}



/**
 * changes parent of all objects
 */
void GeneralControllerDebug::GeneralControllerDebugInit(lv_obj_t *parent)
{
    //sets parent of container to pointer of new parent
    //this is to allow seperation of tabs into seperate classes
    //reduce the quantity in one class and to allow for ease of adding
    //new or different tabs

    lv_obj_set_parent(container, parent);

}




ControllerTab::ControllerTab(lv_obj_t *parent)
{
//init container
    container = lv_cont_create(parent, NULL);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_style(container, &gray);
    lv_cont_set_fit(container, false, false);
    lv_obj_set_width(container, CONTROLLER_CONTAINER_WIDTH);
    lv_obj_set_height(container, CONTROLLER_CONTAINER_HEIGHT);

//text for names
    std::string ctrl_col1 = (
        "Analog Left X\n"
        "Analog Left Y\n"
        "Analog Right X\n"
        "Analog Right Y\n"
        "Digital L1\n"
        "Digital L2\n"
        "Digital R1\n"
        "Digital R2"
    );

    std::string ctrl_col2 = (
        "Digital Up\n"
        "Digital Down\n"
        "Digital Left\n"
        "Digital Right\n"
        "Digital X\n"
        "Digital B\n"
        "Digital Y\n"
        "Digital A\n"
    );

//column one button names
    button_names_one = lv_label_create(container, NULL);
    lv_label_set_style(button_names_one, &toggle_tabbtn_pressed);
    lv_obj_set_width(button_names_one, CONTROLLER_CONTAINER_WIDTH / 4);
    lv_obj_set_height(button_names_one, 20);
    lv_label_set_align(button_names_one, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(button_names_one, ctrl_col1.c_str());


//column two button names
    button_names_two = lv_label_create(container, NULL);
    lv_label_set_style(button_names_two, &toggle_tabbtn_pressed);
    lv_obj_set_width(button_names_two, CONTROLLER_CONTAINER_WIDTH / 4);
    lv_obj_set_height(button_names_two, 20);
    lv_label_set_align(button_names_two, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(button_names_two, ctrl_col2.c_str());


//column one second part that contains function or values
    button_col_one = lv_label_create(container, NULL);
    lv_label_set_style(button_col_one, &toggle_tabbtn_pressed);
    lv_obj_set_width(button_col_one, CONTROLLER_CONTAINER_WIDTH / 4);
    lv_obj_set_height(button_col_one, 20);
    lv_label_set_align(button_col_one, LV_LABEL_ALIGN_LEFT);


//column two second part that contains function or values
    button_col_two = lv_label_create(container, NULL);
    lv_label_set_style(button_col_two, &toggle_tabbtn_pressed);
    lv_obj_set_width(button_col_two, CONTROLLER_CONTAINER_WIDTH / 4);
    lv_obj_set_height(button_col_two, 20);
    lv_label_set_align(button_col_two, LV_LABEL_ALIGN_LEFT);


//set positions relative to container
    lv_obj_align(button_names_one, container, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_align(button_col_one, container, LV_ALIGN_IN_TOP_MID, -80, 10);

    lv_obj_align(button_names_two, container, LV_ALIGN_IN_TOP_MID, 60, 10);
    lv_obj_align(button_col_two, container, LV_ALIGN_IN_TOP_RIGHT, -70, 10);

}


ControllerTab::~ControllerTab()
{
    lv_obj_del(button_names_one);
    lv_obj_del(button_names_two);
    lv_obj_del(button_col_one);
    lv_obj_del(button_col_two);

    lv_obj_del(container);
}



/**
 * updates the data for the controller tab for either the value of each button
 * or the function each button performs by looking at an std::unordered_map contained in
 * controller.hpp
 */
void ControllerTab::update(pros::controller_id_e_t controller, bool showing_values)
{
    std::string functions_col1 = "";
    std::string functions_col2 = "";

    std::string values_col1 = "";
    std::string values_col2 = "";

    if ( controller == pros::E_CONTROLLER_MASTER )
    {
        //text for functions
        functions_col1 = (
            Controller::MASTER_CONTROLLER_ANALOG_MAPPINGS.at(pros::E_CONTROLLER_ANALOG_LEFT_X) + "\n"
            + Controller::MASTER_CONTROLLER_ANALOG_MAPPINGS.at(pros::E_CONTROLLER_ANALOG_LEFT_Y) + "\n"
            + Controller::MASTER_CONTROLLER_ANALOG_MAPPINGS.at(pros::E_CONTROLLER_ANALOG_RIGHT_X) + "\n"
            + Controller::MASTER_CONTROLLER_ANALOG_MAPPINGS.at(pros::E_CONTROLLER_ANALOG_RIGHT_Y) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_L1) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_L2) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_R2) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_R1)
        );

        functions_col2 = (
            Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_UP) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_DOWN) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_LEFT) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_RIGHT) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_X) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_B) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_Y) + "\n"
            + Controller::MASTER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_A)
        );

        values_col1 = (
            std::to_string(Controller::master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)) + "\n"
            + std::to_string(Controller::master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) + "\n"
            + std::to_string(Controller::master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X)) + "\n"
            + std::to_string(Controller::master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
        );

        values_col2 = (
            std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_UP)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_X)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_B)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_Y)) + "\n"
            + std::to_string(Controller::master.get_digital(pros::E_CONTROLLER_DIGITAL_A))
        );


    }

    else if ( controller == pros::E_CONTROLLER_PARTNER )
    {
        //text for functions
        functions_col1 = (
            Controller::PARTNER_CONTROLLER_ANALOG_MAPPINGS.at(pros::E_CONTROLLER_ANALOG_LEFT_X) + "\n"
            + Controller::PARTNER_CONTROLLER_ANALOG_MAPPINGS.at(pros::E_CONTROLLER_ANALOG_LEFT_Y) + "\n"
            + Controller::PARTNER_CONTROLLER_ANALOG_MAPPINGS.at(pros::E_CONTROLLER_ANALOG_RIGHT_X) + "\n"
            + Controller::PARTNER_CONTROLLER_ANALOG_MAPPINGS.at(pros::E_CONTROLLER_ANALOG_RIGHT_Y) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_L1) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_L2) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_R2) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_R1)
        );

        functions_col2 = (
            Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_UP) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_DOWN) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_LEFT) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_RIGHT) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_X) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_B) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_Y) + "\n"
            + Controller::PARTNER_CONTROLLER_DIGITAL_MAPPINGS.at(pros::E_CONTROLLER_DIGITAL_A)
        );


        values_col1 = (
            std::to_string(Controller::partner.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)) + "\n"
            + std::to_string(Controller::partner.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) + "\n"
            + std::to_string(Controller::partner.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X)) + "\n"
            + std::to_string(Controller::partner.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
        );

        values_col2 = (
            std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_UP)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_X)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_B)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_Y)) + "\n"
            + std::to_string(Controller::partner.get_digital(pros::E_CONTROLLER_DIGITAL_A))
        );
    }

    if ( showing_values )
    {
        lv_label_set_text(button_col_one, values_col1.c_str());
        lv_label_set_text(button_col_two, values_col2.c_str());
    }

    else
    {
        lv_label_set_text(button_col_one, functions_col1.c_str());
        lv_label_set_text(button_col_two, functions_col2.c_str());
    }

}





ControllerDebug::ControllerDebug()
{
//reset statics
    cont = true;
    showing_values = 0;


//init screen
    controller_debug_screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(controller_debug_screen, &gray);

//init title label
    title_label = lv_label_create(controller_debug_screen, NULL);
    lv_label_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, CONTROLLER_CONTAINER_WIDTH);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Controller - Debug");

//init tabview
    tabview = lv_tabview_create(controller_debug_screen, NULL);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BG, &gray);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_REL, &toggle_tabbtn_released);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_PR, &toggle_tabbtn_pressed);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_INDIC, &sw_indic);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, &toggle_tabbtn_pressed);
    //lv_tabview_set_tab_load_action(tabview,  tab_load_action);
    lv_obj_set_width(tabview, CONTROLLER_CONTAINER_WIDTH);
    lv_obj_set_height(tabview, 200);

//init tabs
    general_tab = lv_tabview_add_tab(tabview, "General");
    master_tab = lv_tabview_add_tab(tabview, "Master Controller");
    partner_tab = lv_tabview_add_tab(tabview, "Partner Controller");


//init back button
    //button
    btn_back = lv_btn_create(controller_debug_screen, NULL);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, btn_back_action);
    lv_obj_set_width(btn_back, 75);
    lv_obj_set_height(btn_back, 25);

    //label
    btn_back_label = lv_label_create(btn_back, NULL);
    lv_obj_set_style(btn_back_label, &heading_text);
    lv_label_set_text(btn_back_label, "Back");


//init button to switch between showing values and functions
    //button
    btn_show_values = lv_btn_create(controller_debug_screen, NULL);
    lv_btn_set_style(btn_show_values, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_show_values, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_show_values, LV_BTN_ACTION_CLICK, btn_show_values_action);
    lv_obj_set_width(btn_show_values, 150);
    lv_obj_set_height(btn_show_values, 25);

    //label
    btn_show_values_label = lv_label_create(btn_show_values, NULL);
    lv_obj_set_style(btn_show_values_label, &heading_text);
    lv_label_set_text(btn_show_values_label, "Show Values");

    lv_obj_set_hidden(btn_show_values, true);  //set hidden because button is
                                               //not needed on the default tab


//init tabs from other classes
    GeneralControllerDebugInit(general_tab);


//set positions
    lv_obj_set_pos(btn_back, 30, 210);
    lv_obj_align(btn_show_values, controller_debug_screen, LV_ALIGN_IN_BOTTOM_MID, 0, -5);

    lv_obj_set_pos(title_label, 180, 5);

    lv_obj_set_pos(tabview, 20, 25);
}




ControllerDebug::~ControllerDebug()
{
    lv_obj_del(controller_debug_screen);
}




/**
 * switches between showing values or function by setting variable to the
 * opposite of itself and then it updates the text label based on the new value
 */

lv_res_t ControllerDebug::btn_show_values_action(lv_obj_t *btn)
{
    showing_values = !showing_values;
    if ( showing_values )
    {
        lv_label_set_text(btn_show_values_label, "Show Functions");
    }
    else
    {
        lv_label_set_text(btn_show_values_label, "Show Values");
    }

    return LV_RES_OK;
}




/**
 * callback funciton that exits main loop when button is pressed
 */
lv_res_t ControllerDebug::btn_back_action(lv_obj_t *btn)
{
    cont = false;
    return LV_RES_OK;
}




/**
 * main loop that updates controller information
 */
void ControllerDebug::debug()
{
    //used to check if user wants to continue cycling through
    //tabs. Will be set to zero and loop will break if user hits
    //the back button
    cont = true;

    lv_tabview_set_tab_act(tabview, 0, NULL);
    lv_scr_load(controller_debug_screen);

    //init tabs from other classes
    ControllerTab controller_tab(master_tab);
    ControllerTab controller_tab2(partner_tab);

    while ( cont )
    {
        switch ( lv_tabview_get_tab_act(tabview) ) //switches to tab user wants to go to
        {
            case 0:
                lv_obj_set_hidden(btn_show_values, true);
                update_general_info();
                break;
            case 1:
                lv_obj_set_hidden(btn_show_values, false);
                controller_tab.update(pros::E_CONTROLLER_MASTER, showing_values);
                break;
            case 2:
                lv_obj_set_hidden(btn_show_values, false);
                controller_tab2.update(pros::E_CONTROLLER_PARTNER, showing_values);
                break;
        }
        pros::delay(200);
    }

}
