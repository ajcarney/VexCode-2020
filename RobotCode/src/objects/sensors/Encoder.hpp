/**
 * @file: ./RobotCode/src/objects/sensors/Encoder.hpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * contains a wrapper class for the encoders
 */

#ifndef __ENCODER_HPP__
#define __ENCODER_HPP__

#include <atomic>
#include <vector>

#include "main.h"


class Encoder 
{
    private:
        pros::ADIEncoder *encoder;
        
        std::atomic<bool> lock;  //protect vector from concurrent access
        std::vector<double> zero_positions;
        
    public:
        Encoder(char upper_port, char lower_port, bool reverse);
        ~Encoder();
        
        int get_unique_id();
        
        double get_position(int unique_id);
        double get_absolute_position(bool scaled);
        
        int reset(int unique_id);
    
};





#endif
