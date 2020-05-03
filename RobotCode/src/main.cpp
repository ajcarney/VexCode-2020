#include <csignal>
#include <cstdlib>
#include <errno.h>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <clocale>

#include "main.h"

#include "Autons.hpp"
#include "DriverControl.hpp"
#include "Configuration.hpp"
#include "objects/controller/controller.hpp"
#include "objects/lcdCode/DriverControl/LCDTask.hpp"
#include "objects/lcdCode/gui.hpp"
#include "objects/lcdCode/TemporaryScreen.hpp"
#include "objects/motors/Motors.hpp"
#include "objects/motors/MotorThread.hpp"
#include "objects/robotChassis/chassis.hpp"
#include "objects/writer/Writer.hpp"


int final_auton_choice;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
 void initialize()
 {
    pros::delay(100); //wait for terminal to start and lvgl
    Configuration* config = Configuration::get_instance();
    config->init();
    config->print_config_options();

    Motors *motors = Motors::get_instance();

    MotorThread* motor_thread = MotorThread::get_instance();
    motor_thread->start_thread();

    final_auton_choice = chooseAuton();

 	DriverControlLCD::auton = final_auton_choice;

    std::cout << OptionsScreen::cnfg.use_hardcoded << '\n';
    std::cout << OptionsScreen::cnfg.gyro_turn << '\n';
    std::cout << OptionsScreen::cnfg.accelleration_ctrl << '\n';
    std::cout << OptionsScreen::cnfg.check_motor_tmp << '\n';
    std::cout << OptionsScreen::cnfg.use_previous_macros << '\n';
    std::cout << OptionsScreen::cnfg.record << '\n';

    std::cout << "initalize finished" << "\n";
    lv_scr_load(tempScreen::temp_screen);
 }



/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}



/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}



/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    lv_scr_load(tempScreen::temp_screen);
    Autons auton;
    switch(final_auton_choice)
    {
        case 1:
           break;

        case 2:
           auton.auton1(OptionsScreen::cnfg);
           break;

        case 3:
           auton.auton2(OptionsScreen::cnfg);
           break;

        case 4:
           auton.auton3(OptionsScreen::cnfg);
           break;

        case 5:
           auton.auton4(OptionsScreen::cnfg);
           break;

        case 6:
            auton.auton5(OptionsScreen::cnfg);
            break;

        case 7:
            auton.auton6(OptionsScreen::cnfg);
            break;



    }

}


 void wr( void* )
 {
     Writer writer;
     while ( 1 )
     {
         //std::cout << "dumping " << writer.get_count() << " items\n";
         //pros::delay(50);
         //std::cout << writer.get_count() << "\n";
         writer.dump();
         //std::cout << pros::millis() << "\n";
     }
 }



 void Exit( int signal )
 {
     //Writer writer;
     std::cerr << "program caught " << signal << "\n" << std::flush;
     std::cerr << "errno: " << errno << "\n" << std::flush;
     std::cerr << "strerror: " << std::strerror(errno) << "\n" << std::flush;
     raise(signal);
 }



/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
 void opcontrol() {
     std::cout << "opcontrol started\n";

     std::signal(SIGSEGV, Exit);
     std::signal(SIGTERM, Exit);
     std::signal(SIGINT, Exit);
     std::signal(SIGILL, Exit);
     std::signal(SIGABRT, Exit);
     std::signal(SIGFPE, Exit);
     std::signal(SIGBUS, Exit);
     std::signal(SIGALRM, Exit);
     std::signal(SIGSTOP, Exit);
     std::signal(SIGUSR1, Exit);
     std::signal(SIGUSR2, Exit);
     std::signal(SIGKILL, Exit);

     pros::delay(100);

     lv_scr_load(tempScreen::temp_screen);
    
     Motors *motors = Motors::get_instance(); //init singleton motors object
     motors->lift->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
     motors->right_intake->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
     motors->left_intake->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

     pros::Task driver_control_task (driver_control,
                                    (void*)NULL,
                                    TASK_PRIORITY_DEFAULT,
                                    TASK_STACK_DEPTH_DEFAULT,
                                    "DriverControlTask");

/*     pros::Task write_task (wr,
                          (void*)NULL,
                          TASK_PRIORITY_DEFAULT,
                          TASK_STACK_DEPTH_DEFAULT,
                          "write_task");
     std::cout << "starting\n";
     Motors motors;
     motors.record_macro();

     Writer writer;
     while( writer.get_count() > 0 )
     {
         std::cout << pros::millis() << " " << writer.get_count() << "\n";
         pros::delay(1);
     }

     std::cout << "done\n";*/



     while(1)
     {
         Controller controllers;
         Motors *motors = Motors::get_instance();
         //partner button A runs autonomous
         if (controllers.master.get_digital(pros::E_CONTROLLER_DIGITAL_A))
         {
             motors->allow_left_chassis = false;
             motors->allow_right_chassis = false;
             motors->allow_intake = false;
             motors->allow_tilter = false;
             motors->allow_lift = false;
             
             Autons auton;
             switch(final_auton_choice)
             {
                 case 1:
                    break;

                 case 2:
                    std::cout << "starting auton\n";
                    auton.auton1(OptionsScreen::cnfg);
                    std::cout << "auton finished\n";
                    break;

                 case 3:
                    auton.auton2(OptionsScreen::cnfg);
                    break;

                 case 4:
                    auton.auton3(OptionsScreen::cnfg);
                    break;

                 case 5:
                    auton.auton4(OptionsScreen::cnfg);
                    break;

                 case 6:
                     auton.auton5(OptionsScreen::cnfg);
                     break;

                 case 7:
                     auton.auton6(OptionsScreen::cnfg);
                     break;

             }
         }
         pros::delay(20);
     }
 }
