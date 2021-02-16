/**
 * @file: ./RobotCode/src/objects/sensors/BallDetector.hpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * contains a wrapper class for the encoders
 */

#ifndef __BALLDETECTOR_HPP__
#define __BALLDETECTOR_HPP__

#include <tuple>

#include "main.h"

#include "AnalogInSensor.hpp"


class BallDetector 
{
    private:        
        int time_since_last_ball;
        bool log_data;
        
        int threshold;
                        
    public:
        BallDetector(
            AnalogInSensor& detector_top_left, 
            AnalogInSensor& detector_filter, 
            AnalogInSensor& detector_bottom, 
            int optical_port, 
            int detector_threshold
        );
        ~BallDetector();
        
        AnalogInSensor ball_detector_top;
        AnalogInSensor ball_detector_filter;
        AnalogInSensor ball_detector_bottom;
        pros::Optical* optical_sensor;
                        
        int set_threshold(int new_threshold);
        int check_filter_level();
        std::vector<bool> locate_balls();
        
        void set_led_brightness(int pct);
        void auto_set_led_brightness();
        
        std::tuple<int, int> debug_color();
        void start_logging();
        void stop_logging();
    
};





#endif
