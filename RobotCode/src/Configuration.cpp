/**
 * @file: ./RobotCode/src/Configuration.cpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 *
 * @see: Configuration.hpp
 *
 * contains implementation for configuration class
 */

#include <vector>
#include <fstream>

#include "main.h"

#include "../lib/json.hpp"
#include "Configuration.hpp"


Configuration *Configuration::config_obj = NULL;


Configuration::Configuration( )
{
    //set default values for constants in case file can't be read
    internal_motor_pid.kP = 30;
    internal_motor_pid.kI = 37;
    internal_motor_pid.kD = 11;
    internal_motor_pid.I_max = INT32_MAX;

    lift_pid.kP = .1;
    lift_pid.kI = 0.0001;
    lift_pid.kD = 0;
    lift_pid.I_max = INT32_MAX;

    chassis_pid.kP = 60;
    chassis_pid.kI = 0.01;
    chassis_pid.kD = 0;
    chassis_pid.I_max = INT32_MAX;

    //536C motor config
    front_right_port = 9;
    back_left_port = 19;
    front_left_port = 10;
    back_right_port = 20;
    main_intake_port = 6;
    hoarding_intake_port = 2;
    lift_port = 16;

    front_right_reversed = 1;
    back_left_reversed = 0;
    front_left_reversed = 0;
    back_right_reversed = 1;
    main_intake_reversed = 0;
    hoarding_intake_reversed = 1;
    lift_reversed = 0;


    //536D motor config
    // front_right_port = 13;
    // back_left_port = 1;
    // front_left_port = 20;
    // back_right_port = 19;
    // left_intake_port = 2;
    // right_intake_port = 11;
    // tilter_port = 17;
    // lift_port = 12;
    //
    // front_right_reversed = 1;
    // back_left_reversed = 0;
    // front_left_reversed = 0;
    // back_right_reversed = 1;
    // left_intake_reversed = 0;
    // right_intake_reversed = 1;
    // tilter_reversed = 1;
    // lift_reversed = 0;


    //536A motor config
    // front_right_port = 20;
    // back_left_port = 2;
    // front_left_port = 5;
    // back_right_port = 4;
    // left_intake_port = 8;
    // right_intake_port = 1;
    // tilter_port = 13;
    // lift_port = 11;
    //
    // front_right_reversed = 1;
    // back_left_reversed = 1;
    // front_left_reversed = 1;
    // back_right_reversed = 1;
    // left_intake_reversed = 0;
    // right_intake_reversed = 1;
    // tilter_reversed = 0;
    // lift_reversed = 0;

    std::vector<int> vec1 {100, 300, 400, 500};
    std::vector<int> vec2 {100, 300, 400, 500};
    std::vector<int> vec3 {-63, -30, 0, 30, 63};

    tilter_setpoints = vec1;
    lift_setpoints = vec2;
    intake_speeds = vec3;
}



Configuration::~Configuration( )
{

}



/**
 * inits object if object is not already initialized based on a static bool
 * sets bool if it is not set
 */
Configuration* Configuration::get_instance()
{
    if ( config_obj == NULL )
    {
        config_obj = new Configuration;
    }
    return config_obj;
}


/**
 * reads json file into memory in the form of a json object supported by
 * a library
 * parses json array to get pid constants and setpoints by looking at the size
 * sets other variables by looking at their value
 */
