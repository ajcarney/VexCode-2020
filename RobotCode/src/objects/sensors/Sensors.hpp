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

#include "Encoder.hpp"
#include "AnalogInSensor.hpp"



namespace Sensors 
{
    extern Encoder right_encoder;
    extern Encoder left_encoder;
    extern Encoder strafe_encoder;
    extern AnalogInSensor potentiometer;
    extern pros::ADIDigitalIn limit_switch;
    extern pros::Vision vision_sensor;
    extern pros::Imu imu;
    
    void log_data();
    
    /**
     * @return: int -> a number representing the color cube loaded
     *
     * @see: pros docs
     *
     * 0 = no cube, 1 = green, 2 = orange, 3 = purple
     * checks what color cube is loaded if any
     * also sets color of led on vision sensor to the color of the cube loaded
     */
    int check_for_cube();
}




#endif
