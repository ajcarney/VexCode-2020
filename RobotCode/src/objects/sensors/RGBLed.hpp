/**
 * @file: ./RobotCode/src/objects/sensors/RGBLed.hpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * contains a class for working with an rgb led light string
 */

#ifndef __RGBLED_HPP__
#define __RGBLED_HPP__

#include <atomic>
#include <vector>

#include "main.h"


class RGBLedString
{
    private:
        pros::ADIMotor *red;   // use motors because that is how they are wired
        pros::ADIMotor *green;
        pros::ADIMotor *blue;
        bool i_wired_this_wrong;
        
        int get_pwm(int value);
        
    public:
        RGBLedString();
        RGBLedString(char red_port, char green_port, char blue_port);
        RGBLedString(pros::ext_adi_port_pair_t red_port, pros::ext_adi_port_pair_t green_port, pros::ext_adi_port_pair_t blue_port);
        ~RGBLedString();
        
        void set_port(char red_port, char green_port, char blue_port);
        void set_port(pros::ext_adi_port_pair_t red_port, pros::ext_adi_port_pair_t green_port, pros::ext_adi_port_pair_t blue_port);
        
        std::array<int, 3> get_value();
        
        void set_color(int r, int g, int b);
        void turn_off();
};





#endif
