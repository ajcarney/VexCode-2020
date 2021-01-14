/**
 * @file: ./RobotCode/src/objects/subsystems/Indexer.cpp
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
#include "Indexer.hpp"

int Indexer::num_instances = 0;
std::queue<indexer_command> Indexer::command_queue;
std::atomic<bool> Indexer::lock = ATOMIC_VAR_INIT(false);
Motor* Indexer::upper_indexer;
Motor* Indexer::lower_indexer;
BallDetector* Indexer::ball_detector;
AnalogInSensor* Indexer::potentiometer;
std::string Indexer::filter_color;

Indexer::Indexer(Motor &upper, Motor &lower, BallDetector &detector, AnalogInSensor &pot, std::string color)
{    
    upper_indexer = &upper;
    lower_indexer = &lower;
    ball_detector = &detector;
    potentiometer = &pot;
    filter_color = color;

    upper_indexer->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    lower_indexer->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    upper_indexer->set_motor_mode(e_voltage);
    lower_indexer->set_motor_mode(e_voltage);
    
    upper_indexer->disable_slew();
    lower_indexer->disable_slew();
    
    if(num_instances == 0 || thread == NULL) {
        thread = new pros::Task( indexer_motion_task, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "indexer_thread");
    }
    
    num_instances += 1;
}


Indexer::~Indexer() {
    num_instances -= 1;
    if(num_instances == 0) {
        delete thread;
    }
}




void Indexer::indexer_motion_task(void*) {
    while(1) {
        if(command_queue.empty()) {  // delay unitl there is a command in the queue
            pros::delay(7);
            continue;
        }
        
        // take lock and get command
        while ( lock.exchange( true ) ); //aquire lock
        indexer_command command = command_queue.front();
        command_queue.pop();
        lock.exchange( false ); //release lock
        
        // execute command
        switch(command) {
            case e_filter: {
                upper_indexer->set_voltage(-12000); 
                lower_indexer->set_voltage(12000);
                break;
            } case e_auto_index: {
                int color = ball_detector->check_filter_level();
                if((color == 1 && filter_color == "blue") || (color == 2 && filter_color == "red")) {  // ball should be filtered
                    upper_indexer->set_voltage(-12000); 
                    lower_indexer->set_voltage(12000);
                    pros::delay(300);  // let ball filter out
                    upper_indexer->set_voltage(0); 
                    lower_indexer->set_voltage(0);
                    break;
                } else if(color < 0) {  // ball was detected but color could not be determined: print error message and default to intaking
                    Logger logger;
                    log_entry entry;
                    entry.content = "[ERROR], " + std::to_string(pros::millis()) + ", ball was detected but color could not be determined";
                    entry.stream = "cerr";
                    logger.add(entry);
                }          
                // fallthrough and index like normal now that it doesn't need to filter
            } case e_index: {
                upper_indexer->set_voltage(12000); 
                lower_indexer->set_voltage(12000);
                break;
            } case e_index_no_backboard: {
                upper_indexer->set_voltage(12000); 
                lower_indexer->set_voltage(6500);   
                break;
            } case e_auto_increment: {
                // try to filter out ball at second level if necessary
                int color = ball_detector->check_filter_level();
                if((color == 1 && filter_color == "blue") || (color == 2 && filter_color == "red")) {  // ball should be filtered
                    upper_indexer->set_voltage(-12000); 
                    lower_indexer->set_voltage(12000);
                    pros::delay(300);  // let ball filter out
                    upper_indexer->set_voltage(0); 
                    lower_indexer->set_voltage(0);
                    break;
                } else if(color < 0) {  // ball was detected but color could not be determined: print error message and default to intaking
                    Logger logger;
                    log_entry entry;
                    entry.content = "[ERROR], " + std::to_string(pros::millis()) + ", ball was detected but color could not be determined";
                    entry.stream = "cerr";
                    logger.add(entry);
                }
                // fall through if there is nothing to filter out
            } case e_increment: {
                std::vector<bool> locations = ball_detector->locate_balls();
                
                if(!locations.at(0)) {  // move ball into top position
                    upper_indexer->set_voltage(4500); 
                    lower_indexer->set_voltage(12000);
                } else if(locations.at(0) && !locations.at(1)) { // move ball from lowest/no position to middle position
                    lower_indexer->set_voltage(12000);
                } else { // indexer can't do anything to increment so don't run
                    upper_indexer->set_voltage(0); 
                    lower_indexer->set_voltage(0);
                }
                break;
            } case e_raise_brake: {
                lower_indexer->set_voltage(-12000);

                int dt = 0;
                while(potentiometer->get_raw_value() < 3950 && dt < 3000) {  // top value of potentiometer
                    pros::delay(10);
                    dt += 10;
                }
                
                lower_indexer->set_voltage(0);
                break;
            } case e_lower_brake: {
                lower_indexer->set_voltage(-12000);
                
                int dt = 0;
                while(potentiometer->get_raw_value() > 3470 && dt < 3000) {  // lowered position of potentiometer
                    pros::delay(10);
                    dt += 10;
                }
                lower_indexer->set_voltage(0);
                break;
            } case e_fix_ball: {
                upper_indexer->set_voltage(-12000);
                pros::delay(250);
                upper_indexer->set_voltage(12000);
                pros::delay(500);
                upper_indexer->set_voltage(0);
                break;
            } case e_run_upper: {
                upper_indexer->set_voltage(12000);
                break;
            } case e_run_lower: {
                lower_indexer->set_voltage(12000);
                break;
            } case e_stop: {
                lower_indexer->set_voltage(0);
                upper_indexer->set_voltage(0);
                break;
            }
        }
    }
}

void Indexer::index() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_index);
    lock.exchange( false ); //release lock
}

void Indexer::filter() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_filter);
    lock.exchange( false ); //release lock
}

void Indexer::auto_index() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_auto_index);
    lock.exchange( false ); //release lock
}

void Indexer::index_no_backboard() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_index_no_backboard);
    lock.exchange( false ); //release lock
}


void Indexer::increment() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_increment);
    lock.exchange( false ); //release lock
}

void Indexer::auto_increment() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_auto_increment);
    lock.exchange( false ); //release lock
}



void Indexer::lower_brake() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_lower_brake);
    lock.exchange( false ); //release lock
}

void Indexer::raise_brake() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_raise_brake);
    lock.exchange( false ); //release lock    
}



void Indexer::run_upper_roller() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_run_upper);
    lock.exchange( false ); //release lock   
}


void Indexer::run_lower_roller() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_run_lower);
    lock.exchange( false ); //release lock   
}



void Indexer::fix_ball() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_fix_ball);
    lock.exchange( false ); //release lock   
}



void Indexer::stop() {
    reset_queue();
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_stop);
    lock.exchange( false ); //release lock
}



void Indexer::reset_queue() {
    while ( lock.exchange( true ) ); //aquire lock
    std::queue<indexer_command> empty_queue;
    std::swap( command_queue, empty_queue );  // replace command queue with an empty queue
    lock.exchange( false ); //release lock    
}


void Indexer::update_filter_color(std::string new_color) {
    while ( lock.exchange( true ) ); //aquire lock
    filter_color = new_color;
    lock.exchange( false ); //release lock
}
