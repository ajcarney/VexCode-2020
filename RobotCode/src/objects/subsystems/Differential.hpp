/**
 * @file: ./RobotCode/src/objects/subsystems/Differential.hpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * Contains class for the differential subsystem
 * has methods for brake and indexing
 */

#ifndef __DIFFERENTIAL_HPP__
#define __DIFFERENTIAL_HPP__

#include <tuple>
#include <queue>

#include "main.h"

#include "../motors/Motor.hpp"
#include "../sensors/Sensors.hpp"
#include "../sensors/BallDetector.hpp"


typedef enum e_differential_command {
    e_index,
    e_filter,
    e_auto_index,
    e_raise_brake,
    e_lower_brake,
    e_stop
} differential_command;


/**
 * @see: Motors.hpp
 *
 * contains methods to allow for control of the indexer
 */
class Differential
{
    private:
        static Motor *diff1;
        static Motor *diff2;
        static BallDetector *ball_detector;
        static AnalogInSensor *potentiometer;
        static std::string filter_color;
        
        static int num_instances;
        
        pros::Task *thread;  // the motor thread
        static std::queue<differential_command> command_queue;
        static std::atomic<bool> lock;

        static void differential_motion_task(void*);
                
    public:
        Differential(Motor &differential1, Motor &differential2, BallDetector &detector, AnalogInSensor &pot, std::string color);
        ~Differential();
    
        void index();
        void filter();
        void auto_index();
        
        void lower_brake();
        void raise_brake();
        
        void stop();

        void reset_queue();
        void update_filter_color(std::string new_color);
        
};




#endif
