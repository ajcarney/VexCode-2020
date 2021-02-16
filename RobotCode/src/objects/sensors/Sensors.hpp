/**
 * @file: ./RobotCode/src/objects/sensors/Sensors.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/29/2020
 * @reviewed_by: Aiden Carney
 *
 * contains a class for interacting with the ADI sensors on the robot
 */

#ifndef __SENSORS_HPP__
#define __SENSORS_HPP__

#include "main.h"

#include "BallDetector.hpp"
#include "Encoder.hpp"
#include "AnalogInSensor.hpp"
#include "RGBLed.hpp"



namespace Sensors 
{
    extern Encoder right_encoder;
    extern Encoder left_encoder;
    extern Encoder strafe_encoder;
    
    extern AnalogInSensor line_tracker_top;
    extern AnalogInSensor line_tracker_middle;
    extern AnalogInSensor line_tracker_bottom;
    extern pros::Optical optical;
    extern BallDetector ball_detector;
    
    extern pros::Imu imu;
    extern bool imu_is_calibrated;
    
    extern RGBLedString rgb_leds;
    
    void calibrate_imu();
    void log_data();
    std::tuple<double, double> get_average_encoders(int l_id, int r_id);
}




#endif
