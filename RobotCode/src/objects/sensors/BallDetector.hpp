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
        AnalogInSensor ball_detector_top;
        AnalogInSensor ball_detector_filter;
        AnalogInSensor ball_detector_bottom;
        pros::Vision* vision_sensor;
        
        int time_since_last_ball;
        
        int threshold;
                        
    public:
        BallDetector(char line_sensor_port_top_port, char ball_detector_filter_port, char line_sensor_port_bottom_port, char vision_port, int detector_threshold);
        ~BallDetector();
                        
        int set_threshold(int new_threshold);
        int check_filter_level();
        std::vector<bool> locate_balls();
        
        std::tuple<int, int> debug_color();
    
};





#endif
