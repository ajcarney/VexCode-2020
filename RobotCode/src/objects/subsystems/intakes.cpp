/**
 * @file: ./RobotCode/src/objects/subsystems/intakes.cpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * Contains implementation for the front intakes subsystem
 * has methods for intaking
 */

#include "main.h"


#include "../serial/Logger.hpp"
#include "intakes.hpp"

int Intakes::num_instances = 0;
std::queue<intake_command> Intakes::command_queue;
std::atomic<bool> Intakes::lock = ATOMIC_VAR_INIT(false);
Motor* Intakes::l_intake;
Motor* Intakes::r_intake;

Intakes::Intakes(Motor &left, Motor &right)
{    
    l_intake = &left;
    r_intake = &right;

    l_intake->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    r_intake->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    l_intake->set_motor_mode(e_voltage);
    r_intake->set_motor_mode(e_voltage);
    
    l_intake->disable_slew();
    r_intake->disable_slew();
    
    if(num_instances == 0 || thread == NULL) {
        thread = new pros::Task( intake_motion_task, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "intakes_thread");
    }
    
    num_instances += 1;
}


Intakes::~Intakes() {
    num_instances -= 1;
    if(num_instances == 0) {
        delete thread;
    }
}




void Intakes::intake_motion_task(void*) {
    l_intake->tare_encoder();
    r_intake->tare_encoder();
    l_intake->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    r_intake->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    int abs_position_l = 0;  // the absolute postions are calculated based on the change in encoder value
    int abs_position_r = 0;  // and capped to max and min values
    int prev_encoder_l = l_intake->get_encoder_position();
    int prev_encoder_r = r_intake->get_encoder_position();
    int integral_l = 0;
    int integral_r = 0;
    int dt = 0;
    int time = pros::millis();
    
    while(1) {
        if(command_queue.empty()) {  // delay unitl there is a command in the queue
            pros::delay(7);
            continue;
        }
        
        // take lock and get command
        while ( lock.exchange( true ) ); //aquire lock
        intake_command command = command_queue.front();
        command_queue.pop();
        lock.exchange( false ); //release lock
        
        if(command != e_hold_outward) {  // reset integral if no longer holding outwards
            integral_l = 0;
            integral_r = 0;
        }
        
        dt = pros::millis() - time;  // calculate change in time since last command
        time = pros::millis();
        
        int d_enc_l = l_intake->get_encoder_position() - prev_encoder_l;
        int d_enc_r = r_intake->get_encoder_position() - prev_encoder_r;
        prev_encoder_l = l_intake->get_encoder_position();
        prev_encoder_r = r_intake->get_encoder_position();
        abs_position_l += d_enc_l;
        abs_position_r += d_enc_r;
        
        // cap encoder values. This can be done because mechanical stops stop the motion of
        // the intakes
        if (abs_position_l > 0) {  // innermost value of the encoder
            abs_position_l = 0;
        }
        
        if (abs_position_r > 0) {  // innermost value of the encoder
            abs_position_r = 0;
        }
        // std::cout << abs_position_l << " " << l_intake->get_actual_voltage() << "\n";
        // execute command
        switch(command) {
            case e_intake: {
                l_intake->set_voltage(12000);
                r_intake->set_voltage(12000);
                break;
            } case e_stop_movement: {
                l_intake->set_voltage(0);
                r_intake->set_voltage(0);    
                break;
            } case e_secure: {
                l_intake->set_voltage(12000);
                r_intake->set_voltage(12000);
                if ((l_intake->get_torque() + r_intake->get_torque()) / 2 > 1) { // wait a little bit and then say ball is secure
                    pros::delay(300);
                    l_intake->set_voltage(0);
                    r_intake->set_voltage(0);
                }
                break;
            } case e_hold_outward: {  // PI controller to hold outwards
                // double l_error = -37 - abs_position_l;  // set first number to encoder setpoint
                // double r_error = -37 - abs_position_r;  // set first number to encoder setpoint
                // 
                // integral_l = integral_l + (l_error * dt);
                // integral_r = integral_r + (r_error * dt);
                // 
                // int voltage_l = (40 * l_error) + (1 * integral_l);  // set first number to kP, second number to kI
                // int voltage_r = (40 * r_error) + (1 * integral_r);  // set first number to kP, second number to kI
                // if(abs_position_l > -30) {
                //     l_intake->set_voltage(-5000);
                // } else {
                //     l_intake->set_voltage(-1500);  // doesn't take a lot to keep it out, so less voltage
                // }
                // 
                // if(abs_position_r > -30) {
                //     r_intake->set_voltage(-5000);
                // } else {
                //     r_intake->set_voltage(-1500);  // doesn't take a lot to keep it out, so less voltage
                // }
                // 
                l_intake->set_voltage(-4000);
                r_intake->set_voltage(-4000);
                break;
            } case e_rocket_outwards: {
                l_intake->set_voltage(-12000);
                r_intake->set_voltage(-12000);
                break;
            }
        }
        
    }
}

void Intakes::intake() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_intake);
    lock.exchange( false ); //release lock
}

void Intakes::stop() {
    reset_queue();
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_stop_movement);
    lock.exchange( false ); //release lock
}

void Intakes::intake_until_secure() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_secure);
    lock.exchange( false ); //release lock
}

void Intakes::hold_outward() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_hold_outward);
    lock.exchange( false ); //release lock
}

void Intakes::rocket_outwards() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_rocket_outwards);
    lock.exchange( false ); //release lock
}

void Intakes::reset_queue() {
    while ( lock.exchange( true ) ); //aquire lock
    std::queue<intake_command> empty_queue;
    std::swap( command_queue, empty_queue );  // replace command queue with an empty queue
    lock.exchange( false ); //release lock    
}
