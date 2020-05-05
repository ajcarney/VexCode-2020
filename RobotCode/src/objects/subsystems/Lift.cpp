/**
 * @file: ./RobotCode/src/objects/lift/Lift.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/17/2020
 * @reviewed_by: Aiden Carney
 *
 * @see: Lift.hpp
 *
 * contains implementation for the lift subsystem
 */

#include <vector>
#include <algorithm>

#include "Lift.hpp"
#include "../logger/Logger.hpp"



Lift::Lift( Motor &lift_motor, std::vector<int> setpoints )
{
    motor = &lift_motor;
    
    motor->disable_velocity_pid();
    motor->disable_slew();
    motor->set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    
    for ( int i = 0; i < setpoints.size(); i++ )
    {
        lift_setpoints.push_back(setpoints.at(i));
    }
        
    integral = 0;
    prev_error = 0;
    t = pros::millis();
    
    Configuration* config = Configuration::get_instance();
    pid_constants.kP = config->lift_pid.kP;
    pid_constants.kI = config->lift_pid.kI;
    pid_constants.kD = config->lift_pid.kD;
    pid_constants.I_max = config->lift_pid.I_max;
    
}



Lift::~Lift()
{

}




/**
 * sets scaled voltage on lift motor
 */
void Lift::move( int voltage )
{
    motor->move(voltage);
}




/**
 * moves to a specified potentiomter value by using a PID algorithm
 * can be blocking or not
 * can log data if flag is set
 */
int Lift::move_to( int pot_value, bool wait_until_settled /*false*/, bool log_data /*false*/)
{
    Configuration* config = Configuration::get_instance();
    double kP = pid_constants.kP;
    double kI = pid_constants.kI;
    double kD = pid_constants.kD;
    double I_max = pid_constants.I_max;
    
    int voltage;
    bool settled = false;
    
    int error;
    do
    {
        error = pot_value - get_potentiometer_value();
        if ( std::abs(error) < 10 || std::abs(integral) > I_max )
        {
            integral = 0;  //reset integral if error is too small or greater than max value
        }
        else if ( pot_value != setpoint )
        {
            integral = 0;  //reset integral for new setpoint
            setpoint = pot_value;
        }
        else 
        {
            int delta_t = pros::millis() - t;
            t = pros::millis();
            integral = integral + (error * delta_t);
        }
        double derivative = error - prev_error;
        prev_error = error;
        
        
        voltage = (kP * error) + (kI * integral) + (kD * derivative);
        
        if ( log_data )
        {
            Logger logger;
            log_entry entry;
            entry.content = (
                "[INFO] " + std::string("LIFT_PID ")
                + ", Actual_Vol: " + std::to_string(motor->get_actual_voltage())
                + ", Brake: " + std::to_string(motor->get_brake_mode())
                + ", Gear: " + std::to_string(motor->get_gearset())
                + ", I_max: " + std::to_string(I_max)
                + ", I: " + std::to_string(integral)
                + ", kD: " + std::to_string(kD)
                + ", kI: " + std::to_string(kI)
                + ", kP: " + std::to_string(kP)
                + ", Time: " + std::to_string(pros::millis())
                + ", Pot_Sp: " + std::to_string(setpoint)
                + ", Pot_Value: " + std::to_string(get_potentiometer_value())
                + ", Vel: " + std::to_string(motor->get_actual_velocity())
            );
            entry.stream = "clog";
            logger.add(entry);            
        }
        move(voltage);  //set voltage for motor

        
        if ( !wait_until_settled || ( std::abs(error) < 10 && motor->get_actual_voltage() < 15 ) )
        {
            settled = true;
        }
        
        if ( wait_until_settled )
        {
            pros::delay(10);
        }
    } while ( !settled );    

    return error;

}




/**
 * finds where the current index is in the list of setpoints
 * then increases or decreases the index to find the new setpoint
 * based on the parameter
 * caps the max and min and does not wrap back around
 */
int Lift::get_setpoint( int direction )
{
    int current_pot_value = get_potentiometer_value();
    
    std::vector<int> sorted_setpoints;
    for ( int i = 0; i < lift_setpoints.size(); i++ )
    {
        sorted_setpoints.push_back( lift_setpoints.at(i) );
    }
    sorted_setpoints.push_back( current_pot_value );

    std::sort( sorted_setpoints.begin(), sorted_setpoints.end() );
    
    
    std::vector<int>::iterator elem = std::find (sorted_setpoints.begin(),
                                                 sorted_setpoints.end(),
                                                 current_pot_value);
    int index = std::distance(sorted_setpoints.begin(), elem);

    int loc = index + direction;


    if ( loc < 0 )
    {
        loc = 0;
    }
    else if ( loc > ( lift_setpoints.size() - 1 )  )
    {
        loc = lift_setpoints.size() - 1;
    }

    return lift_setpoints.at(loc);

}



/**
 * returns the lift potentiometer value
 */
int Lift::get_potentiometer_value()
{
    return 0;//Sensors::potentiometer.get_value(false);
}




void Lift::set_pid_constants( pid new_constants )
{
    pid_constants.kP = new_constants.kP;
    pid_constants.kI = new_constants.kI;
    pid_constants.kD = new_constants.kD;
    pid_constants.I_max = new_constants.I_max;
}
