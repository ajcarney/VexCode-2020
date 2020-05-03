/**
 * @file: ./RobotCode/src/Autons.hpp
 * @author: Aiden Carney
 * @reviewed_on: 12/5/19
 * @reviewed_by: Aiden Carney
 * TODO: add and test autons as well as add functionality for autons written on the sd card
 *
 * contains class that holds data about the autonomous period as well as
 * structs for configuration data
 */

#ifndef __AUTONS_HPP__
#define __AUTONS_HPP__

#include <unordered_map>

#include "main.h"


typedef struct
{
    bool use_hardcoded = 0;
    bool gyro_turn = 1;
    bool accelleration_ctrl = 1;
    bool check_motor_tmp = 0;
    bool use_previous_macros = 1;
    bool record = 0;
} autonConfig;



/**
 * @see: Motors.hpp
 * @see: ./objects/lcdCode
 *
 * contains data for the autonomous period as well as functions to run the
 * selected autonomous
 */
class Autons
{
    private:


    public:
        Autons();
        ~Autons();


        int debug_auton_num;        //change if more autons are added
                                   //debugger should be last option
        int driver_control_num;

        const std::unordered_map <int, const char*> AUTONOMOUS_NAMES = {
            {1, "Driver Control"},             //used to find name of auton
            {2, "auton1"},                     //selected to keep title the same
            {3, "auton2"},
            {4, "auton3"},
            {5, "auton4"},
            {6, "auton5"},
            {7, "auton6"},
            {8, "Debugger"}
        };
        const std::unordered_map <int, const char*> AUTONOMOUS_DESCRIPTIONS = {   //used to find color of auton
            {1, "goes directly to driver control"},                               //selected to keep background the same
            {2, "scores four cubes in smallest\nzone"},
            {3, "scores four cubes in smallest\nzone"},
            {4, "scores in the big zone"},
            {5, "scores in the big zone"},
            {6, "drives forward and backwards"},
            {7, "runs a unit test so programmer\ncan understand what values should be"},
            {8, "opens debugger"}
        };
        const std::unordered_map <int, std::string> AUTONOMOUS_COLORS = {
            {1, "None"},                     //used to find color of auton
            {2, "red"},                     //selected to keep background the same
            {3, "blue"},
            {4, "red"},
            {5, "blue"},
            {6, "None"},
            {7, "None"},
            {8, "None"}
        };


        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores four cubes in the smallest zone for the red team
         */
        void auton1( autonConfig cnfg );

        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores four cubes in the smallest zone for the blue team
         */
        void auton2( autonConfig cnfg );



        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores cubes in the big zone for red
         */
        void auton3( autonConfig cnfg );




        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores cubes in the big zone for blue
         */
        void auton4( autonConfig cnfg );




        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * drives forward
         */
        void auton5( autonConfig cnfg );




        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * runs a unit test
         */
        void auton6( autonConfig cnfg );


};




#endif
