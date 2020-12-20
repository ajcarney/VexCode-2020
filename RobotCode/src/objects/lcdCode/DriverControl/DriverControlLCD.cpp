/**
 * @file: ./RobotCode/src/lcdCode/DriverControl/DriverControlLCD.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: DriverControlLCD.hpp
 *
 * contains methods for driver control lcd
 */

#include "main.h"

#include "../../../Autons.hpp"
#include "../../serial/Logger.hpp"
#include "../../position_tracking/PositionTracker.hpp"
#include "../AutonSelection/OptionsScreen.hpp"
#include "../Debug/Debug.hpp"
#include "DriverControlLCD.hpp"


bool DriverControlLCD::log_data = false;
bool DriverControlLCD::open_debugger = false;
std::string DriverControlLCD::toggle_logging_text = "Start Logging";
LV_IMG_DECLARE(logo);


DriverControlLCD::DriverControlLCD()
{
    log_data = false;
    
    screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(screen, &gray);
    
// init labels
    queue_size_label = lv_label_create(screen, NULL);
    lv_obj_set_style(queue_size_label, &subheading_text);
    lv_obj_set_width(queue_size_label, 300);
    lv_obj_set_height(queue_size_label, 20);
    lv_label_set_align(queue_size_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(queue_size_label, "Logger Queue Size: ");
    

    title_label = lv_label_create(screen, NULL);
    lv_obj_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, 300);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Driver Control");
    
// init image area 
    logo_img = lv_img_create(screen, NULL);
    lv_img_set_src(logo_img, &logo);
    lv_img_set_auto_size(logo_img, false);
    lv_obj_set_width(logo_img, 210);
    lv_obj_set_height(logo_img, 150);
    
// init buttons
    //button
    btn_debugger = lv_btn_create(screen, NULL);
    lv_btn_set_style(btn_debugger, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_debugger, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_debugger, LV_BTN_ACTION_CLICK, btn_debugger_action);
    lv_obj_set_width(btn_debugger, 180);
    lv_obj_set_height(btn_debugger, 25);

    //label
    btn_debugger_label = lv_label_create(btn_debugger, NULL);
    lv_obj_set_style(btn_debugger_label, &subheading_text);
    lv_label_set_text(btn_debugger_label, "Open Debugger");
    
    
    //button
    btn_run_auton = lv_btn_create(screen, NULL);
    lv_btn_set_style(btn_run_auton, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_run_auton, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_run_auton, LV_BTN_ACTION_CLICK, btn_run_auton_action);
    lv_obj_set_width(btn_run_auton, 180);
    lv_obj_set_height(btn_run_auton, 25);

    //label
    btn_run_auton_label = lv_label_create(btn_run_auton, NULL);
    lv_obj_set_style(btn_run_auton_label, &subheading_text);
    lv_label_set_text(btn_run_auton_label, "Run Auton");
    
    
    //button
    btn_toggle_logging = lv_btn_create(screen, NULL);
    lv_btn_set_style(btn_toggle_logging, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_toggle_logging, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_toggle_logging, LV_BTN_ACTION_CLICK, btn_toggle_logging_action);
    lv_obj_set_width(btn_toggle_logging, 180);
    lv_obj_set_height(btn_toggle_logging, 25);

    //label
    btn_toggle_logging_label = lv_label_create(btn_toggle_logging, NULL);
    lv_obj_set_style(btn_toggle_logging_label, &subheading_text);
    lv_label_set_text(btn_toggle_logging_label, toggle_logging_text.c_str());
    
    
    //button
    btn_flush_queue = lv_btn_create(screen, NULL);
    lv_btn_set_style(btn_flush_queue, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_flush_queue, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_flush_queue, LV_BTN_ACTION_CLICK, btn_flush_queue_action);
    lv_obj_set_width(btn_flush_queue, 180);
    lv_obj_set_height(btn_flush_queue, 25);

    //label
    btn_flush_queue_label = lv_label_create(btn_flush_queue, NULL);
    lv_obj_set_style(btn_flush_queue_label, &subheading_text);
    lv_label_set_text(btn_flush_queue_label, "Flush Logger Queue");
    
    
// place objects
    lv_obj_set_pos(title_label, 180, 9);
    lv_obj_set_pos(queue_size_label, 280, 200);
    lv_obj_set_pos(logo_img, 30, 40);

    lv_obj_set_pos(btn_debugger, 280, 40);
    lv_obj_set_pos(btn_run_auton, 280, 80);
    lv_obj_set_pos(btn_toggle_logging, 280, 120);
    lv_obj_set_pos(btn_flush_queue, 280, 160);    
    
}



DriverControlLCD::~DriverControlLCD()
{
    lv_obj_del(screen);
}




lv_res_t DriverControlLCD::btn_debugger_action(lv_obj_t *btn)
{
    open_debugger = true;
    return LV_RES_OK;
}



lv_res_t DriverControlLCD::btn_run_auton_action(lv_obj_t *btn)
{
    pros::delay(3000);
    
    Autons auton_obj;
    
    Motors::disable_driver_control();
    
    auton_obj.run_autonomous();

    Motors::enable_driver_control();
    
    return LV_RES_OK;
}



lv_res_t DriverControlLCD::btn_toggle_logging_action(lv_obj_t *btn)
{
    if(log_data)
    {
        log_data = false;
    }
    else 
    {
        log_data = true;
    }
    return LV_RES_OK;
}



lv_res_t DriverControlLCD::btn_flush_queue_action(lv_obj_t *btn)
{
    Logger logger;
    while(logger.get_count() > 0)
    {
        logger.dump();
    }
    return LV_RES_OK;
}





/**
 * updates colors and borders during driver control
 * keeps data relevent
 */
void DriverControlLCD::update_labels()
{
    lv_scr_load(screen);
    
    Logger logger;
    // PositionTracker* pos_tracker = PositionTracker::get_instance();
    
    // update toggle logging label text
    if(log_data)
    {
        lv_label_set_text(btn_toggle_logging_label, "Stop Logging");
        Motors::set_log_level(1);
    
        // pos_tracker->start_logging();
    
        Sensors::log_data();
    }
    else 
    {
        lv_label_set_text(btn_toggle_logging_label, "Start Logging");
        Motors::set_log_level(0);
    
        // pos_tracker->stop_logging();
    }
    
    if(open_debugger)
    {
        debug();
        open_debugger = false;
    }
    
    //update logger queue size label 
    std::string text = std::string("Logger Queue Size: ") + std::to_string(logger.get_count());
    lv_label_set_text(queue_size_label, text.c_str());
}
