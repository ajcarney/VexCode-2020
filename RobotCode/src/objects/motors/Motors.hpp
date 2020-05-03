/**
 * @file: ./RobotCode/src/motors/Motors.hpp
 * @author: Aiden Carney
 * @reviewed_on: 11/8/19
 * @reviewed_by: Aiden Carney
 * TODO: add function for updating motors
 *
 * contains singleton class for all the motors
 */

#ifndef __MOTORS_HPP__
#define __MOTORS_HPP__

#include <string>
#include <unordered_map>

#include "../../../include/api.h"
#include "../../../include/main.h"
#include "../../../include/pros/motors.hpp"
#include "../../../include/pros/rtos.hpp"

#include "../../Configuration.hpp"

//motor port definitions
#define FRONTRIGHT_PORT    5
#define BACKRIGHT_PORT     2
#define FRONTLEFT_PORT     3
#define BACKLEFT_PORT      4
#define LEFT_INTAKE_PORT   6
#define RIGHT_INTAKE_PORT  8
#define TILTER_PORT        7
#define LIFT_PORT          9



/**
 * @see: ../../Configuration.hpp
 *
 * has motor functions to be used throughout program
 * exists so that all motors are in one place and can be changed easily
 * purpose of class line 3
 */
class Motors
{
    private:
        Motors();
        static Motors *motors_obj;

        static std::string bin_string( pros::Motor* motor );


    public:
        ~Motors();

        /**
         * @return: Motors* -> instance of class to be used throughout program
         *
         * give user the instance of the singleton class or creates it if it does
         * not yet exist
         */
        static Motors* get_instance();


        bool allow_left_chassis; //bools allow for autonomous commands
                                 //to be run in opcontrol
                                 //eg. motor defaults to off
                                 //in opcontrol, so if task is being
                                 //run motor will stall
        bool allow_right_chassis;
        bool allow_intake;
        bool allow_tilter;
        bool allow_lift;

        pros::Motor *frontRight; //drive motors
        pros::Motor *backRight;
        pros::Motor *frontLeft;
        pros::Motor *backLeft;

        pros::Motor *right_intake; //intake motors
        pros::Motor *left_intake;

        pros::Motor *tilter; //tilter motor

        pros::Motor *lift; //other motor

        /**
         * @return: None
         *
         * @see: ../writer/Writer.hpp
         *
         * takes information from motor at 1 second intervals and sends
         * it to the writer queue
         */
        void record_macro();


        /**
         * @return: None
         *
         * TODO: add actual content to function
         *
         * takes data from the sd card for motor movements over an interval of time
         * with the intent of repeating the action that occured at that time
         */
        void run_macro();


};




#endif
