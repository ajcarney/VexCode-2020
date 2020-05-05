/**
 * @file: ./RobotCode/src/objects/sensors/AnalogInSensor.cpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * contains implementation for wrapper class for analog in sensor
 */

#include "../logger/Logger.hpp"
#include "AnalogInSensor.hpp" 



AnalogInSensor::AnalogInSensor(char port)
{        
    sensor = new pros::ADIAnalogIn(port);
}


AnalogInSensor::~AnalogInSensor()
{
    delete sensor;
}




double AnalogInSensor::get_raw_value()
{
    double value = sensor->get_value();
    return value;
    
}




double AnalogInSensor::get_value(bool high_res)
{
    if(!calibrated)
    {
        Logger logger;
        log_entry entry;
        entry.content = "[ERROR] " + std::to_string(pros::millis()) + "could not read analog sensor (not calibrated) ";
        entry.stream = "cerr";
        
        logger.add(entry);
        
        return INT32_MAX;
    }
    
    if(high_res)
    {
        return sensor->get_value_calibrated_HR();
    }
    else 
    {
        return sensor->get_value_calibrated();
    }
}




void AnalogInSensor::calibrate()
{
    sensor->calibrate();
    calibrated = true;
    
}




bool AnalogInSensor::is_calibrated()
{
    return calibrated;
}
