/**
 * @file: ./RobotCode/src/objects/sensors/AnalogInSensor.hpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * contains a wrapper class for ADI analog in sensor
 */

#ifndef __ANALOGINSENSOR_HPP__
#define __ANALOGINSENSOR_HPP__

#include <atomic>
#include <vector>

#include "main.h"


class AnalogInSensor 
{
    private:
        pros::ADIAnalogIn *sensor;
        bool calibrated;
        
    public:
        AnalogInSensor();
        AnalogInSensor(char port);
        AnalogInSensor(pros::ext_adi_port_pair_t port_pair);
        ~AnalogInSensor();
        
        void set_port(char port);
        void set_port(pros::ext_adi_port_pair_t port_pair);
        
        double get_raw_value();
        double get_value(bool high_res);
        
        void calibrate();
        bool is_calibrated();
};





#endif
