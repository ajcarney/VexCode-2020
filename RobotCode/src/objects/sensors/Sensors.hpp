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



namespace Sensors 
{
    extern Encoder right_encoder;
    extern Encoder left_encoder;
    extern Encoder strafe_encoder;
    extern AnalogInSensor potentiometer;
    
    extern BallDetector ball_detector;
    extern pros::Imu imu;
    
    void log_data();
}




#endif
