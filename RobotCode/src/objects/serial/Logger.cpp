/**
 * @file: ./RobotCode/src/objects/serial/Logger.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/9/2020
 * @reviewed_by: Aiden Carney
 *
 * @see Logger.hpp
 *
 * contains implementation for the logger class
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
bool Logger::use_queue = true;


Logger::Logger() { }




Logger::~Logger() { }




/**
 * fsends data on the given stream based on the log entry
 */
bool Logger::log( log_entry entry )
{
    if ( entry.stream == "cout" )
    {
        std::cout << pros::millis() << " " << entry.content << "\n";
    }
    else if ( entry.stream == "cerr" )
    {
        std::cerr << pros::millis() << " " << entry.content << "\n";
    }
    else if ( entry.stream == "clog" )
    {
        std::clog << pros::millis() << " " << entry.content << "\n";
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
        if(use_queue) {  // save the message in a queue to be viewed later
            while ( lock.exchange( true ) ); //aquire lock
            logger_queue.push( entry );
            lock.exchange( false ); //release lock
        } else {  // log the message right away
            log(entry);
        }
        
        return true;
    }
    
    return false;
}




/**
 * gets an item from the queue by acquiring the lock and releasing it
 */
std::vector<log_entry> Logger::get_entries(int num_entries)
{
    std::vector<log_entry> contents;

    while ( lock.exchange( true ) ); //aquire lock
    
    for(int i=0; i<num_entries; i++) {
        if ( !logger_queue.empty() ) {
            contents.push_back(logger_queue.front());
            logger_queue.pop();
        } else {
            break;
        }
    }
    
    lock.exchange( false ); //release lock because there is no more iteraction
                            //with the queue
    return contents;
}




/**
 * builds up a cache of items 
 * this is used so that data can be sent at closer to the max speed
 */
void Logger::dump( )
{
    std::vector<log_entry> entries = get_entries(50);

    for ( int i = 0; i < entries.size(); i++ )
    {
        log(entries.at(i));
    }

}


void Logger::start_queueing() {
    use_queue = true;
}

void Logger::stop_queueing() {
    use_queue = false;
}


/**
 * gets the size of the writer queue
 */
int Logger::get_count()
{
    return logger_queue.size();
}
