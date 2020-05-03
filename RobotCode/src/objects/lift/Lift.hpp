/**
 * @file: ./RobotCode/src/objects/lift/Lift.hpp
 * @author: Aiden Carney
 * @reviewed_on: 12/5/19
 * @reviewed_by: Aiden Carney
 * TODO: Test implementation with robot especially with setpoint and move_to
 *
 * contains class for tilter sub system and commands to move it
 */

#ifndef __LIFT_HPP__
#define __LIFT_HPP__

#include <array>

#include "../../../include/main.h"
#include "../../../include/pros/motors.hpp"

#include "../motors/Motors.hpp"
#include "../sensors/Sensors.hpp"




/**
 * @see: Sensors.hpp
 * @see: Motors.hpp
 *
 * contains methods for moving lift based on motor encoders as well as
 * with the potentiomter
 */
class Lift :
    public Sensors
{
    private:

    public:
        Lift();
        ~Lift();

        /**
         * @param: int revolutions -> the setpoint for the motor to move to
         * @return: None
         *
         * @see: Motors.hpp
         *
         * moves tilter to within a range of the setpoint
         * sets brake mode to brake at the end to act as a holder
         */
        void move( int revolutions, int timeout=INT32_MAX );

        /**
         * @param: int pot_value -> the potentiometer setpoint for the motor to move to
         * @return: None
         *
         * @see: Motors.hpp
         * @see: Sensors.hpp
         *
         * moves tilter so that it is at a specific setpoint based on the
         * potentiometer
         * Useful because the potentiometer acts as an independent reading that will
         * be more accurate and consistent
         */
        void move_to( int pot_value );




        /**
         * @param: int direction -> move up or down in the setpoints list ( + = up , - = down )
         * @return: None
         *
         * @see: Tilter::move_to
         * @see: Configuration.hpp
         *
         * finds the next or previous index in the list based on the current setpoint
         * this index is capped at either the max or min and does not loop around
         * no movement is performed if it is at the max or min
         * once the new setpoint is determined, move_to is called for the potentiomter value
         */
        void cycle_setpoint( int direction );


        /**
         * @param: int target_pot_value -> the setpoint as a potentiometer value to hold at
         * @return: None
         *
         * TODO: add actual pid holding instead of just the brake mode
         *
         * sets brake mode to pid hold
         */
        void hold( int target_pot_value ); //holds motors in place
};



#endif
