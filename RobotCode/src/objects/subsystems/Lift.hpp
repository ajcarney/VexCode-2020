/**
 * @file: ./RobotCode/src/objects/subsystems/Lift.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * contains class for tilter subsystem and commands to move it
 */

#ifndef __LIFT_HPP__
#define __LIFT_HPP__

#include <vector>

#include "main.h"

#include "../motors/Motor.hpp"
#include "../sensors/Sensors.hpp"




/**
 * @see: Sensors.hpp
 * @see: Motors.hpp
 *
 * contains methods for moving lift based on motor encoders as well as
 * with the potentiomter
 */
class Lift
{
    private:
        std::vector<int> lift_setpoints;
        
        pid pid_constants;
        
        int setpoint;
        int integral;
        int prev_error;
        int t;
        
        Motor *motor;

    public:
        Lift( Motor &lift_motor, std::vector<int> setpoints );
        ~Lift();

        /**
         * @param: int voltage -> the voltage on interval [-127,127] to set the motor to
         * @return: None
         *
         * @see: Motors.hpp
         *
         * moves lift by voltage
         */
        void move( int voltage );


        /**
         * @param: int pot_value -> the potentiometer setpoint for the motor to move to
         * @param: wait_until_settled -> if function should block until motor is not moving anymore
         * @param: log_data -> log data about the motor or not
         * @return: int -> error at the end of the cycle
         *
         * @see: Motors.hpp
         * @see: Sensors.hpp
         *
         * moves lift so that it is at a specific setpoint based on the
         * potentiometer
         * Useful because the potentiometer acts as an independent reading that will
         * be more accurate and consistent
         */
        int move_to( int pot_value, bool wait_until_settled=false, bool log_data=false);



        /**
         * @param: int direction -> move up or down in the setpoints list ( + = up , - = down )
         * @return: int -> the new potentiometer setpoint
         *
         * @see: Lift::move_to
         *
         * finds the next or previous index in the list based on the current pot value
         * this index is capped at either the max or min and does not loop around
         * no movement is performed if it is at the max or min
         * once the new setpoint is determined, move_to is called for the potentiomter value
         */
        int get_setpoint( int direction );
        
        
        /**
         * @return: int -> the value of the potentiometer
         *
         * returns the value of the potentiometer
         */
        int get_potentiometer_value();

        /**
         * @param: pid new_constants -> the new pid constants for the lift
         * @return: None
         *
         * sets new pid constants to use for the lift
         */
        void set_pid_constants( pid new_constants );
};



#endif
