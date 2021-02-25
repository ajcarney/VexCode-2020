/**
 * @file: ./RobotCode/src/objects/sensors/RGBLed.cpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * contains implementation for class to control rgb led light string
 */

#include "../serial/Logger.hpp"
#include "RGBLed.hpp" 


RGBLedString::RGBLedString() {
    red = NULL;
    green = NULL;
    blue = NULL;
}


int RGBLedString::get_pwm(int value) {
    int old_min = 0;
    int new_min = 0;
    int old_max = 255;
    int new_max = 127;
    int pwm = (((value - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min;
    
    return pwm;
}


RGBLedString::RGBLedString(char red_port, char green_port, char blue_port) {
    red = new pros::ADIMotor(red_port);
    green = new pros::ADIMotor(green_port);
    blue = new pros::ADIMotor(blue_port);
}


RGBLedString::RGBLedString(pros::ext_adi_port_pair_t red_port, pros::ext_adi_port_pair_t green_port, pros::ext_adi_port_pair_t blue_port) {
    red = new pros::ADIMotor(red_port);
    green = new pros::ADIMotor(green_port);
    blue = new pros::ADIMotor(blue_port);
}


RGBLedString::~RGBLedString() {
    if(red != NULL) {
        delete red;
    }
    if(green != NULL) {
        delete green;
    }
    if(blue != NULL) {
        delete blue;
    }
}

void RGBLedString::set_port(char red_port, char green_port, char blue_port) {
    if(red != NULL) {
        delete red;
    }
    if(green != NULL) {
        delete green;
    }
    if(blue != NULL) {
        delete blue;
    }
    
    red = new pros::ADIMotor(red_port);
    green = new pros::ADIMotor(green_port);
    blue = new pros::ADIMotor(blue_port); 
}


void RGBLedString::set_port(pros::ext_adi_port_pair_t red_port, pros::ext_adi_port_pair_t green_port, pros::ext_adi_port_pair_t blue_port) {
    if(red != NULL) {
        delete red;
    }
    if(green != NULL) {
        delete green;
    }
    if(blue != NULL) {
        delete blue;
    }
    
    red = new pros::ADIMotor(red_port);
    green = new pros::ADIMotor(green_port);
    blue = new pros::ADIMotor(blue_port); 
}


std::array<int, 3> RGBLedString::get_value() {
    return {red->get_value(), green->get_value(), blue->get_value()};
}


void RGBLedString::set_color(int r, int g, int b) {
    red->set_value(get_pwm(r));
    green->set_value(get_pwm(g));
    blue->set_value(get_pwm(b));
}

void RGBLedString::set_color(int hex_code) {
    int r = (hex_code & 0xFF0000) >> 16;
    int g = (hex_code & 0x00FF00) >> 8;
    int b = (hex_code & 0x0000FF);
    
    red->set_value(get_pwm(r));
    green->set_value(get_pwm(g));
    blue->set_value(get_pwm(b));
}

void RGBLedString::turn_off() {
    red->stop();
    green->stop();
    blue->stop();
}
