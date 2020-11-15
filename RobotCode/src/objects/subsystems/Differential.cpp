/**
 * @file: ./RobotCode/src/objects/subsystems/Differential.cpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * Contains implementation for the differential subsystem
 * has methods for brake and indexing
 */

#include "main.h"


#include "../serial/Logger.hpp"
#include "../sensors/BallDetector.hpp"
#include "Differential.hpp"

int Differential::num_instances = 0;
std::queue<differential_command> Differential::command_queue;
std::atomic<bool> Differential::lock = ATOMIC_VAR_INIT(false);
Motor* Differential::diff1;
Motor* Differential::diff2;
BallDetector* Differential::ball_detector;
std::string Differential::filter_color;

Differential::Differential(Motor &differential1, Motor &differential2, BallDetector &detector, std::string color)
{    
    diff1 = &differential1;
    diff2 = &differential2;
    ball_detector = &detector;
    filter_color = color;

    diff1->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    diff2->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    
    diff1->disable_velocity_pid();
    diff2->disable_velocity_pid();
    
    diff1->disable_slew();
    diff2->disable_slew();
    
    if(num_instances == 0 || thread == NULL) {
        thread = new pros::Task( differential_motion_task, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "differential_thread");
    }
    
    num_instances += 1;
}


Differential::~Differential() {
    num_instances -= 1;
    if(num_instances == 0) {
        delete thread;
    }
}




void Differential::differential_motion_task(void*) {
    while(1) {
        if(command_queue.empty()) {  // delay unitl there is a command in the queue
            pros::delay(10);
            continue;
        }
        
        // take lock and get command
        while ( lock.exchange( true ) ); //aquire lock
        differential_command command = command_queue.front();
        command_queue.pop();
        lock.exchange( false ); //release lock
        
        // execute command
        switch(command) {
            case e_index: {
                diff1->set_voltage(-12000); 
                diff2->set_voltage(12000);
                break;
            } case e_filter: {
                diff1->set_voltage(12000); 
                diff2->set_voltage(-12000);
                break;
            } case e_auto_index: {
                int color = ball_detector->check_for_ball();
                if((color == 1 && filter_color == "blue") || (color == 2 && filter_color == "red")) {  // ball should be filtered
                    diff1->set_voltage(12000); 
                    diff2->set_voltage(-12000);
                    pros::delay(500);
                    diff1->set_voltage(0); 
                    diff2->set_voltage(0);
                    break;
                } else if(color < 0) {  // ball was detected but color could not be determined: print error message and default to intaking
                    Logger logger;
                    log_entry entry;
                    entry.content = "[ERROR], " + std::to_string(pros::millis()) + ", ball was detected but color could not be determined";
                    entry.stream = "cerr";
                    logger.add(entry);
                }
                
                diff1->set_voltage(-12000); 
                diff2->set_voltage(12000);                        
                    
                break;
            } case e_raise_brake: {
                diff1->set_voltage(-12000);  // lower brake to reset position
                diff2->set_voltage(-12000);
                pros::delay(300);
                
                diff1->set_voltage(12000); // raise brake to top position
                diff2->set_voltage(12000);
                pros::delay(500);
                
                int voltage = 12000;  // ramp down voltage to 0 so brake hopefully does not slip down
                while(voltage >= 0) {
                    diff1->set_voltage(voltage);
                    diff2->set_voltage(voltage);
                    voltage -= 240;
                    pros::delay(10);
                }
                
                diff1->set_voltage(0);
                diff2->set_voltage(0);
                break;
            } case e_lower_brake: {
                diff1->set_voltage(-2000);
                diff2->set_voltage(-2000);
                
                pros::delay(300);
                
                diff1->set_voltage(0);
                diff2->set_voltage(0);
                break;
            } case e_stop: {
                diff1->set_voltage(0);
                diff2->set_voltage(0);
                break;
            }
        }
    }
}

void Differential::index() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_index);
    lock.exchange( false ); //release lock
}

void Differential::filter() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_filter);
    lock.exchange( false ); //release lock
}

void Differential::auto_index() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_auto_index);
    lock.exchange( false ); //release lock
}

void Differential::lower_brake() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_lower_brake);
    lock.exchange( false ); //release lock
}
void Differential::raise_brake() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_raise_brake);
    lock.exchange( false ); //release lock    
}

void Differential::stop() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_stop);
    lock.exchange( false ); //release lock
}


void Differential::reset_queue() {
    while ( lock.exchange( true ) ); //aquire lock
    std::queue<differential_command> empty_queue;
    std::swap( command_queue, empty_queue );  // replace command queue with an empty queue
    lock.exchange( false ); //release lock    
}

void Differential::update_filter_color(std::string new_color) {
    while ( lock.exchange( true ) ); //aquire lock
    filter_color = new_color;
    lock.exchange( false ); //release lock
}
