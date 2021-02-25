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


#define LEFT_ENC_TOP_PORT        'G'
#define LEFT_ENC_BOTTOM_PORT     'H'
#define RIGHT_ENC_TOP_PORT       'A'
#define RIGHT_ENC_BOTTOM_PORT    'B'
#define STRAFE_ENC_TOP_PORT      'E'
#define STRAFE_ENC_BOTTOM_PORT   'F'
#define DETECTOR_MIDDLE_PORT     'C'
#define POTENTIOMETER_PORT       'Z'

#define DETECTOR_BOTTOM_PORT     'Z'  // no port available but still wanted in code
#define DETECTOR_TOP_PORT        'D'  // no port available but still wanted in code

#define OPTICAL_PORT              5
#define IMU_PORT                 10
#define EXPANDER_PORT            4


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
        int left_intake_port;
        int right_intake_port;
        int upper_indexer_port;
        int lower_indexer_port;

        bool front_right_reversed;
        bool back_left_reversed;
        bool front_left_reversed;
        bool back_right_reversed;
        bool left_intake_reversed;
        bool right_intake_reversed;
        bool upper_indexer_reversed;
        bool lower_indexer_reversed;

        std::vector<int> lift_setpoints;
        std::vector<int> tilter_setpoints;
        std::vector<int> intake_speeds;

        int filter_threshold;
        std::string filter_color;  // color to remove


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
