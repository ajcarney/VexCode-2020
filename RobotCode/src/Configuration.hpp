/**
 * @file: ./RobotCode/src/Configuration.hpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 *
 * contains class static variables for runtime configuration
 *
 */

#ifndef __CONFIGURATION_HPP__
#define __CONFIGURATION_HPP__

#include <iostream>
#include <vector>

#include "main.h"

#include "../lib/json.hpp"


#define RIGHT_ENC_TOP_PORT       'A'
#define RIGHT_ENC_BOTTOM_PORT    'B'
#define LEFT_ENC_TOP_PORT        'C'
#define LEFT_ENC_BOTTOM_PORT     'D'
#define STRAFE_ENC_TOP_PORT      'E'
#define STRAFE_ENC_BOTTOM_PORT   'F'
#define POTENTIOMETER_PORT       'G'
#define LIMITSWITCH_PORT         'H'
#define VISIONSENSOR_PORT        21
#define IMU_PORT                 20


typedef struct
{
    double kP = 0;
    double kI = 0;
    double kD = 0;
    double I_max = 0;
    void print() {
        std::cout << "kP: " << this->kP << "\n";
        std::cout << "kI: " << this->kI << "\n";
        std::cout << "kD: " << this->kD << "\n";
        std::cout << "I_max: " << this->I_max << "\n";
    };
} pid;



/**
 * @see: ../lib/json.hpp
 *
 * Singleton class
 * contains class to read data from config file on sd card for better runtime config
 * useful so that a clean build is not always necessary
 * contains static variables used throughout rest of project
 */
class Configuration
{
    private:
        Configuration();
        static Configuration *config_obj;
        
    public:
        ~Configuration();
        
        /**
         * @return: Configuration -> instance of class to be used throughout program
         *
         * give user the instance of the singleton class or creates it if it does
         * not yet exist
         */
        static Configuration* get_instance();
        
        pid internal_motor_pid;
        pid lift_pid;
        pid chassis_pid;
        
        int front_right_port;
        int back_left_port;
        int front_left_port;
        int back_right_port;
        int main_intake_port;
        int hoarding_intake_port;
        int lift_port;
        
        bool front_right_reversed;
        bool back_left_reversed;
        bool front_left_reversed;
        bool back_right_reversed;
        bool main_intake_reversed;
        bool hoarding_intake_reversed;
        bool lift_reversed;
        
        std::vector<int> lift_setpoints;
        std::vector<int> tilter_setpoints;
        std::vector<int> intake_speeds;
        
        
        /**
         * @return: int -> 1 if file was successfully read, 0 if no changes were made
         *
         * @see: ../lib/json.hpp 
         *
         * parses json file looking for data to set variables to
         */
        int init();
        
        
        /**
         * @return: None
         *
         * @see: typedef struct pid
         *
         * prints all the variables in the class
         * used for debugging to make sure values are what they are
         * supposed to be
         */
        void print_config_options();

};




#endif
