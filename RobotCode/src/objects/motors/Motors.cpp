#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <errno.h>
#include <cerrno>
#include <cstring>
#include <clocale>
#include <fstream>

#include "../../../include/api.h"
#include "../../../include/main.h"
#include "../../../include/pros/motors.hpp"
#include "../../../include/pros/rtos.hpp"

#include "../../../lib/date/date.h"
#include "../../Configuration.hpp"
#include "../writer/Writer.hpp"
#include "Motors.hpp"




#ifdef MACRO_RECORD_DEBUG
    std::array< std::array<int, 1001>, 5 >numbers;
#endif

Motors *Motors::motors_obj = NULL;



//default instance constructor
Motors::Motors()
{
    allow_left_chassis = true;
    allow_right_chassis = true;
    allow_intake = true;
    allow_tilter = true;
    allow_lift = true;

    //drive
    frontRight = new pros::Motor(Configuration::get_instance()->front_right_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->front_right_reversed, pros::E_MOTOR_ENCODER_DEGREES);
    backRight = new pros::Motor(Configuration::get_instance()->back_left_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->back_right_reversed, pros::E_MOTOR_ENCODER_DEGREES);
    frontLeft = new pros::Motor(Configuration::get_instance()->front_left_port, pros::E_MOTOR_GEARSET_18, 0, pros::E_MOTOR_ENCODER_DEGREES);
    backLeft = new pros::Motor(Configuration::get_instance()->back_right_port, pros::E_MOTOR_GEARSET_18, 0, pros::E_MOTOR_ENCODER_DEGREES);

    //intake
    left_intake = new pros::Motor(Configuration::get_instance()->left_intake_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->left_intake_reversed, pros::E_MOTOR_ENCODER_DEGREES);
    right_intake = new pros::Motor(Configuration::get_instance()->right_intake_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->right_intake_reversed, pros::E_MOTOR_ENCODER_DEGREES);

    //tilter
    tilter = new pros::Motor(Configuration::get_instance()->tilter_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->tilter_reversed, pros::E_MOTOR_ENCODER_DEGREES);

    //lift
    lift = new pros::Motor(Configuration::get_instance()->lift_port, pros::E_MOTOR_GEARSET_18, Configuration::get_instance()->lift_reversed, pros::E_MOTOR_ENCODER_DEGREES);
}


//instance destructor
Motors::~Motors()
{

}



/**
 * inits object if object is not already initialized based on a static bool
 * sets bool if it is not set
 */
Motors* Motors::get_instance()
{
    if ( motors_obj == NULL )
    {
        motors_obj = new Motors;
    }
    return motors_obj;
}




/**
 * returns information about a motor in a string of bits
 * this string is always 12 bits long
 */
std::string Motors::bin_string( pros::Motor* motor )
{
    std::string str;
    str += motor->get_current_draw() ? "1" : "0";
    str += ( motor->get_direction() > 0 ) ? "1" : "0";
    str += std::bitset<10>( (int)std::abs( motor->get_actual_velocity() ) ).to_string();

    return str;
}



/**
 * starts recording the macro, if MACRO_RECORD_DEBUG is set, then data will also
 * be written that can be graphed to see if the program is working
 */
