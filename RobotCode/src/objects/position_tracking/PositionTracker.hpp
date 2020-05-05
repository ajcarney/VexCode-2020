/**
 * @file: ./RobotCode/src/objects/position_tracking/PositionTracker.hpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 *
 * contains functions for calculating robot position
 *
 */

#ifndef __POSITIONTRACKER_HPP__
#define __POSITIONTRACKER_HPP__

#include <atomic>

#include "main.h"


typedef struct
{
    double x_pos = 0;
    double y_pos = 0;
    double angle = 0;
    void print() {
        std::cout << "x pos: " << this->x_pos << "\n";
        std::cout << "y pos: " << this->y_pos << "\n";
        std::cout << "angle: " << this->angle << "\n";
    };
} position;



class PositionTracker 
{
    private:
        PositionTracker();
        static PositionTracker *tracker_obj;
        
        static position current_position;
        static position previous_position;
        static std::atomic<bool> lock;  //protect position from concurrent access
        
        static bool log_data;
        
        static double to_inches( double encoder_ticks, double wheel_size );
        
        static void calc_position(void*);
        pros::Task *thread;  // the thread for keeping track of position
        
    
    public:
        ~PositionTracker();
        
        /**
         * @return: PositionTracker -> instance of class to be used throughout program
         *
         * give the instance of the singleton class or creates it if it does
         * not yet exist
         */
        static PositionTracker* get_instance();
        
        
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
        
        void start_logging();
        void stop_logging();
        
        
        position get_position();
        void set_position(position robot_coordinates);
};



#endif
