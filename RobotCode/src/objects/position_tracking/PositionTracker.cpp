/**
 * @file: ./RobotCode/src/objects/position_tracking/PositionTracker.cpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 * TODO:
 *
 * contains implementation for functions that track position
 */

#include <atomic>

#include "main.h"

#include "../serial/Logger.hpp"
#include "../sensors/Sensors.hpp"
#include "PositionTracker.hpp"


PositionTracker *PositionTracker::tracker_obj = NULL;
std::atomic<bool> PositionTracker::lock = ATOMIC_VAR_INIT(false);
position PositionTracker::current_position;
position PositionTracker::previous_position;
bool PositionTracker::log_data = false;



PositionTracker::PositionTracker()
{
    thread = new pros::Task( calc_position, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "position_tracking");
    thread->suspend();
}


PositionTracker::~PositionTracker()
{
    thread->remove();
    delete thread;
}


/**
 * inits object if object is not already initialized based on a static bool
 * sets bool if it is not set
 */
PositionTracker* PositionTracker::get_instance()
{
    if ( tracker_obj == NULL )
    {
        tracker_obj = new PositionTracker;
    }
    return tracker_obj;
}




double PositionTracker::to_inches( double encoder_ticks, double wheel_size )
{
    double circumference = (wheel_size * 3.14159265358);
    double revolutions = encoder_ticks / 360.0;
    double inches = circumference * revolutions;
    
    return inches;
}




void PositionTracker::calc_position(void*)
{
    Logger logger;
    log_entry entry;
    
    while(1)
    {
        
        
        if(log_data)
        {
            entry.content = ("[INFO], " + std::to_string(pros::millis()) 
                + ", Position Tracking Data"
                +  ", X_POS: " + std::to_string(current_position.x_pos)
                +  ", Y_POS: " + std::to_string(current_position.y_pos)
                +  ", Angle: " + std::to_string(current_position.angle));
            entry.stream = "clog";
            logger.add(entry);
        }
        
        pros::delay(10);
    }
}




void PositionTracker::start_thread()
{
    thread->resume();
}

void PositionTracker::stop_thread()
{
    thread->suspend();
}




void PositionTracker::start_logging()
{
    log_data = true;
}




void PositionTracker::stop_logging()
{
    log_data = false;
}




position PositionTracker::get_position()
{
    return current_position;
}




void PositionTracker::set_position(position robot_coordinates)
{
    while ( lock.exchange( true ) );
    
    current_position = robot_coordinates;
    
    lock.exchange(false);
}
