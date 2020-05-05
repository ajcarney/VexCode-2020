/**
 * @file: ./RobotCode/src/objects/tilter/Tilter.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/17/2020
 * @reviewed_by: Aiden Carney
 *
 * @see: Tilter.hpp
 *
 * contains implementation for the tilter subsystem
 */

#include <array>
#include <vector>
#include <algorithm>

#include "../sensors/Sensors.hpp"
#include "Tilter.hpp"



Tilter::Tilter( Motor &lift_motor )
{
    motor = &lift_motor;
    
    motor->disable_velocity_pid();
    motor->disable_slew();
    motor->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}



Tilter::~Tilter()
{

}




/**
 * sets the voltage for the motor and scales it using a quadratic equation
 * if that flag is set
 */
void Tilter::move( int target_voltage, bool scale_voltage /*false*/ )
{
    if ( scale_voltage )
    {
        int tilter_pos = Motors::tilter.get_encoder_position();
        target_voltage =  ( 127.00103572106 ) / ( 1.0 + ( 0.0000081552839279816 * std::pow( 2.718281828, ( 0.00369600250676 * tilter_pos ) ) ) ); 
    }
    if ( target_voltage < 0 && Sensors::limit_switch.get_value() )
    {
        motor->tare_encoder();
        target_voltage = 0;
    }
    motor->move(target_voltage);
}




/**
 * moves tilter motor to a given setpoint and stops when it is within 10
 * encoder ticks
 */
void Tilter::move_to( int encoder_pos, bool scale_voltage /*false*/, int timeout /*INT32_MAX*/  )
{
    if ( !Sensors::limit_switch.get_value() )
    {
        motor->tare_encoder();
    }

    if ( encoder_pos > 0 )
    {
        move(127, scale_voltage);  //use voltage control because it allows the motor to go faster
    }
    else 
    {
        move(-127, scale_voltage);
    }
    

    int stop = pros::millis() + timeout;
    while ( ( std::abs(encoder_pos - motor->get_encoder_position()) ) > 10 && pros::millis() < stop )
    {
        pros::delay(10);
    }
    
    motor->move(0);
}
