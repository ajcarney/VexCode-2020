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
        static int selected_number;

    public:
        Autons();
        ~Autons();


        int debug_auton_num;       //change if more autons are added
                                   //debugger should be last option
        int driver_control_num;

        const std::unordered_map <int, const char*> AUTONOMOUS_NAMES = {
            {1, "Driver Control"},             //used to find name of auton
            {2, "one_pt"},                     //to keep title the same
            {3, "skills-47"},
            {4, "skills-66"},
            {5, "blue north"},
            {6, "blue north 2"},
            {7, "red north"},
            {8, "Debugger"}
        };
        const std::unordered_map <int, const char*> AUTONOMOUS_DESCRIPTIONS = {   //used to find color of auton
            {1, "goes directly to\ndriver control"},                               //selected to keep background the same
            {2, "drives forward and\nbackwards"},
            {3, "skills auton that scores 47 points"},
            {4, "skills auton that scores 66 points"},
            {5, "Goes to cap middle wall\ntower and then cycle\nstarting tower"},
            {6, "cycles closest tower, turns left"},
            {7, "cylces closest tower, turns right"},
            {8, "opens debugger"}
        };
        const std::unordered_map <int, std::string> AUTONOMOUS_COLORS = {
            {1, "none"},                     //used to find color of auton
            {2, "none"},                     //selected to keep background the same  
            {3, "none"},
            {4, "none"},
            {5, "blue"},
            {6, "blue"},
            {7, "red"},
            {8, "none"}
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
        
        
        void blue_north();
        void blue_north_2();
        void red_north();

        void run_autonomous();
};




#endif