int Configuration::init()
{
    std::ifstream input("/usd/config.json"); //open file with library
    if ( input.fail() )
    {
        std::cerr << "[ERROR], " << pros::millis() << ", configuration file could not be opened\n";
        return 0;
    }
    nlohmann::json contents;
    input >> contents;


    std::vector<double> constants1;  //read pid constants for different systems
    std::vector<double> constants2;

    for ( int i1 = 0; i1 < 4; i1++)
    {
        double value1 = contents["internal_motor_pid"][i1];
        double value2 = contents["lift_pid"][i1];

        std::cout << value1 << "\n";
        constants1.push_back(value1);
        constants2.push_back(value2);
    }

    internal_motor_pid.kP = constants1.at(0);
    internal_motor_pid.kI = constants1.at(1);
    internal_motor_pid.kD = constants1.at(2);
    internal_motor_pid.I_max = constants1.at(3);

    lift_pid.kP = constants2.at(0);
    lift_pid.kI = constants2.at(1);
    lift_pid.kD = constants2.at(2);
    lift_pid.I_max = constants2.at(3);




    front_right_port = contents["front_right_port"];  //read motor port definitions
    back_left_port = contents["back_left_port"];
    front_left_port = contents["front_left_port"];
    back_right_port = contents["back_right_port"];
    main_intake_port = contents["main_intake_port"];
    hoarding_intake_port = contents["hoarding_intake_port"];
    lift_port = contents["lift_port"];

    front_right_reversed = contents["front_right_reversed"] == 1 ? true : false; //read motor port reversals
    back_left_reversed = contents["back_left_reversed"] == 1 ? true : false;
    front_left_reversed = contents["front_left_reversed"] == 1 ? true : false;
    back_right_reversed = contents["back_right_reversed"] == 1 ? true : false;
    main_intake_reversed = contents["main_intake_reversed"] == 1 ? true : false;
    hoarding_intake_reversed = contents["hoarding_intake_reversed"] == 1 ? true : false;
    lift_reversed = contents["lift_reversed"] == 1 ? true : false;


    lift_setpoints.clear();
    for ( int i2 = 0; i2 < contents["lift_setpoints"].size(); i2++)
    {
        lift_setpoints.push_back(contents["lift_setpoints"][i2]);

    }

    intake_speeds.clear();
    for ( int i3 = 0; i3 < contents["intake_speeds"].size(); i3++)
    {
        intake_speeds.push_back(contents["intake_speeds"][i3]);

    }

    return 1;
}




/**
 * prints all the variables and what they are so that they can be debugged
 * makes use of internal pid print function
 */
void Configuration::print_config_options()
{
    std::cout << "drive PID constants\n";
    internal_motor_pid.print();
    std::cout << "lift PID constants\n";
    lift_pid.print();

    std::cout << "\n";

    std::cout << "front_right_port: " << front_right_port << "\n";
    std::cout << "back_left_port: " << back_left_port << "\n";
    std::cout << "front_left_port: " << front_left_port << "\n";
    std::cout << "back_right_port: " << back_right_port << "\n";
    std::cout << "main_intake_port: " << main_intake_port << "\n";
    std::cout << "hoarding_intake_port: " << hoarding_intake_port << "\n";
    std::cout << "lift_port: " << lift_port << "\n";

    std::cout << "front_right_reversed: " << front_right_reversed << "\n";
    std::cout << "back_left_reversed: " << back_left_reversed << "\n";
    std::cout << "front_left_reversed: " << front_left_reversed << "\n";
    std::cout << "back_right_reversed: " << back_right_reversed << "\n";
    std::cout << "main_intake_reversed: " << main_intake_reversed << "\n";
    std::cout << "hoarding_intake_reversed: " << hoarding_intake_reversed << "\n";
    std::cout << "lift_reversed: " << lift_reversed << "\n";


    std::cout << "\nlift_setpoints: ";
    for ( int i = 0; i < lift_setpoints.size() - 1; i++ )
    {
        std::cout << lift_setpoints.at(i) << ", ";
    }
    std::cout << lift_setpoints.at(lift_setpoints.size() - 1) << "\n";


    std::cout << "\nintake_speeds: ";
    for ( int i = 0; i < intake_speeds.size() - 1; i++ )
    {
        std::cout << intake_speeds.at(i) << ", ";
    }
    std::cout << intake_speeds.at(intake_speeds.size() - 1) << "\n";
}
