/**
 * @file: ./RobotCode/src/objects/sensors/Sensors.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/29/2020
 * @reviewed_by: Aiden Carney
 *
 * @see: Sensors.hpp
 *
 * contains definitions for sensors and implementation for sensor class
 */

#include "Sensors.hpp"
#include "../../Configuration.hpp"
#include "../serial/Logger.hpp"


namespace Sensors 
{
    Encoder right_encoder{RIGHT_ENC_TOP_PORT, RIGHT_ENC_BOTTOM_PORT, false};
    Encoder left_encoder{LEFT_ENC_TOP_PORT, LEFT_ENC_BOTTOM_PORT, true};
    AnalogInSensor potentiometer{POTENTIOMETER_PORT};
    pros::ADIDigitalIn limit_switch{LIMITSWITCH_PORT};
    
    BallDetector ball_detector{DETECTOR1_PORT, VISIONSENSOR_PORT, Configuration::get_instance()->filter_threshold};
    pros::Imu imu{IMU_PORT};
    
    
    
    
    void log_data()
    {
        Logger logger;
        log_entry entry;
        entry.content = ("[INFO], " + std::to_string(pros::millis()) 
            + ", Sensor Data"
            +  ", Right_Enc: " + std::to_string(right_encoder.get_absolute_position(false))
            +  ", Left_Enc: " + std::to_string(left_encoder.get_absolute_position(false))
            +  ", Detector_1: " + std::to_string(std::get<0>(ball_detector.debug())));
        entry.stream = "clog";
        logger.add(entry);
    }

}

