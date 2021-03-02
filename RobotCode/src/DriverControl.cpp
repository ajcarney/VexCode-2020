/**
 * @file: ./RobotCode/src/DriverControl.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: DriverControl.hpp
 *
 */

#include <cstdlib>
#include <cmath>

#include "../include/main.h"

#include "objects/lcdCode/gui.hpp"
#include "objects/subsystems/chassis.hpp"
#include "objects/subsystems/Indexer.hpp"
#include "objects/subsystems/intakes.hpp"
#include "objects/controller/controller.hpp"
#include "objects/motors/Motors.hpp"
#include "objects/sensors/Sensors.hpp"
#include "Configuration.hpp"
#include "DriverControl.hpp"
#include "Configuration.hpp"



/**
 * uses if statements to control motor based on controller settings
 */
void driver_control(void*)
{
    Configuration *config = Configuration::get_instance();
    Autons autons;
    if(autons.AUTONOMOUS_COLORS.at(autons.selected_number) == "blue") {
        config->filter_color = "red";
    } else if(autons.AUTONOMOUS_COLORS.at(autons.selected_number) == "red") {
        config->filter_color = "blue";
    } else {
        config->filter_color = "none";
    }
    config->filter_color = "blue";

    Controller controllers;

    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, config->filter_color);
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    
    int left_analog_y = 0;
    int right_analog_y = 0;

    bool auto_filter = true;
    bool hold_intakes_out = true;
    int intake_start_time = 0;  // no possible way to think indexer should run at the start of driver control

    controllers.master.print(0, 0, "Auto Filter %s     ", config->filter_color);

    while ( true ) {
        controllers.update_button_history();

    // section for front roller intake movement
        if(controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_R1)) {  // define velocity for main intake
            intakes.intake();
            intake_start_time = pros::millis();
        } else if(hold_intakes_out){  // rest state is outward with motor power
            intakes.hold_outward();
        } else {  // rest state is no motor power
            intakes.stop();
        }
        
        if(controllers.btn_get_release(pros::E_CONTROLLER_DIGITAL_R2)) {
            hold_intakes_out = !hold_intakes_out;
        }

    // section for indexer motion
        if(controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_L1) && auto_filter) {  // define movement for indexer subsystem
            indexer.auto_index();
        } else if(controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_L1) && !auto_filter) {
            indexer.index();
        } else if(controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
            indexer.run_upper_roller_reverse();
        } else if(controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_L2) && auto_filter) {
            indexer.index();
        } else if(controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_L2) && !auto_filter) {
            indexer.index();
        } else if(controllers.master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)) {
            indexer.fix_ball(true);
        } else if(controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_X)) {
            indexer.index_no_backboard();
        } else if (pros::millis() < intake_start_time + 1000 && auto_filter) {
            indexer.auto_increment();
        } else {
            indexer.stop();
        }
        
        if(controllers.btn_get_release(pros::E_CONTROLLER_DIGITAL_B)) {
            auto_filter = !auto_filter;
            if(auto_filter) {  // give different message if not auto filtering
                controllers.master.print(0, 0, "Auto Filter %s     ", config->filter_color);
            } else {
                controllers.master.print(0, 0, "Man Filter %s     ", config->filter_color);
            }
        }

    // section for setting filter color
        if(controllers.btn_get_release(pros::E_CONTROLLER_DIGITAL_A)) {  // cycle filter colors
            if(config->filter_color == "red") {
                config->filter_color = "blue";
            } else if(config->filter_color == "blue") {
                config->filter_color = "none";
            } else if(config->filter_color == "none") {
                config->filter_color = "red";
            }
            
            if(auto_filter) {  // give different message if not auto filtering
                controllers.master.print(0, 0, "Auto Filter %s     ", config->filter_color);
            } else {
                controllers.master.print(0, 0, "Man Filter %s     ", config->filter_color);
            }
            std::cout << "filtering " << config->filter_color << "\n";
            indexer.update_filter_color(config->filter_color);
        }


    // section for chassis movement
        if(std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) < 5) {   // define deadzone for left analog input on the y axis
            left_analog_y = 0;
        } else {
            left_analog_y = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        }

        if(std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) < 5) {   // define deadzone for right analog input on the y axis
            right_analog_y = 0;
        } else {
            right_analog_y = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        }

        // float corrected_speed = ( .000043326431866017 * std::pow( leftDriveSpeed, 3 ) ) + ( 0.29594689028631 * leftDriveSpeed);
        Motors::front_left.user_move(left_analog_y);
        Motors::back_left.user_move(left_analog_y);

        // float corrected_speed = ( .000043326431866017 * std::pow( rightDriveSpeed, 3 ) ) + ( 0.29594689028631 * rightDriveSpeed);
        Motors::front_right.user_move(right_analog_y);
        Motors::back_right.user_move(right_analog_y);


        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN) ) {
            Autons auton;
            auton.deploy();
        }
        
        if(controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_UP) && controllers.btn_is_pressing(pros::E_CONTROLLER_DIGITAL_Y)) {
            Autons auton;
            auton.run_autonomous();
        }

        pros::delay(5);

    }
}
