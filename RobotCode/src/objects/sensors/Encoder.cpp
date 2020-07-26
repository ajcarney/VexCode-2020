/**
 * @file: ./RobotCode/src/objects/sensors/Encoder.cpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * contains implementation for wrapper class for Encoder
 */

#include <atomic>
#include <vector>

#include "main.h"

#include "../serial/Logger.hpp"
#include "Encoder.hpp" 



Encoder::Encoder( char upper_port, char lower_port, bool reverse )
{        
    lock = ATOMIC_VAR_INIT(false);

    encoder = new pros::ADIEncoder(upper_port, lower_port, reverse);
    
    
    while ( lock.exchange( true ) ); //aquire lock
    zero_positions.push_back(encoder->get_value());
    lock.exchange(false);  //release lock
}


Encoder::~Encoder()
{
    // TODO: figure out why checking for null pointer needs to be present to not crash when program starts
    if(encoder != NULL)  // causes segfault when program begins if this is not present
    {
        delete encoder;
    }
}




int Encoder::get_unique_id()
{
    while ( lock.exchange( true ) ); //aquire lock
    
    int id = zero_positions.size();
    zero_positions.push_back(zero_positions.at(0));
    
    lock.exchange(false);  //release lock
    
    return id;
}




double Encoder::get_position(int unique_id)
{
    if(unique_id > zero_positions.size() - 1 || unique_id < 0)
    {
        Logger logger;
        log_entry entry;
        entry.content = "[ERROR], " + std::to_string(pros::millis()) + ", could not get encoder position with unique id " + std::to_string(unique_id);
        entry.stream = "cerr";
        
        logger.add(entry);
        
        return INT32_MAX;
    }
    
    double position = get_absolute_position(false) - zero_positions.at(unique_id);
    return position;
    
}



double Encoder::get_absolute_position(bool scaled)
{
    double position = encoder->get_value() - zero_positions.at(0);
    
    if(scaled)
    {
        position = ((int)position % 720) - 360;  // scales to interval [-360,360]
    }
    
    return position;
}




int Encoder::reset(int unique_id)
{
    if(unique_id > zero_positions.size() - 1 || unique_id < 0)
    {
        Logger logger;
        log_entry entry;
        entry.content = "[ERROR], " + std::to_string(pros::millis()) + ", could not get encoder position with unique id " + std::to_string(unique_id);
        entry.stream = "cerr";
        
        logger.add(entry);
        
        return 0;
    }
    
    zero_positions.at(unique_id) = encoder->get_value();
    return 1;
}
