/**
 * @file: ./RobotCode/src/objects/subsystems/Indexer.hpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * Contains class for the differential subsystem
 * has methods for brake and indexing
 */

#ifndef __INDEXER_HPP__
#define __INDEXER_HPP__

#include <tuple>
#include <queue>

#include "main.h"

#include "../motors/Motor.hpp"
#include "../sensors/Sensors.hpp"
#include "../sensors/BallDetector.hpp"


typedef enum e_indexer_command {
    e_index,
    e_filter,
    e_auto_index,
    e_index_no_backboard,
    e_increment,
    e_auto_increment,
    e_raise_brake,
    e_lower_brake,
    e_fix_ball,
    e_run_upper,
    e_run_lower,
    e_stop
} indexer_command;


/**
 * @see: Motors.hpp
 *
 * contains methods to allow for control of the indexer
 */
class Indexer
{
    private:
        static Motor *upper_indexer;
        static Motor *lower_indexer;
        static BallDetector *ball_detector;
        static AnalogInSensor *potentiometer;
        static std::string filter_color;
        
        static int num_instances;
        
        pros::Task *thread;  // the motor thread
        static std::queue<indexer_command> command_queue;
        static std::atomic<bool> lock;

        static void indexer_motion_task(void*);
                
    public:
        Indexer(Motor &upper, Motor &lower, BallDetector &detector, AnalogInSensor &pot, std::string color);
        ~Indexer();
    
        void index();
        void filter();
        void auto_index();
        void index_no_backboard();
        
        void increment();
        void auto_increment();
        
        void lower_brake();
        void raise_brake();
        
        void run_upper_roller();
        void run_lower_roller();
        
        void fix_ball();
        
        void stop();

        void reset_queue();
        void update_filter_color(std::string new_color);
        
};




#endif
