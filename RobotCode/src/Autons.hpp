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

        static int selected_number;
        int debug_auton_num;       //change if more autons are added
                                   //debugger should be last option
        int driver_control_num;

        const std::unordered_map <int, const char*> AUTONOMOUS_NAMES = {
            {1, "Driver Control"},             //used to find name of auton
            {2, "one_pt"},                     //to keep title the same
            {3, "skills-47"},
            {4, "skills-66"},
            {5, "blue one tower left"},
            {6, "blue one tower right"},
            {7, "red one tower left"},
            {8, "red one tower right"},
            {9, "blue two tower left"},
            {10, "blue two tower right"},
            {11, "red two tower left"},
            {12, "red two tower right"},
            {13, "Debugger"}
        };
        const std::unordered_map <int, const char*> AUTONOMOUS_DESCRIPTIONS = {   //used to find color of auton
            {1, "goes directly to\ndriver control"},                               //selected to keep background the same
            {2, "drives forward and\nbackwards"},
            {3, "skills auton that scores 47 points"},
            {4, "skills auton that scores 66 points"},
            {5, "Caps one tower for blue, turns left"},
            {6, "Caps one tower for blue, turns right"},
            {7, "Caps one tower for red, turns left"},
            {8, "Caps one tower for red, turns right"},
            {9, "Caps two towers for blue, first tower is right"},
            {10, "Caps two towers for blue, first turn is left"},
            {11, "Caps two towers for red, first turn is right"},
            {12, "Caps two towers for red, first turn is left"},
            {13, "opens debugger"}
        };
        const std::unordered_map <int, std::string> AUTONOMOUS_COLORS = {
            {1, "none"},                     //used to find color of auton
            {2, "none"},                     //selected to keep background the same  
            {3, "none"},
            {4, "none"},
            {5, "blue"},
            {6, "blue"},
            {7, "red"},
            {8, "red"},
            {9, "blue"},
            {10, "blue"},
            {11, "red"},
            {12, "red"},
            {13, "none"}
        };

        void set_autonomous_number(int n);
        int get_autonomous_number();


        /**
         * @return: None
         *
         * @see: Motors.hpp
         * @see: Chassis.hpp
         *
         * get robot ready for auton
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
        void one_pt();

        /**
         * @param: autonConfig cnfg -> the configuration to use for the auton
         * @return: None
         *
         * @see: Motors.hpp
         *
         * runs skills
         */
        void skills();
        
        void skills2();
        
        void blue_one_tower_left();
        void blue_one_tower_right();
        void red_one_tower_left();
        void red_one_tower_right();
        
        void blue_two_tower_left();
        void blue_two_tower_right();
        void red_two_tower_left();
        void red_two_tower_right();
        
        
        void blue_north();
        void blue_north_2();
        void red_north();

        void run_autonomous();
};




#endif
