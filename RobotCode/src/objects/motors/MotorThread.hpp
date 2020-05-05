/**
 * @file: ./RobotCode/src/objects/motors/MotorThread.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 * TODO:
 *
 * contains functions that handle motor functions
 */
 
#ifndef __MOTORTHREAD_HPP__
#define __MOTORTHREAD_HPP__

#include <vector>
#include <atomic>

#include "main.h"

#include "../../Configuration.hpp"
#include "Motor.hpp"


/**
 * @see: Motor.hpp
 *
 * contains singleton class for using motors in a thread
 * motors are added to a vector and iterated over in a thread so that the voltage
 * can be set
 */
class MotorThread
{
    private:
        MotorThread();
        static MotorThread *thread_obj;
        
        static std::vector<Motor*> motors;
        static std::atomic<bool> lock;  //protect vector from concurrent access
        
        
        /**
         * @param: void* -> not used, but necessary to follow thread making constructor
         * @return: None
         *
         * the function to be run on a thread that calls the run function for 
         * each motor that sets the voltage and performs logging
         */
        static void run(void*);
        
        pros::Task *thread;  // the motor thread
                
        
    public:
        ~MotorThread();
        
        /**
         * @return: MotorThread -> instance of class to be used throughout program
         *
         * give the instance of the singleton class or creates it if it does
         * not yet exist
         */
        static MotorThread* get_instance();
    
    
    
    
        /**
         * @return: None
         *
         * starts the thread or resmes it if it was stopped
         */
        void start_thread();
        
        /**
         * @return: None
         *
         * stops the thread from being scheduled
         */
        void stop_thread();
                
                
                
                
        /**
         * @param: Motor &motor -> the motor to add to the vector
         * @return: int -> 1 if motor was successfully added, 0 otherwise
         *
         * adds a motor to the vector of motors to operate
         * logs that the motor was added to the logger queue
         */
        int register_motor( Motor &motor );
        
        /**
         * @param: Motor &motor -> the motor to remove from the vector
         * @return: int -> 1 if motor was successfully added, 0 otherwise
         *
         * removes a motor from the vector of motors to operate
         * logs that the motor was removed to the logger queue
         */
        int unregister_motor( Motor &motor );
    
    
};

#endif 
