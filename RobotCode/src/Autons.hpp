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
            {2, "five_cube_red_small_zone"},   //to keep title the same
            {3, "five_cube_blue_small_zone"},
            {4, "seven_cube_red_small_zone"},
            {5, "seven_cube_blue_small_zone"},
            {6, "red_big_zone"},
            {7, "blue_big_zone"},
            {8, "one_pt"},
            {9, "skills"},
            {10, "Debugger"}
        };
        const std::unordered_map <int, const char*> AUTONOMOUS_DESCRIPTIONS = {   //used to find color of auton
            {1, "goes directly to\ndriver control"},                               //selected to keep background the same
            {2, "scores five cubes in smallest\nzone for red"},
            {3, "scores five cubes in smallest\nzone for blue"},
            {4, "scores seven cubes in the\nsmallest zone for red"},
            {5, "scores seven cubes in the\nsmallest zone for blue"},
            {6, "scores in the big zone\nfor red"},
            {7, "scores in the big zone\nfor blue"},
            {8, "drives forward and\nbackwards"},
            {9, "skills auton"},
            {10, "opens debugger"}
        };
        const std::unordered_map <int, std::string> AUTONOMOUS_COLORS = {
            {1, "None"},                     //used to find color of auton
            {2, "red"},                     //selected to keep background the same
            {3, "blue"},
            {4, "red"},                     
            {5, "blue"},
            {6, "red"},
            {7, "blue"},
            {8, "None"},
            {9, "None"},
            {10, "None"}
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
         * @return: None
         *
         * @see: Motors.hpp
         * @see: Chassis.hpp
         *
         * function for dumping a stack
         */
        void dump_stack();

        
        
        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores five cubes in the smallest zone for the red team
         */
        void five_cube_red_small_zone( autonConfig cnfg );

        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores five cubes in the smallest zone for the blue team
         */
        void five_cube_blue_small_zone( autonConfig cnfg );



        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores seven cubes in the smallest zone for the red team
         */
        void seven_cube_red_small_zone( autonConfig cnfg );

        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores seven cubes in the smallest zone for the blue team
         */
        void seven_cube_blue_small_zone( autonConfig cnfg );
        
        
        
        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores cubes in the big zone for red
         */
        void red_big_zone( autonConfig cnfg );




        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * scores cubes in the big zone for blue
         */
        void blue_big_zone( autonConfig cnfg );




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
