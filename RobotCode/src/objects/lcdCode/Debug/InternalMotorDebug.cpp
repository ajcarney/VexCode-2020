/**
 * @file: ./RobotCode/src/lcdCode/Debug/InternalMotorDebug.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * @see: InternalMotorDebug.hpp
 *
 * contains class implementation for tuning the motors internal velocity PID 
 * controller
 */

#include <stdexcept>
#include <string>

#include "main.h"

#include "InternalMotorDebug.hpp"
#include "../Styles.hpp"
#include "../../../Configuration.hpp"
#include "../../motors/Motor.hpp"
#include "../../motors/MotorThread.hpp"
#include "../../serial/Logger.hpp"



bool InternalMotorDebug::cont = true;
bool InternalMotorDebug::run = false;
pros::motor_gearset_e_t InternalMotorDebug::current_gearset = pros::E_MOTOR_GEARSET_18;
pros::motor_brake_mode_e_t InternalMotorDebug::current_brake_mode = pros::E_MOTOR_BRAKE_COAST;


InternalMotorDebug::InternalMotorDebug() : 
    motor(1, pros::E_MOTOR_GEARSET_18, false)
{
    MotorThread* motor_thread = MotorThread::get_instance();
    motor_thread->register_motor(motor);
    
        
    cont = true;
    run = false; 
    
//screen
    main_screen = lv_obj_create(NULL, NULL);
    lv_obj_set_style(main_screen, &gray);

//init title label
    title_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(title_label, &heading_text);
    lv_obj_set_width(title_label, 440);
    lv_obj_set_height(title_label, 20);
    lv_label_set_align(title_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title_label, "Velocity PID Controller - Debugger");


    //init parameters side two
    //port
    port_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(port_label, &heading_text);
    lv_obj_set_width(port_label, 100);
    lv_obj_set_height(port_label, 40);
    lv_label_set_align(port_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(port_label, "Port");

    port_text_area = lv_ta_create(main_screen, NULL);
    lv_obj_set_style(port_text_area, &subheading_text);
    lv_ta_set_accepted_chars(port_text_area, "0123456789");
    lv_obj_set_size(port_text_area, 80, 20);
    lv_ta_set_text(port_text_area, "1");
    lv_ta_set_one_line(port_text_area, true);
        
    //gearset
    current_gearset = pros::E_MOTOR_GEARSET_18;
    
    gearset_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(gearset_label, &heading_text);
    lv_obj_set_width(gearset_label, 100);
    lv_obj_set_height(gearset_label, 40);
    lv_label_set_align(gearset_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(gearset_label, "Gearset");
    
    ddlist_gearset = lv_ddlist_create(main_screen, NULL);
    lv_ddlist_set_options(ddlist_gearset, "100\n200\n600");
    lv_obj_set_style(ddlist_gearset, &subheading_text);
    lv_obj_set_width(ddlist_gearset, 125);
    lv_obj_set_height(ddlist_gearset, 60);
    lv_ddlist_set_action(ddlist_gearset, ddlist_gearset_action);
    lv_ddlist_set_selected(ddlist_gearset, 1);

    //brakemode
    current_brake_mode = pros::E_MOTOR_BRAKE_COAST;

    brakemode_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(brakemode_label, &heading_text);
    lv_obj_set_width(brakemode_label, 100);
    lv_obj_set_height(brakemode_label, 40);
    lv_label_set_align(brakemode_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(brakemode_label, "Brakemode");

    ddlist_brake_mode = lv_ddlist_create(main_screen, NULL);
    lv_ddlist_set_options(ddlist_brake_mode, "Coast\n"
                                             "Brake\n"
                                             "PID Hold");
    lv_obj_set_style(ddlist_brake_mode, &subheading_text);
    lv_obj_set_width(ddlist_brake_mode, 125);
    lv_obj_set_height(ddlist_brake_mode, 20);
    lv_ddlist_set_action(ddlist_brake_mode, ddlist_brake_mode_action);
        
        
//init parameters side one
//kP
    kp_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(kp_label, &heading_text);
    lv_obj_set_width(kp_label, 440);
    lv_obj_set_height(kp_label, 20);
    lv_label_set_align(kp_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(kp_label, "kP");
    
    kp_text_area = lv_ta_create(main_screen, NULL);
    lv_obj_set_style(kp_text_area, &subheading_text);
    lv_ta_set_accepted_chars(kp_text_area, ".0123456789");
    lv_obj_set_size(kp_text_area, 80, 15);
    lv_ta_set_text(kp_text_area, std::to_string(motor.get_pid().kP).c_str());
    lv_ta_set_one_line(kp_text_area, true);
    
//kI    
    ki_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(ki_label, &heading_text);
    lv_obj_set_width(ki_label, 100);
    lv_obj_set_height(ki_label, 20);
    lv_label_set_align(ki_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(ki_label, "kI");
    
    ki_text_area = lv_ta_create(main_screen, NULL);
    lv_obj_set_style(ki_text_area, &subheading_text);
    lv_ta_set_accepted_chars(ki_text_area, ".0123456789");
    lv_obj_set_size(ki_text_area, 80, 15);
    lv_ta_set_text(ki_text_area, std::to_string(motor.get_pid().kI).c_str());
    lv_ta_set_one_line(ki_text_area, true);
    
//kD    
    kd_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(kd_label, &heading_text);
    lv_obj_set_width(kd_label, 100);
    lv_obj_set_height(kd_label, 20);
    lv_label_set_align(kd_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(kd_label, "kD");
    
    kd_text_area = lv_ta_create(main_screen, NULL);
    lv_obj_set_style(kd_text_area, &subheading_text);
    lv_ta_set_accepted_chars(kd_text_area, ".0123456789");
    lv_obj_set_size(kd_text_area, 80, 15);
    lv_ta_set_text(kd_text_area, std::to_string(motor.get_pid().kD).c_str());
    lv_ta_set_one_line(kd_text_area, true);
    
//I max    
    I_max_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(I_max_label, &heading_text);
    lv_obj_set_width(I_max_label, 100);
    lv_obj_set_height(I_max_label, 20);
    lv_label_set_align(I_max_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(I_max_label, "kI Max");
    
    I_max_text_area = lv_ta_create(main_screen, NULL);
    lv_obj_set_style(I_max_text_area, &subheading_text);
    lv_ta_set_accepted_chars(I_max_text_area, ".0123456789");
    lv_obj_set_size(I_max_text_area, 80, 15);
    lv_ta_set_text(I_max_text_area, std::to_string(motor.get_pid().I_max).c_str());
    lv_ta_set_one_line(I_max_text_area, true);
    
//slew rate    
    slew_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(slew_label, &heading_text);
    lv_obj_set_width(slew_label, 100);
    lv_obj_set_height(slew_label, 40);
    lv_label_set_align(slew_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(slew_label, "Slew Rate");
    
    slew_text_area = lv_ta_create(main_screen, NULL);
    lv_obj_set_style(slew_text_area, &subheading_text);
    lv_ta_set_accepted_chars(slew_text_area, "0123456789");
    lv_obj_set_size(slew_text_area, 80, 15);
    lv_ta_set_text(slew_text_area, std::to_string(motor.get_slew_rate()).c_str());
    lv_ta_set_one_line(slew_text_area, true);

//setpoint_label 
    setpoint_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(setpoint_label, &heading_text);
    lv_obj_set_width(setpoint_label, 100);
    lv_obj_set_height(setpoint_label, 40);
    lv_label_set_align(setpoint_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(setpoint_label, "Setpoint");

    setpoint_text_area = lv_ta_create(main_screen, NULL);
    lv_obj_set_style(setpoint_text_area, &subheading_text);
    lv_ta_set_accepted_chars(setpoint_text_area, "0123456789");
    lv_obj_set_size(setpoint_text_area, 80, 15);
    lv_ta_set_text(setpoint_text_area, "200");
    lv_ta_set_one_line(setpoint_text_area, true);

//duration 
    duration_label = lv_label_create(main_screen, NULL);
    lv_label_set_style(duration_label, &heading_text);
    lv_obj_set_width(duration_label, 100);
    lv_obj_set_height(duration_label, 40);
    lv_label_set_align(duration_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(duration_label, "Duration");

    duration_text_area = lv_ta_create(main_screen, NULL);
    lv_obj_set_style(duration_text_area, &subheading_text);
    lv_ta_set_accepted_chars(duration_text_area, "0123456789");
    lv_obj_set_size(duration_text_area, 80, 15);
    lv_ta_set_text(duration_text_area, "10000");
    lv_ta_set_one_line(duration_text_area, true);



//information label 
    information_label = lv_label_create(main_screen, NULL);
    lv_obj_set_style(information_label, &subheading_text);
    lv_label_set_text(information_label, "Info");


//init back button
    //button
    btn_back = lv_btn_create(main_screen, NULL);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, btn_back_action);
    lv_obj_set_width(btn_back, 75);
    lv_obj_set_height(btn_back, 25);

    //label
    btn_back_label = lv_label_create(btn_back, NULL);
    lv_obj_set_style(btn_back_label, &heading_text);
    lv_label_set_text(btn_back_label, "Back");    
    
    
//init run button
    //button
    btn_run = lv_btn_create(main_screen, NULL);
    lv_btn_set_style(btn_run, LV_BTN_STYLE_REL, &toggle_btn_released);
    lv_btn_set_style(btn_run, LV_BTN_STYLE_PR, &toggle_btn_pressed);
    lv_btn_set_action(btn_run, LV_BTN_ACTION_CLICK, btn_run_action);
    lv_obj_set_width(btn_run, 150);
    lv_obj_set_height(btn_run, 25);

    //label
    btn_run_label = lv_label_create(btn_run, NULL);
    lv_obj_set_style(btn_run_label, &heading_text);
    lv_label_set_text(btn_run_label, "Run Unit Test");


//set up keyboard
    keyboard = lv_kb_create(main_screen, NULL);
    // lv_kb_set_ta(keyboard, kp_text_area);
    // lv_kb_set_ta(keyboard, ki_text_area);
    // lv_kb_set_ta(keyboard, kd_text_area);
    // lv_kb_set_ta(keyboard, I_max_text_area);
    // lv_kb_set_ta(keyboard, slew_text_area);
    
    //lv_ta_set_action(port_text_area, LV_EVENT_PRESSED);
    
//set positions
//title
    lv_obj_set_pos(title_label, 100, 5);
    
//bottom buttons
    lv_obj_set_pos(btn_back, 30, 210);
    lv_obj_set_pos(btn_run, 300, 210);

//parameters side 1
    lv_obj_set_pos(kp_label, 20, 30);
    lv_obj_set_pos(ki_label, 20, 55);
    lv_obj_set_pos(kd_label, 20, 80);
    lv_obj_set_pos(I_max_label, 20, 105);
    lv_obj_set_pos(slew_label, 20, 130);
    lv_obj_set_pos(setpoint_label, 20, 155);
    lv_obj_set_pos(duration_label, 20, 180);
    
    lv_obj_set_pos(kp_text_area, 130, 23);
    lv_obj_set_pos(ki_text_area, 130, 48);
    lv_obj_set_pos(kd_text_area, 130, 73);
    lv_obj_set_pos(I_max_text_area, 130, 98);
    lv_obj_set_pos(slew_text_area, 130, 123);
    lv_obj_set_pos(setpoint_text_area, 130, 148);
    lv_obj_set_pos(duration_text_area, 130, 173);

//parameters side 2
    lv_obj_set_pos(port_label, 240, 40);
    lv_obj_set_pos(gearset_label, 240, 75);
    lv_obj_set_pos(brakemode_label, 240, 100);
    
    lv_obj_set_pos(port_text_area, 350, 33);
    lv_obj_set_pos(ddlist_gearset, 350, 75);
    lv_obj_set_pos(ddlist_brake_mode, 350, 100);
    
//information
    lv_obj_set_pos(information_label, 240, 140);



}

InternalMotorDebug::~InternalMotorDebug()
{
    MotorThread* motor_thread = MotorThread::get_instance();
    motor_thread->unregister_motor(motor);
    
    lv_obj_del(main_screen);
}


/**
 * sets cont to false signifying user wants to go back, main loop will exit
 */
lv_res_t InternalMotorDebug::btn_back_action(lv_obj_t *btn)
{
    cont = false;
    return LV_RES_OK;
}

lv_res_t InternalMotorDebug::btn_run_action(lv_obj_t *btn)
{
    lv_btn_set_state(btn, LV_BTN_STATE_INA);
    run = true;
    return LV_RES_OK;
}



/**
 * looks at the string of the current drop down list option and compares it to
 * a string to see what gearset the user wants
 * sets gearset to this value
 */
lv_res_t InternalMotorDebug::ddlist_gearset_action(lv_obj_t * ddlist)
{
    //checks what the drop down list string is
    char sel_cstr[32];
    lv_ddlist_get_selected_str(ddlist, sel_cstr);

    std::string sel_str = std::string(sel_cstr);  //convert to std::string so
                                                  //that the strings can be
                                                  //compared

    //sets brake mode for motor
    if ( sel_str == "100" )
    {
        current_gearset = pros::E_MOTOR_GEARSET_36;
    }

    else if ( sel_str == "600" )
    {
        current_gearset = pros::E_MOTOR_GEARSET_06;
    }

    else
    {
        current_gearset = pros::E_MOTOR_GEARSET_18;
    }

    return LV_RES_OK; //Return OK because the drop down list was not deleted
}




/**
 * looks at the string of the current drop down list option and compares it to
 * a string to see what brakemode the user wants
 * sets brake mode to this value
 */
lv_res_t InternalMotorDebug::ddlist_brake_mode_action(lv_obj_t * ddlist)
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
 * reads values from text areas and performs data validation, exits on invalid data
 * starts unit test and logs data
 * updates labels on lcd while waiting for duration to finish
 */
int InternalMotorDebug::run_unit_test()
{
    Logger logger;
    
    pid pid_constants;
    
    int slew = 0;
    int setpoint = 0;
    int motor_port = 0;
    
    int duration = 0;
    
    //read info from text areas in exception safe way
    try
    {
        double kP = std::stod(lv_ta_get_text(kp_text_area)); 
        double kI = std::stod(lv_ta_get_text(ki_text_area));
        double kD = std::stod(lv_ta_get_text(kd_text_area));
        double I_max = std::stod(lv_ta_get_text(I_max_text_area));

        pid_constants.kP = kP;
        pid_constants.kI = kI;
        pid_constants.kD = kD;
        pid_constants.I_max = I_max;
    }
    catch ( const std::invalid_argument& )
    {
        run = false;
        
        log_entry entry;
        entry.content = "[ERROR] " + std::to_string(pros::millis()) + " invalid pid constants given to internal motor unit test";
        entry.stream = "cerr";
        logger.add(entry);
        
        return 0;
    }
    
    try
    {
        slew = std::stoi(lv_ta_get_text(slew_text_area)); 
    }
    catch ( const std::invalid_argument& )
    {
        run = false;
        
        log_entry entry;
        entry.content = "[ERROR] " + std::to_string(pros::millis()) + " invalid slew rate given to internal motor unit test";
        entry.stream = "cerr";
        logger.add(entry);
        
        return 0;
    }
    
    try
    {
        setpoint = std::stoi(lv_ta_get_text(setpoint_text_area)); 
    }
    catch ( const std::invalid_argument& )
    {
        run = false;
        
        log_entry entry;
        entry.content = "[ERROR] " + std::to_string(pros::millis()) + " invalid setpoint given to internal motor unit test";
        entry.stream = "cerr";
        logger.add(entry);
        
        return 0;
    }
    
    try
    {
        motor_port = std::stoi(lv_ta_get_text(port_text_area)); 
    }
    catch ( const std::invalid_argument& )
    {
        run = false;
        
        log_entry entry;
        entry.content = "[ERROR] " + std::to_string(pros::millis()) + " invalid motor port given to internal motor unit test";
        entry.stream = "cerr";
        logger.add(entry);
        
        return 0;
    }
    
    try
    {
        duration = std::stoi(lv_ta_get_text(duration_text_area)); 
    }
    catch ( const std::invalid_argument& )
    {
        run = false;
        
        log_entry entry;
        entry.content = "[ERROR] " + std::to_string(pros::millis()) + " invalid duration given to internal motor unit test";
        entry.stream = "cerr";
        logger.add(entry);
        
        return 0;
    }
    
    //set motor information 
    motor.set_port(motor_port);
    motor.set_gearing(current_gearset);
    motor.set_brake_mode(current_brake_mode);
    // if ( std::abs(slew) > 0 )
    // {
    // motor.enable_slew();
    // motor.set_slew(30);
    // }
    // else 
    // {
         motor.disable_slew();
    // }

    //motor.disable_velocity_pid();
    motor.disable_driver_control();
    motor.set_pid( pid_constants );
    motor.enable_velocity_pid();
    motor.set_log_level(1);
    std::cout << motor.get_pid().kP << "\n";
    
    MotorThread * motor_thread = MotorThread::get_instance();
    motor_thread->start_thread();
    
    int ut_end_time = pros::millis() + duration; 
    //motor.move_velocity(setpoint);
    motor.move_velocity(200);
    
    //wait for unit test to finish and update gui in the meantime
    while ( pros::millis() < ut_end_time )
    {
        //update gui 
        std::string info_str;
        info_str = "Voltage: " + std::to_string(motor.get_actual_voltage()) + "\n";
        info_str += "Velocity: " + std::to_string(motor.get_actual_velocity()) + "\n";
        info_str += "Error: " + std::to_string(setpoint - motor.get_actual_velocity());
        
        lv_label_set_text(information_label, info_str.c_str());
        logger.dump();
        pros::delay(50);
    }
    
    motor.set_voltage(0);
    pros::delay(2000);
    logger.dump();
    logger.dump();
    logger.dump();
    logger.dump();
    logger.dump();
    logger.dump();
    logger.dump();
    logger.dump();
    logger.dump();
    motor.set_log_level(0);

    return 1;
}




/**
 * waits for cont to be false which occurs when the user hits the back button
 */
void InternalMotorDebug::debug()
{
    std::string error_str = "-";
    cont = true;
    run = false; 
    

    lv_scr_load(main_screen);

    while ( cont )
    {
        //update information label 
        std::string info_str;
        info_str = "Voltage: " + std::to_string(motor.get_actual_voltage()) + "\n";
        info_str += "Velocity: " + std::to_string(motor.get_actual_velocity()) + "\n";
        info_str += "Error: -";
        lv_label_set_text(information_label, info_str.c_str());
        
        if ( run )
        {
            run_unit_test();
            run = false;
            lv_btn_set_state(btn_run, LV_BTN_STYLE_REL);

        }
        
        
        pros::delay(100);
    }
    
}
