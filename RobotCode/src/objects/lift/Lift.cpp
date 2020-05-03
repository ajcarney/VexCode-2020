/**
 * @file: ./RobotCode/src/objects/lift/Lift.cpp
 * @author: Aiden Carney
 * @reviewed_on: 12/5/19
 * @reviewed_by: Aiden Carney
 *
 * @see: Lift.hpp
 *
 * contains implementation for the lift subsystem
 */

#include <array>
#include <vector>
#include <algorithm>

#include "Lift.hpp"



Lift::Lift()
{

}



Lift::~Lift()
{

}




/**
 * moves lift motor to a given setpoint and stops when it is within 10
 * encoder ticks
 */
void Lift::move( int revolutions, int timeout )
{
    Motors *motors = Motors::get_instance();

    motors->lift->tare_position();

    motors->lift->move_absolute(revolutions, 100);

    while ( ( std::abs(revolutions - motors->lift->get_position()) ) > 10 )
    {
        pros::delay(10);
    }

    motors->lift->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
}




/**
 * moves to a specified potentiomter value and sets brake
 * mode to brake to act as a holder
 */
void Lift::move_to( int pot_value )
{
    Motors *motors = Motors::get_instance();

    //TODO: add custom PID controller

    motors->lift->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

    float error = pot_value - getCorrectedPot();
    while ( std::abs( error ) > 10 )
    {
        // +/- 5 ticks
        int revolutions = ( std::abs(error) / error) * 5;  //try catch not needed
                                                           //for division because
                                                           //segment will not be executed
                                                           //if error is 0
        motors->lift->move_absolute(revolutions, 100);

        while ( motors->lift->get_current_draw() > 0)
        {
            pros::delay(1);
        }

        error = pot_value - getCorrectedPot();

    }


}




/**
 * finds where the current index is in the list of setpoints
 * then increases or decreases the index to find the new setpoint
 * based on the parameter
 * then calls move_to to move to the new setpoint
 * caps the max and min and does not wrap back around
 */
void Lift::cycle_setpoint( int direction )
{
    Configuration* config = Configuration::get_instance();


    Motors *motors = Motors::get_instance();

    int current_pot_value = getCorrectedPot();
    int target_set_point;

    std::vector<int> sorted_setpoints;
    for ( int i = 0; i < config->lift_setpoints.size(); i++ )
    {
        sorted_setpoints.push_back( config->lift_setpoints[i] );
    }
    sorted_setpoints.push_back( current_pot_value );

    std::sort( sorted_setpoints.begin(), sorted_setpoints.end() );
    std::vector<int>::iterator elem = std::find (sorted_setpoints.begin(),
                                                 sorted_setpoints.end(),
                                                 current_pot_value);
    int index = std::distance(sorted_setpoints.begin(), elem);

    int loc = (std::abs(direction) / direction) + index;

    if ( !(loc < 0) || !(loc > config->lift_setpoints.size() ))  //cap the max and min for the list
    {
        target_set_point = config->lift_setpoints[loc];
        move_to( target_set_point );
    }
}




/**
 * sets brake mode to type brake for the lift
 */
void Lift::hold( int target_pot_value )
{
    Motors *motors = Motors::get_instance();

    //TODO: add custom PID controller
    motors->lift->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

}