void Motors::record_macro()
{
    #ifdef MACRO_RECORD_DEBUG
        int n = 0;
        for ( int i = 0; i < 5; i++ )
        {
            for ( int j = 0; j < 1001; j++ )
            {
                numbers[i][j] = n;
                n++;
            }
        }
    #endif


    //std::vector< std::pair< pros::Motor, std::string> > *test1 = new std::vector< std::pair< pros::Motor, std::string> >;

    std::vector< std::pair< pros::Motor*, std::string> > motors;

    motors.push_back( std::make_pair( frontRight, "thread1" ) );
    motors.push_back( std::make_pair( frontRight, "thread2" ) );
    motors.push_back( std::make_pair( frontRight, "thread3" ) );
    motors.push_back( std::make_pair( frontRight, "thread4" ) );
    motors.push_back( std::make_pair( frontRight, "thread5" ) );
    //test->push_back( std::make_pair( frontRight, "frontRight6" ) );
    //test->push_back( std::make_pair( frontRight, "frontRight7" ) );
    //test->push_back( std::make_pair( frontRight, "frontRight8" ) );

/*    pros::Task macro_test_task1 (record,
                                (void*)test1,
                                TASK_PRIORITY_DEFAULT,
                                TASK_STACK_DEPTH_DEFAULT,
                                "macro_test_task1" );*/


    Writer writer;
    //td::vector< std::pair< pros::Motor, std::string> > *m = static_cast <std::vector< std::pair< pros::Motor, std::string> > *> (test1);
    //std::vector< std::pair< pros::Motor, std::string> > motors = *m;

    while ( pros::millis() < 30000 )
    {
        //std::cout << writer.get_num_files_open() << "\n";
        //run unit test and tell each thread to send an item to the queue
        //every x milliseconds
        auto start = std::chrono::high_resolution_clock::now();

        //macro_test_task1.notify();
        for ( int i = 0; i < motors.size(); i++ )
        {
            #ifdef MACRO_RECORD_DEBUG
                std::string str = (
                    std::to_string( numbers[i][index] )
                    + ","
                    + date::format("%d-%m-%Y %T", date::floor<std::chrono::microseconds>(std::chrono::system_clock::now()))
                    + ","
                    + date::format("%d-%m-%Y %T", date::floor<std::chrono::microseconds>(std::chrono::system_clock::now()))
                    + "\n"
                );
                std::string file_name = std::string("/usd/log/15/") + std::get<1>( motors[i] ) + "/data.csv";
            #else
                std::string str = bin_string( std::get<0>( motors[i] ) );
                std::string file_name = std::string("/usd/macros/") + std::get<1>( motors[i] ) + "_temp";
            #endif

            writer_obj w = { file_name, "a", str };
            writer.add( w );
        }
        //std::cout << pros::millis() << " " << writer.get_count() << "\n";
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        int delta_t = ( duration.count() <= 1000 ) ? duration.count() : 1000;  //counted in microseconds, limited to 1000 microseconds
        int wait_time = ( 1000 - delta_t ) / 1000;  //convert to milliseconds because
                                                  //that is what the system clock
                                                  //uses
        pros::delay(wait_time);

        //std::cout << writer.get_count() << " " << pros::millis() << " " << wait_time << " " << duration.count() << " " << delta_t << "\n";
        //writer.dump();
    }
    std::cout << "task ended\n";
    //std::cout << "done waiting\n";
    //macro_test_task1.remove();

    //Check for Completion
    // write eof at end of file
    //wait until each file has "eof" at end of file so that it is known that it
    //is finished being written to
    #ifdef MACRO_RECORD_DEBUG

/*        Writer writer;
        std::vector< std::pair< pros::Motor, std::string> > *m = static_cast <std::vector< std::pair< pros::Motor, std::string> > *> (test1);
        std::vector< std::pair< pros::Motor, std::string> > motors = *m;*/

        //std::cout << "writing eof\n";
        //add eof so that macro can be recorded
        for ( int i = 0; i < motors.size(); i++ )
        {
            //std::string file_name = std::string("/usd/log/15/") + std::get<1>( motors[i] ) + "/data.csv";
            std::string file_name = std::string("/usd/macros/") + std::get<1>( motors[i] );
            writer_obj w = { file_name, "a", "eof" };
            writer.add( w );
        }

        //std::cout << "checking for eof to be written\n";
        //wait for eof to be written
        for( int i = 0; i < motors.size(); i++ )
        {
            std::string file_name = std::string("/usd/macros/") + std::get<1>( motors[i] ) + "_temp";
            std::string last_char;
            while( last_char.compare("f") == 0 ) //check if last char is f from "eof" -> signifies file is finished
                                                 //being written to
            {
                std::ifstream fileIn( file_name );  //Read file

                std::stringstream buffer;
                buffer << fileIn.rdbuf();
                last_char = buffer.str().back();

                fileIn.close();
                pros::delay(50);  //don't starve processer in a passive area
            }

            writer.rename( file_name, std::string("/usd/macros/") + std::get<1>( motors[i] ));
            writer.remove( file_name );
        }
        //std::cout << "function exiting\n";

    #endif



}



/**
 * does nothing at the moment
 */
void Motors::run_macro()
{

}
