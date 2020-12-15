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


#define WHEEL_TRACK_R 6.35
#define WHEEL_TRACK_L 6.35
#define S_ENC_OFFSET 3

typedef struct
{
    long double x_pos = 0;
    long double y_pos = 0;
    long double theta = 0;
    void print() {
        std::cout << "x pos: " << this->x_pos << "\n";
        std::cout << "y pos: " << this->y_pos << "\n";
        std::cout << "angle: " << this->theta << "\n";
    };
} position;



class PositionTracker 
{
    private:
        PositionTracker();
        static PositionTracker *tracker_obj;
        
        static position current_position;

        static long double initial_l_enc;
        static long double initial_r_enc; 
        static long double initial_theta;
        static long double imu_offset;
        
        static long double prev_l_enc;
        static long double prev_r_enc;
        static long double delta_theta_rad;
        
        static int l_id;
        static int r_id;
                
        static std::atomic<bool> lock;  //protect position from concurrent access
        
        static bool log_data;
        
        
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
        
        static long double to_inches( long double encoder_ticks, long double wheel_size );
        static long double to_encoder_ticks(long double inches, long double wheel_size);
        static long double to_radians(long double degrees);
        static long double to_degrees(long double radians);
        
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
        
        long double get_delta_theta_rad();
        long double get_heading_rad();
        
        position get_position();
        
        static void set_position(position robot_coordinates);
};



#endif
