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

#include "objects/robotChassis/chassis.hpp"
#include "objects/controller/controller.hpp"
#include "objects/motors/Motors.hpp"
#include "objects/sensors/Sensors.hpp"
#include "Configuration.hpp"
#include "objects/tilter/Tilter.hpp"
#include "DriverControl.hpp"
#include "Configuration.hpp"


/**
 * calls a blocking function for moving tilter to a set value
 * function will continuously be called while task is enabled
 */
void tilter_holding_strength(void*)
{
    Tilter tilter;
    while ( true )
    {
        tilter.move_to(100);
        pros::delay(20);
    }
}


/**
 * uses if statements to control motor based on controller settings
 * checks to set it to zero based on if static var in Motors class allows it
 * this is to make sure that other tasks can controll motors too
 */
void driver_control(void*)
{
    Controller controllers;
    Motors *motors = Motors::get_instance();

    Chassis chassis;
    Tilter tilter;
    Sensors sensors;

    Configuration *configuration = Configuration::get_instance();

    bool hold_tilter = true;
    int intake_speed = 0;
    
    bool right_x_axis_active = false;
    bool left_x_axis_active = false;
    bool move_tilter_home = false;
    
    int down_arrow_last_click = 0;


    while ( true )
    {
        //master left analog y moves left side of robot
        if (
            (std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) > 5 &&
            (std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)) < 20))
            || 
            (std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) > 20)
        )
        {
            float leftDriveSpeed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
            float corrected_speed = ( .000043326431866017 * std::pow( leftDriveSpeed, 3 ) ) + ( 0.29594689028631 * leftDriveSpeed);
            motors->frontLeft->move(corrected_speed);
            motors->backLeft->move(corrected_speed);
        }
        else if ( motors->allow_left_chassis && !right_x_axis_active && !left_x_axis_active  )
        {
            motors->frontLeft->move(0);
            motors->backLeft->move(0);

        }
        
        // uncomment for strafing functionality
        // //master left analog x moves strafes left and right
        // if (
        //     std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)) > 20 &&
        //     std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) < 5
        // )
        // {
        //     left_x_axis_active = true;
        // 
        //     float drive_speed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
        //     float corrected_speed = ( .000043326431866017 * std::pow( drive_speed, 3 ) ) + ( 0.29594689028631 * drive_speed);
        // 
        //     //set up to strafe right, will strafe left when controller values are negative
        //     motors->frontRight->move(-drive_speed);
        //     motors->backRight->move(drive_speed);
        //     motors->frontLeft->move(drive_speed);
        //     motors->backLeft->move(-drive_speed);
        // 
        //     motors->right_intake->move(intake_speed);
        //     motors->left_intake->move(intake_speed);
        // 
        // 
        // }
        // else
        // {
        //     left_x_axis_active = false;
        // }



        //master right analog y moves right side of robot
        if (
            (std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) > 5 &&
            (std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X)) < 20))
            || 
            (std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) > 20)
        )
        {
            float rightDriveSpeed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
            float corrected_speed = ( .000043326431866017 * std::pow( rightDriveSpeed, 3 ) ) + ( 0.29594689028631 * rightDriveSpeed);
            motors->frontRight->move(corrected_speed);
            motors->backRight->move(corrected_speed);
        }
        else if ( motors->allow_right_chassis && !right_x_axis_active && !left_x_axis_active )
        {
            motors->frontRight->move(0);
            motors->backRight->move(0);

        }
        
        
        //master right analog x moves both sides of chassis slowly and outake slowly
        //or move forward and intake slowly
        if (
            controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X) > 20 &&
            std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) < 5
        )
        {
            right_x_axis_active = true;
            
            float drive_speed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X) / -5;
            float intake_speed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X) / -4;

            motors->frontRight->move(drive_speed);
            motors->backRight->move(drive_speed);
            motors->frontLeft->move(drive_speed);
            motors->backLeft->move(drive_speed);
            
            motors->right_intake->move(intake_speed);
            motors->left_intake->move(intake_speed);
            
            
        }
        else
        {
            right_x_axis_active = false;
        }
        


        //master right digital moves the intake
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_R1) )
        {
            motors->right_intake->move(127);
            motors->left_intake->move(127);
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_R2) )
        {
            motors->right_intake->move(-127);
            motors->left_intake->move(-127);
        }
        // uncomment for intake always running functionality
        // else if ( intake_speed != 0 )
        // {
        //     motors->right_intake->move(intake_speed);
        //     motors->left_intake->move(intake_speed);
        // }
        // else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_UP) )
        // {
        //     motors->right_intake->move(-40);
        //     motors->left_intake->move(-40);
        // }
        else if ( motors->allow_intake && !intake_speed && !right_x_axis_active )
        {
            motors->right_intake->move(0);
            motors->left_intake->move(0);
        }


        // uncomment for intake always running functionality
        // //master up and down arrows change the intake speed
        // if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_UP) )
        // {
        //     std::vector<int>::iterator elem = std::find (configuration->intake_speeds.begin(),
        //                                                  configuration->intake_speeds.end(),
        //                                                  intake_speed);
        //     int index = std::distance(configuration->intake_speeds.begin(), elem);
        //     index += 1;
        //     if ( index > configuration->intake_speeds.size() - 1 )  //cap speed
        //     {
        //         index = configuration->intake_speeds.size() - 1;
        //     }
        //     intake_speed = configuration->intake_speeds.at(index);
        //     pros::delay(200);  //add delay to make up for bad hardware
        // }
        // else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN) )
        // {
        //     std::vector<int>::iterator elem = std::find (configuration->intake_speeds.begin(),
        //                                                  configuration->intake_speeds.end(),
        //                                                  intake_speed);
        //     int index = std::distance(configuration->intake_speeds.begin(), elem);
        //     index -= 1;
        //     if ( index < 0 )  //cap speed
        //     {
        //         index = 0;
        //     }
        //     intake_speed = configuration->intake_speeds.at(index);
        //     pros::delay(200);  //add delay to make up for bad hardware
        // }


        //master up arrow moves tilter forward
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_UP) )
        {
                motors->tilter->move(127);
        }
        // uncomment for auto move back functionality (requires a limit switch)
        // else if ( 
        //     controllers.master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN) 
        //     && (pros::millis() - down_arrow_last_click) < 500)
        // {
        //     down_arrow_last_click = pros::millis();
        //     if ( !sensors.getLimitSwitch() )
        //     {
        //         motors->tilter->move(-127);
        //     }
        //     else 
        //     {
        //         motors->tilter->move(0);
        //     }
        // }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN) )
        {
            motors->tilter->move(-127);
        }
        else if ( motors->allow_tilter && hold_tilter)
        {
            motors->tilter->move(0);
        }


        //master left digital moves the lift
        //partner right analog y moves the lift
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_L1) )
        {
            motors->lift->move(127);
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) )
        {
            motors->lift->move(-127);
        }
        else if (std::abs(controllers.partner.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) > 0)
        {
            float lift_speed = controllers.partner.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
            float corrected_speed = ( .000043326431866017 * std::pow( lift_speed, 3 ) ) + ( 0.29594689028631 * lift_speed);
            motors->lift->move(corrected_speed);
        }
        else if ( motors->allow_lift && hold_tilter)
        {
            motors->lift->move(0);
        }


        //master button B cycles brakemode
        if (controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_B))
        {
            if ( motors->frontLeft->get_brake_mode() == pros::E_MOTOR_BRAKE_COAST )
            {
                motors->frontLeft->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
                motors->backLeft->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
                motors->frontRight->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
                motors->backRight->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            }
            else
            {
                motors->frontLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                motors->backLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                motors->frontRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                motors->backRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            }
        }


        pros::delay(20);
    }
}
