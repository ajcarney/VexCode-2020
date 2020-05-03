/**
 * @file: ./RobotCode/src/objects/motors/MotorThread.cpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 * TODO:
 *
 * contains implementation for functions that handle motor functions
 */

#include <vector>
#include <atomic>

#include "main.h"

#include "Motor.hpp"
#include "MotorThread.hpp"


MotorThread *MotorThread::thread_obj = NULL;
std::vector<Motor*> MotorThread::motors;
std::atomic<bool> MotorThread::lock = ATOMIC_VAR_INIT(false);


MotorThread::MotorThread() //:
    //thread( run, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "motor_thread")
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
    while ( 1 )
    {
        while ( lock.exchange( true ) );
        for ( int i = 0; i < motors.size(); i++ )
        {
            motors.at(i)->run( pros::millis() - start );
        }
        start = pros::millis();
        lock.exchange(false);
        pros::delay(10);
    }
}



/**
 * inits object if object is not already initialized based on a static bool
 * sets bool if it is not set
 */
MotorThread* MotorThread::get_instance()
{
    if ( thread_obj == NULL )
    {
        thread_obj = new MotorThread;
    }
    return thread_obj;
}


void MotorThread::start_thread()
{
    thread->resume();
}

void MotorThread::stop_thread()
{
    thread->suspend();
}


int MotorThread::register_motor( Motor &motor )
{
    while ( lock.exchange( true ) );
    
    try
    {
        motors.push_back(&motor);
        std::clog << "[INFO] " << pros::millis() << " motor added at " << &motor << "\n";
    } 
    catch ( ... )
    {
        std::cerr << "[WARNING] " << pros::millis() << " could not add motor at " << &motor << "\n";
        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);
    return 1;
}


int MotorThread::unregister_motor( Motor &motor )
{
    while ( lock.exchange( true ) );
    
    auto element = std::find(begin(motors), end(motors), &motor);
    if ( element != motors.end())
    {
        motors.erase(element);
        std::clog << "[INFO] " << pros::millis() << " motor removed at " << &motor << "\n";
    }
    else 
    {
        std::cerr << "[WARNING] " << pros::millis() << " could not remove motor at " << &motor << "\n";
        return 0;
    }
    
    lock.exchange(false);
    return 1;
    
}
