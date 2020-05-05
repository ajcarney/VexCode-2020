
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
#include "objects/controller/controller.hpp"
#include "objects/motors/Motors.hpp"
#include "objects/sensors/Sensors.hpp"
#include "Configuration.hpp"
#include "objects/subsystems/Tilter.hpp"
#include "objects/subsystems/Lift.hpp"
#include "DriverControl.hpp"
#include "Configuration.hpp"



/**
 * uses if statements to control motor based on controller settings
 * checks to set it to zero based on if static var in Motors class allows it
 * this is to make sure that other tasks can controll Motors too
 */
void driver_control(void*)
{
    Controller controllers;

    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 16 );
    Lift lift( Motors::lift, {0, 1100, 1372} );
    Tilter tilter( Motors::tilter );

    Configuration *configuration = Configuration::get_instance();

    bool shift_key_pressing = false;
    bool move_tilter_back = false;
    bool intake_running = false;
    int lift_setpoint = 0;
    int raised_lift_outake_speed = -60;


    while ( true )
    {
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT) || controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_Y) )
        {
            shift_key_pressing = true;
        }
        else 
        {
            shift_key_pressing = false;
        }
        
        
        //master left analog y moves left side of robot
        if ( std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) > 5 && !shift_key_pressing )
        {
            float leftDriveSpeed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
            float corrected_speed = ( .000043326431866017 * std::pow( leftDriveSpeed, 3 ) ) + ( 0.29594689028631 * leftDriveSpeed);
            Motors::front_left.move(leftDriveSpeed);
            Motors::back_left.move(leftDriveSpeed);
        }
        else if ( std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y)) > 5 && shift_key_pressing )
        {
            lift.move(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
            lift_setpoint = lift.get_potentiometer_value();
        }
        else if ( Motors::front_left.driver_control_allowed() && Motors::back_left.driver_control_allowed() )
        {
            Motors::front_left.move(0);
            Motors::back_left.move(0);
        }



        //master right analog y moves right side of robot
        if ( std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) > 5 && !shift_key_pressing)
        {
            float rightDriveSpeed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
            float corrected_speed = ( .000043326431866017 * std::pow( rightDriveSpeed, 3 ) ) + ( 0.29594689028631 * rightDriveSpeed);
            Motors::front_right.move(rightDriveSpeed);
            Motors::back_right.move(rightDriveSpeed);
        }
        else if ( Motors::front_right.driver_control_allowed() && Motors::back_right.driver_control_allowed() )
        {
            Motors::front_right.move(0);
            Motors::back_right.move(0);
        }
        
        if ( std::abs(controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y)) > 5 && shift_key_pressing )  //back up and outake macro
        {
            float drive_speed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) / 5;
            float intake_speed = controllers.master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) / 4;
            
            Motors::front_left.move(drive_speed);
            Motors::back_left.move(drive_speed);
            Motors::front_right.move(drive_speed);
            Motors::back_right.move(drive_speed);
            
            Motors::right_intake.move(intake_speed);
            Motors::left_intake.move(intake_speed);
            
            intake_running = true;
            
        }
        else 
        {
            intake_running = false;
        }



        //master right digital moves the intake
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && !shift_key_pressing )
        {
            Motors::right_intake.move(127);
            Motors::left_intake.move(127);
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_R2) && !shift_key_pressing )
        {
            if ( lift.get_potentiometer_value() < 500 )  
            {                
                Motors::right_intake.move(-127);
                Motors::left_intake.move(-127);
            }
            else if ( lift.get_potentiometer_value() >= 500 )  //outake slower when lift is up so cubes dont launch over tower
            {
                Motors::right_intake.move(raised_lift_outake_speed);
                Motors::left_intake.move(raised_lift_outake_speed);
            }
            
        }
        else if ( controllers.master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2) && shift_key_pressing )
        {
            raised_lift_outake_speed = raised_lift_outake_speed == -127 ? -60 : -127; //flip between outake speeds when lift is raised
        }
        else if ( Motors::right_intake.driver_control_allowed() && Motors::left_intake.driver_control_allowed() && !intake_running )
        {
            Motors::right_intake.move(0);
            Motors::left_intake.move(0);
        }




        //master up arrow moves tilter forward
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_UP) && !shift_key_pressing )
        {
            tilter.move(127, true);
            move_tilter_back = false;
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN) && !shift_key_pressing || move_tilter_back)
        {
            if ( !Sensors::limit_switch.get_value() )
            {
                Motors::tilter.move(-127);
            }
            else 
            {
                Motors::tilter.move(0);
                Motors::tilter.tare_encoder();
                move_tilter_back = false;
            }
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_UP) && shift_key_pressing )
        {
            tilter.move(127, false);
            move_tilter_back = false;
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN) && shift_key_pressing )
        {
            move_tilter_back = true;
        }
        else if ( Motors::tilter.driver_control_allowed() )
        {
            Motors::tilter.move(0);
        }




        //master left digital moves the lift
        if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !shift_key_pressing )
        {
            lift.move(127);
            lift_setpoint = lift.get_potentiometer_value();
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && !shift_key_pressing )
        {
            lift.move(-127);
            lift_setpoint = lift.get_potentiometer_value();
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && shift_key_pressing )
        {
            lift_setpoint = lift.get_setpoint(1);
        }
        else if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && shift_key_pressing )
        {
            lift_setpoint = lift.get_setpoint(-1);
        }
        else
        {
            lift.move_to(lift_setpoint, false, false);
        }




        //master button B cycles brakemode
        if (controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_B))
        {
            if ( Motors::front_left.get_brake_mode() == pros::E_MOTOR_BRAKE_COAST )
            {
                Motors::front_left.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
                Motors::back_left.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
                Motors::front_right.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
                Motors::back_right.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            }
            else
            {
                Motors::front_left.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                Motors::back_left.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                Motors::front_right.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                Motors::back_right.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            }
        }
        
        
        
        #ifdef AUTON_DEBUG
            if (controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_A) && shift_key_pressing )
            {

                Autons auton;
                
                Motors::front_left.disable_driver_control();
                Motors::front_left.disable_driver_control();
                Motors::back_right.disable_driver_control();
                Motors::back_left.disable_driver_control();
                Motors::right_intake.disable_driver_control();
                Motors::left_intake.disable_driver_control();
                Motors::tilter.disable_driver_control();
                Motors::lift.disable_driver_control();
                
                switch(DriverControlLCD::auton)
                {
                    case 1:
                       break;

                    case 2:
                       auton.five_cube_red_small_zone(OptionsScreen::cnfg);
                       break;

                    case 3:
                       auton.five_cube_blue_small_zone(OptionsScreen::cnfg);
                       break;

                    case 4:
                       auton.seven_cube_red_small_zone(OptionsScreen::cnfg);
                       break;

                    case 5:
                       auton.seven_cube_blue_small_zone(OptionsScreen::cnfg);
                       break;

                    case 6:
                        auton.red_big_zone(OptionsScreen::cnfg);
                        break;

                    case 7:
                        auton.blue_big_zone(OptionsScreen::cnfg);
                        break;

                    case 8:
                        auton.one_pt(OptionsScreen::cnfg);
                        break;

                    case 9:
                        auton.skills(OptionsScreen::cnfg);
                        break;

                }
            
                Motors::front_left.enable_driver_control();
                Motors::front_left.enable_driver_control();
                Motors::back_right.enable_driver_control();
                Motors::back_left.enable_driver_control();
                Motors::right_intake.enable_driver_control();
                Motors::left_intake.enable_driver_control();
                Motors::tilter.enable_driver_control();
                Motors::lift.enable_driver_control();
            }
            
            if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) )
            {
                Autons auton;
                auton.deploy();
            }
            
            if ( controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_X) )
            {
                Autons auton;
                auton.dump_stack();
            }
            
        #endif


        pros::delay(20);
    }
}
