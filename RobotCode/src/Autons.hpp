/**
 * @file: ./RobotCode/src/Autons.hpp
 * @author: Aiden Carney
 * @reviewed_on: 12/5/19
 * @reviewed_by: Aiden Carney
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


        int debug_auton_num;       //change if more autons are added
                                   //debugger should be last option
        int driver_control_num;

        const std::unordered_map <int, const char*> AUTONOMOUS_NAMES = {
            {1, "Driver Control"},             //used to find name of auton
            {2, "one_pt"},                     //to keep title the same
            {3, "skills"},
            {4, "Debugger"}
        };
        const std::unordered_map <int, const char*> AUTONOMOUS_DESCRIPTIONS = {   //used to find color of auton
            {1, "goes directly to\ndriver control"},                               //selected to keep background the same
            {2, "drives forward and\nbackwards"},
            {3, "skills auton"},
            {4, "opens debugger"}
        };
        const std::unordered_map <int, std::string> AUTONOMOUS_COLORS = {
            {1, "None"},                     //used to find color of auton
            {2, "None"},                     //selected to keep background the same  
            {3, "None"},
            {4, "None"}
        };


        /**
         * @return: None
         *
         * @see: Motors.hpp
         * @see: Chassis.hpp
         *
         * function for deploying the tray
         */
        void deploy();

        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * drives forward
         */
        void one_pt( autonConfig cnfg );

        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * runs skills
         */
        void skills( autonConfig cnfg );

};




#endif