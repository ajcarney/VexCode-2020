/**
 * @file: ./RobotCode/src/objects/motors/MotorThread.hpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
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


// singleton class with thread for running motors
class MotorThread
{
    private:
        MotorThread();
        static MotorThread *thread_obj;
        
        static std::vector<Motor*> motors;
        static std::atomic<bool> lock;  //protect vector from concurrent access
        
        static void run(void*);
        
        pros::Task *thread;
                
        
    public:
        ~MotorThread();
        
        /**
         * @return: MotorThread -> instance of class to be used throughout program
         *
         * give user the instance of the singleton class or creates it if it does
         * not yet exist
         */
        static MotorThread* get_instance();
    
        void start_thread();
        void stop_thread();
                
        int register_motor( Motor &motor );
        int unregister_motor( Motor &motor );
    
    
};

#endif 
