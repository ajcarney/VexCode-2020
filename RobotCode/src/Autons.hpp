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
        void one_tower_auton(std::string filter_color, int turn_direction);
        void two_tower_auton(std::string filter_color, int turn_direction);
        void two_tower_mid_auton(std::string filter_color, int turn_direction);
        

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
            {3, "skills-current"},
            {4, "skills-old"},
            {5, "blue one tower left"},
            {6, "blue one tower right"},
            {7, "red one tower left"},
            {8, "red one tower right"},
            {9, "blue two tower left"},
            {10, "blue two tower right"},
            {11, "red two tower left"},
            {12, "red two tower right"},
            {13, "blue two tower mid left"},
            {14, "blue two tower mid right"},
            {15, "red two tower mid left"},
            {16, "red two tower mid right"},
            {17, "Debugger"}
        };
        const std::unordered_map <int, const char*> AUTONOMOUS_DESCRIPTIONS = {   //used to find color of auton
            {1, "goes directly to\ndriver control"},                               //selected to keep background the same
            {2, "deploys and thats about it"},
            {3, "skills auton that scores ? points"},
            {4, "skills auton that scores 66 points\n on a good run"},
            {5, "Caps one tower for blue, \nturns left"},
            {6, "Caps one tower for blue, \nturns right"},
            {7, "Caps one tower for red, \nturns left"},
            {8, "Caps one tower for red, \nturns right"},
            {9, "Caps two towers for blue, \nfirst tower is right"},
            {10, "Caps two towers for blue, \nfirst turn is left"},
            {11, "Caps two towers for red, \nfirst turn is right"},
            {12, "Caps two towers for red, \nfirst turn is left"},
            {13, "Caps two towers for blue middle, \nfirst tower is right"},
            {14, "Caps two towers for blue middle, \nfirst turn is left"},
            {15, "Caps two towers for red middle, \nfirst turn is right"},
            {16, "Caps two towers for red middle, \nfirst turn is left"},
            {17, "opens debugger"}
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
            {13, "blue"},
            {14, "blue"},
            {15, "red"},
            {16, "red"},
            {17, "none"}
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
         * @return: None
         *
         * @see: Motors.hpp
         *
         * drives forward
         */
        void one_pt();

        /**
         * @return: None
         *
         * @see: Motors.hpp
         *
         * runs skills
         */
        void skills();
        
        void skills_old();
        
        void blue_one_tower_left();
        void blue_one_tower_right();
        void red_one_tower_left();
        void red_one_tower_right();
        
        void blue_two_tower_left();
        void blue_two_tower_right();
        void red_two_tower_left();
        void red_two_tower_right();
        
        void blue_two_tower_mid_left();
        void blue_two_tower_mid_right();
        void red_two_tower_mid_left();
        void red_two_tower_mid_right();
        
        void blue_north();
        void blue_north_2();
        void red_north();

        void run_autonomous();
};




#endif
