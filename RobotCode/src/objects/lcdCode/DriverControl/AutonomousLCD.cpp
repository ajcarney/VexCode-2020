/**
 * @file: ./RobotCode/src/lcdCode/DriverControl/AutonomousLCD.cpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * @see: DriverControlLCD.hpp
 *
 * contains methods for driver control lcd
 */

#include "main.h"

#include "../../../Autons.hpp"
#include "../../logger/Logger.hpp"
#include "../../position_tracking/PositionTracker.hpp"
#include "../AutonSelection/OptionsScreen.hpp"
#include "../Debug/Debug.hpp"
#include "AutonomousLCD.hpp"

LV_IMG_DECLARE(logo);


AutonomousLCD::AutonomousLCD()
{
    screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(screen, &gray);
    
// init labels
    title_label = lv_label_create(screen, NULL);
    lv_obj_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, 300);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Autonomous");


    auton_label = lv_label_create(screen, NULL);
    lv_obj_set_style(auton_label, &subheading_text);
    lv_obj_set_width(auton_label, 300);
    lv_obj_set_height(auton_label, 20);
    lv_label_set_align(auton_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(auton_label, "");
    
    
    description_label = lv_label_create(screen, NULL);
    lv_obj_set_style(description_label, &subheading_text);
    lv_obj_set_width(description_label, 300);
    lv_obj_set_height(description_label, 20);
    lv_label_set_align(description_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(description_label, "");

    
// init image area 
    logo_img = lv_img_create(screen, NULL);
    lv_img_set_src(logo_img, &logo);
    lv_img_set_auto_size(logo_img, false);
    lv_obj_set_width(logo_img, 210);
    lv_obj_set_height(logo_img, 150);

    
// place objects
    lv_obj_set_pos(title_label, 180, 9);
    
    lv_obj_set_pos(logo_img, 30, 40); 
    
    lv_obj_set_pos(auton_label, 280, 40);
    lv_obj_set_pos(description_label, 280, 80);
    
}



AutonomousLCD::~AutonomousLCD()
{
    lv_obj_del(screen);
}




/**
 * updates colors and borders during driver control
 * keeps data relevent
 */
void AutonomousLCD::update_labels(int auton_number)
{
    Autons autons;
    
    lv_scr_load(screen);
    
    lv_label_set_text(auton_label, autons.AUTONOMOUS_NAMES.at(auton_number));
    lv_label_set_text(description_label, autons.AUTONOMOUS_DESCRIPTIONS.at(auton_number));
}
