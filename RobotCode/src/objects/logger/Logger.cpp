/**
 * @file: ./RobotCode/src/objects/logger/Logger.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/9/2020
 * @reviewed_by: Aiden Carney
 *
 * @see Writer.hpp
 *
 * contains implementation for the writer class
 */

#include <atomic>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "main.h"

#include "Logger.hpp"

std::queue<log_entry> Logger::logger_queue;
std::atomic<bool> Logger::lock = ATOMIC_VAR_INIT(false);


Logger::Logger() { }




Logger::~Logger() { }




/**
 * fsends data on the given stream based on the log entry
 */
bool Logger::log( log_entry entry )
{
    if ( entry.stream == "cout" )
    {
        std::cout << entry.content << "\n";
    }
    else if ( entry.stream == "cerr" )
    {
        std::cerr << entry.content << "\n";
    }
    else if ( entry.stream == "clog" )
    {
        std::clog << entry.content << "\n";
    }
    else
    {
        return false;
    }
    
    return true;
}




/**
 * add item to the queue by aquiring and releasing atomic lock
 */
bool Logger::add( log_entry entry )
{
    if ( !entry.stream.empty() && !entry.content.empty() )
    {
        while ( lock.exchange( true ) ); //aquire lock
        logger_queue.push( entry );
        lock.exchange( false ); //release lock
        
        return true;
    }
    
    return false;
}




/**
 * gets an item from the queue by acquiring the lock and releasing it
 */
log_entry Logger::get_entry( )
{
    log_entry contents;

    //checks if there are items to be written, useful so that this function is
    //not blocking if the queue is empty
    if ( !logger_queue.empty() )
    {
        while ( lock.exchange( true ) ); //aquire lock
        contents = logger_queue.front();
        logger_queue.pop();
        lock.exchange( false ); //release lock because there is no more iteraction
                                //with the queue

    }
    return contents;
}




/**
 * builds up a cache of items 
 * this is used so that data can be sent at closer to the max speed
 */
bool Logger::dump( )
{
    std::vector<log_entry> entries;
    
    int start = pros::millis();
    while ( pros::millis() < (start + 50) )
    {
        log_entry entry = get_entry();

        if ( !entry.stream.empty() )
        {
            entries.push_back( entry );
        }
        else //object returned was empty meaning queue is empty
             //it would be wasteful to look at the queue when it is known to be empty
        {
            break;
        }
    }
    
    
    for ( int i = 0; i < entries.size(); i++ )
    {
        log(entries.at(i));
    }

}



/**
 * gets the size of the writer queue
 */
int Logger::get_count()
{
    return logger_queue.size();
}
