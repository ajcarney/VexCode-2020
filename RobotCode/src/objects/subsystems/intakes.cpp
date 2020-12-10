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
    
    l_intake->disable_velocity_pid();
    r_intake->disable_velocity_pid();
    
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
    while(1) {
        if(command_queue.empty()) {  // delay unitl there is a command in the queue
            pros::delay(10);
            continue;
        }
        
        // take lock and get command
        while ( lock.exchange( true ) ); //aquire lock
        intake_command command = command_queue.front();
        command_queue.pop();
        lock.exchange( false ); //release lock
        
        // execute command
        int secure = false;
        while(command_queue.empty()) {  // continue with the current command until a new one is given
            switch(command) {
                case e_intake: {
                    l_intake->set_voltage(12000);
                    r_intake->set_voltage(12000);
                } case e_outtake: {
                    l_intake->set_voltage(-12000);
                    r_intake->set_voltage(-12000);
                } case e_stop: {
                    l_intake->set_voltage(0);
                    r_intake->set_voltage(0);    
                } case e_secure: {
                    if(!secure) {
                        l_intake->set_voltage(12000);
                        r_intake->set_voltage(12000);
                    } else {
                        break;
                    }
                    
                    if((l_intake->get_torque() + r_intake->get_torque()) / 2 > 1) { // wait a little bit and then say ball is secure
                        pros::delay(300);
                        secure = true;
                    }
                    
                    break;
                }
            }
        }
    }
}

void Intakes::intake() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_intake);
    lock.exchange( false ); //release lock
}

void Intakes::outtake() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_outtake);
    lock.exchange( false ); //release lock
}

void Intakes::stop() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_stop);
    lock.exchange( false ); //release lock
}

void Intakes::intake_until_secure() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_secure);
    lock.exchange( false ); //release lock
}

void Intakes::reset_queue() {
    while ( lock.exchange( true ) ); //aquire lock
    std::queue<intake_command> empty_queue;
    std::swap( command_queue, empty_queue );  // replace command queue with an empty queue
    lock.exchange( false ); //release lock    
}
