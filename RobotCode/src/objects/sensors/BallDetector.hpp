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
        AnalogInSensor ball_detector;
        pros::Vision* vision_sensor;
        
        int time_since_last_ball;
        
        int threshold;
                        
    public:
        BallDetector(char line_sensor_port, char vision_port, int threshold);
        ~BallDetector();
                        
        int set_threshold(int new_threshold);
        int check_for_ball();
        
        std::tuple<int, int> debug();
    
};





#endif
