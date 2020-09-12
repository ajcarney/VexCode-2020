#include <csignal>

#include "main.h"

#include "Autons.hpp"
#include "DriverControl.hpp"
#include "Configuration.hpp"
#include "objects/controller/controller.hpp"
#include "objects/lcdCode/gui.hpp"
#include "objects/lcdCode/DriverControl/DriverControlLCD.hpp"
#include "objects/lcdCode/DriverControl/AutonomousLCD.hpp"
#include "objects/lcdCode/TemporaryScreen.hpp"
#include "objects/motors/Motors.hpp"
#include "objects/motors/MotorThread.hpp"
#include "objects/subsystems/chassis.hpp"
#include "objects/serial/Logger.hpp"
#include "objects/serial/Server.hpp"
#include "objects/subsystems/Lift.hpp"

int final_auton_choice;
AutonomousLCD auton_lcd;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
 void initialize()
 {
     pros::c::serctl(SERCTL_ACTIVATE, 0);  // I think this enables stdin (necessary to start server)
    //Sensors::potentiometer.calibrate();
    
    MotorThread* motor_thread = MotorThread::get_instance();
    motor_thread->register_motor(Motors::front_right);
    motor_thread->register_motor(Motors::front_left);
    motor_thread->register_motor(Motors::back_right);
    motor_thread->register_motor(Motors::back_left);
    motor_thread->register_motor(Motors::main_intake);
    motor_thread->register_motor(Motors::hoarding_intake);
    motor_thread->register_motor(Motors::lift);
    motor_thread->start_thread();
    
    pros::delay(100); //wait for terminal to start and lvgl
    Configuration* config = Configuration::get_instance();
    config->init();
    config->print_config_options();

    final_auton_choice = chooseAuton();

 	DriverControlLCD::auton = final_auton_choice;

    // std::cout << OptionsScreen::cnfg.use_hardcoded << '\n';
    // std::cout << OptionsScreen::cnfg.gyro_turn << '\n';
    // std::cout << OptionsScreen::cnfg.accelleration_ctrl << '\n';
    // std::cout << OptionsScreen::cnfg.check_motor_tmp << '\n';
    // std::cout << OptionsScreen::cnfg.use_previous_macros << '\n';
    // std::cout << OptionsScreen::cnfg.record << '\n';

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
void competition_initialize() {
    auton_lcd.update_labels(final_auton_choice);
}



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
    Autons auton;
    switch(final_auton_choice)
    {
        case 1:
           break;

        case 2:
            auton.one_pt(OptionsScreen::cnfg);
            break;

        case 3:
            auton.skills(OptionsScreen::cnfg);
            break;

    }

}


 void log_thread_fn( void* )
 {
     Logger logger;
     pros::ADIDigitalIn limit_switch('A');
     while ( 1 )
     {
        pros::delay(2000);
        std::cout << (limit_switch.get_value()) << "\n";
        if ( limit_switch.get_value() )
        {
            std::cout << "dumping" << "\n";
            logger.dump();
        }
     }
 }



 void Exit( int signal )
 {
     //Writer writer;
     std::cerr << "program caught " << signal << "\n" << std::flush;
     std::cerr << "errno: " << errno << "\n" << std::flush;
     std::cerr << "strerror: " << std::strerror(errno) << "\n" << std::flush;
     pros::delay(100); // wait for stdout to be flushed
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
     // pros::ADIAnalogIn l1 (1);
     // pros::ADIAnalogIn l2 (2);
     // pros::ADIAnalogIn l3 (3);
     // while(1) {
     //     std::cout << l1.get_value() << " " << l2.get_value() << " " << l3.get_value() << "\n";
     //     pros::delay(50);
     // }
    Logger logger;
    // pros::ADIDigitalIn limit_switch('A');
    // pros::Task write_task (log_thread_fn,
    //                       (void*)NULL,
    //                       TASK_PRIORITY_DEFAULT,
    //                       TASK_STACK_DEPTH_DEFAULT,
    //                       "logger_thread");
    
    
    
    Server server;
    server.clear_stdin();
    server.start_server();
    server.set_debug_mode(true);
    
     // int stop = pros::millis() + 8000;
     // 
     // Lift lift(Motors::lift, {0, 800});
     // while ( pros::millis() < stop )
     // {
     //     lift.move_to(900, false, true);
     //     pros::delay(10);
     // }
     // stop = pros::millis() + 2000;
     // while ( pros::millis() < stop )
     // {
     //     lift.move_to(0, false, true);
     //     pros::delay(10);
     // }
     // logger.dump();
     // logger.dump();
     // logger.dump();
     // logger.dump();
     // logger.dump();
     // logger.dump();
     // logger.dump();
     // logger.dump();
     // 
     // Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
     // int stop = pros::millis() + 8000;
     // 
     // chassis.turn_left(13, 12000, INT32_MAX, true, false, true);
     // 
     // while ( pros::millis() < stop )
     // {
     //     chassis.turn_left(13, 12000, INT32_MAX, false, false, true );
     //     pros::delay(10);
     // }
    
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
    
    pros::Task driver_control_task (driver_control,
                                    (void*)NULL,
                                    TASK_PRIORITY_DEFAULT,
                                    TASK_STACK_DEPTH_DEFAULT,
                                    "DriverControlTask");
    
     // Motors motors;
     // Motors::record_macro();
     // 
     // Writer writer;
     // while( writer.get_count() > 0 )
     // {
     //     std::cout << pros::millis() << " " << writer.get_count() << "\n";
     //     pros::delay(1);
     // }
     // 
     // std::cout << "done\n";
    
     //update controller with color of cube and if it is loaded or not
    
    // Controller controllers;
    // std::string controller_text = "no cube loaded";
    // std::string prev_controller_text = "";
    DriverControlLCD lcd(final_auton_choice);
    while(1)
    {
        lcd.update_labels();
        server.handle_requests(50);
        // std::cout << "handling requests\n";
    
        // if ( limit_switch.get_value() )
        // {
        //     std::cout << "dumping" << "\n";
        logger.dump();
        // }
    
        pros::delay(5);
    }
}
