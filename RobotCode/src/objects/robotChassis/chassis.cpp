/**
 * @file: ./RobotCode/src/objects/robotChassis/chassis.cpp
 * @author: Aiden Carney
 * @reviewed_on: 12/4/19
 * @reviewed_by: Aiden Carney
 *
 * @see: chassis.hpp
 *
 * contains implementation for chassis subsytem class
 */

#include "../../../include/main.h"
#include "../../../include/api.h"
#include "../../../include/pros/rtos.hpp"
#include "../../../include/pros/motors.hpp"

#include "chassis.hpp"



Chassis::Chassis()
{
    Motors *motors = Motors::get_instance();

    reversed = false;
    motorSlew = false;
    slewMaxSpeed = 100;

    numMotors = 4;

    motors->frontRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->backRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->frontLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->backLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
}


Chassis::~Chassis()
{

}





//other methods

/**
 * sets motors to spin to a setpoint at a given velocity
 * waits until motors are in range of 10 encoder units before stopping
 * the motors
 */
void Chassis::straight(int revolutions, int velocity, int timeout /*INT32_MAX*/ ) //drives straight
{
    Motors *motors = Motors::get_instance();

    motors->frontRight->tare_position();
    motors->frontLeft->tare_position();
    motors->backRight->tare_position();
    motors->backLeft->tare_position();

    motors->frontRight->move_absolute(revolutions, velocity);
    motors->frontLeft->move_absolute(revolutions, velocity);
    motors->backRight->move_absolute(revolutions, velocity);
    motors->backLeft->move_absolute(revolutions, velocity);

    int start = pros::millis();
    int time_elapsed = pros::millis() - start;
    
    while ( ( std::abs(revolutions - motors->frontRight->get_position()) ) > 10 
        && std::abs(revolutions - motors->frontLeft->get_position()) > 10
        && time_elapsed < timeout
    )
    {
        pros::delay(10);
        time_elapsed = pros::millis() - start;
    }

    motors->frontRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->backRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->frontLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->backLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);

}




/**
 * sets right motors to move forwards and left motors to move backwards
 * until they hit a given setpoint
 * stops motors once they are within 10 endoder units of the setpoint
 */
void Chassis::turnLeft(int revolutions, int timeout /*INT32_MAX*/ ) //turns left with wheels on both
                                        //sides of chassis spinning
{
    Motors *motors = Motors::get_instance();

    motors->frontRight->tare_position();
    motors->frontLeft->tare_position();
    motors->backRight->tare_position();
    motors->backLeft->tare_position();

    motors->frontRight->move_absolute(revolutions, 100);
    motors->frontLeft->move_absolute(0-revolutions, 100);
    motors->backRight->move_absolute(revolutions, 100);
    motors->backLeft->move_absolute(0-revolutions, 100);

    while ( ( std::abs(revolutions - motors->frontRight->get_position()) ) > 10 && std::abs(revolutions - motors->frontLeft->get_position()) > 10)
    {
        pros::delay(10);
    }

    motors->frontRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->backRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->frontLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->backLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
}




/**
 * sets left motors to move forwards and right motors to move backwards
 * until they hit a given setpoint
 * stops motors once they are within 10 endoder units of the setpoint
 */
void Chassis::turnRight(int revolutions, int timeout /*INT32_MAX*/ ) //turns right with wheels on both
                                         //sides of chassis spinning
{
    Motors *motors = Motors::get_instance();

    motors->frontRight->tare_position();
    motors->frontLeft->tare_position();
    motors->backRight->tare_position();
    motors->backLeft->tare_position();

    motors->frontRight->move_absolute(0-revolutions, 100);
    motors->frontLeft->move_absolute(revolutions, 100);
    motors->backRight->move_absolute(0-revolutions, 100);
    motors->backLeft->move_absolute(revolutions, 100);

    while ( ( std::abs(revolutions - motors->frontRight->get_position()) ) > 10 && std::abs(revolutions - motors->frontLeft->get_position()) > 10)
    {
        pros::delay(10);
    }

    motors->frontRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->backRight->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->frontLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motors->backLeft->set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
}




/**
 * does nothing
 */
void Chassis::leftSide(int revolutions) //starts left side of robot
                                        //used in auton
{

}




/**
 * does nothing
 */
void Chassis::rightSide(int revolutions) //starts right side of robot
                                         //used in auton
{

}




/**
 * sets all chassis motors to the opposite direction that they were facing
 * ie. reversed is now normal and normal is now reversed
 */
void Chassis::changeDirection() //changes orientation of robot
{
    Motors *motors = Motors::get_instance();

    reversed = !(reversed);

    motors->frontRight->set_reversed(!(motors->frontRight->is_reversed()));
    motors->frontLeft->set_reversed(!(motors->frontLeft->is_reversed()));
    motors->backLeft->set_reversed(!(motors->backLeft->is_reversed()));
    motors->backRight->set_reversed(!(motors->backRight->is_reversed()));
}




/**
 * sets the private variable to the parameter given
 */
void Chassis::setSlewMaxSpeed(int speed) //sets max velocity used by
                                         //acceleration control
{
    slewMaxSpeed = speed;
}
