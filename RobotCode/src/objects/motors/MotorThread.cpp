/**
 * @file: ./RobotCode/src/objects/motors/MotorThread.cpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 *
 * contains implementation for functions that handle motor functions
 */

#include <atomic>
#include <stdio.h>
#include <vector>

#include "main.h"

#include "../serial/Logger.hpp"
#include "Motor.hpp"
#include "MotorThread.hpp"


MotorThread *MotorThread::thread_obj = NULL;
std::vector<Motor*> MotorThread::motors;
std::atomic<bool> MotorThread::lock = ATOMIC_VAR_INIT(false);


MotorThread::MotorThread()
{
    thread = new pros::Task( run, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "motor_thread");
    thread->suspend();
}


MotorThread::~MotorThread()
{
    thread->remove();
    delete thread;
}


void MotorThread::run(void*)
{
    int start = pros::millis();
    while (1) {
        while ( lock.exchange( true ) );
        for ( int i = 0; i < motors.size(); i++ ) {
            motors.at(i)->run( pros::millis() - start );
        }
        start = pros::millis();
        lock.exchange(false);
        pros::delay(5);
    }
}



/**
 * inits object if object is not already initialized based on a static bool
 * sets bool if it is not set
 */
MotorThread* MotorThread::get_instance() {
    if ( thread_obj == NULL ) {
        thread_obj = new MotorThread;
    }
    return thread_obj;
}


void MotorThread::start_thread() {
    thread->resume();
}

void MotorThread::stop_thread() {
    thread->suspend();
}


int MotorThread::register_motor( Motor &motor ) {
    while ( lock.exchange( true ) );
    
    Logger logger;
    log_entry entry;
    char buffer[10];
    
    
    try
    {
        motors.push_back(&motor);
        
        sprintf(buffer, "%p", &motor);
        entry.stream = "clog";
        entry.content = "[INFO], " + std::to_string(pros::millis()) +  ", motor added at " + buffer;
        logger.add(entry);
    } 
    catch ( ... )
    {
        sprintf(buffer, "%p", &motor);
        entry.content = "[WARNING], " + std::to_string(pros::millis()) +  ", could not add motor at " + buffer;
        entry.stream = "cerr";
        logger.add(entry);

        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);
    return 1;
}


int MotorThread::unregister_motor( Motor &motor )
{
    while ( lock.exchange( true ) );
    
    Logger logger;
    log_entry entry;
    char buffer[10];
    
    auto element = std::find(begin(motors), end(motors), &motor);
    if ( element != motors.end())
    {
        motors.erase(element);
        
        sprintf(buffer, "%p", &motor);
        entry.stream = "clog";
        entry.content = "[INFO] " + std::to_string(pros::millis()) + ", motor removed at " + buffer;
        logger.add(entry);
    }
    else 
    {
        sprintf(buffer, "%p", &motor);
        entry.content = "[WARNING] " + std::to_string(pros::millis()) +  ", could not remove motor at " + buffer;
        entry.stream = "cerr";
        logger.add(entry);
        
        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);
    return 1;
    
}


int MotorThread::is_registered(Motor &motor) {
    int registered = 0;
    
    while ( lock.exchange( true ) );
    
    auto element = std::find(begin(motors), end(motors), &motor);
    if ( element != motors.end()) {
        registered = 1;
    }
    
    lock.exchange(false);
    
    return registered;
}