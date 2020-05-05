/**
 * @file: ./RobotCode/src/objects/tilter/Tilter.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/17/2020
 * @reviewed_by: Aiden Carney
 *
 * contains class for tilter sub system and commands to move it
 */

#ifndef __TILTER_HPP__
#define __TILTER_HPP__

#include <array>

#include "main.h"

#include "../motors/Motors.hpp"
#include "../sensors/Sensors.hpp"




/**
 * @see: Sensors.hpp
 * @see: Motors.hpp
 *
 * contains methods for moving tilter based on motor encoders as well as
 * with the potentiomter
 */
class Tilter
{
    private:
        Motor *motor;


    public:
        Tilter( Motor &lift_motor );
        ~Tilter();

        /**
         * @param: int target_voltage -> the requested voltage to set the motor to on interval [-127,127]
         * @param: bool scale_voltage -> if the quadratic equation should be used to scale the voltage as the encoder position increases
         * @return: None
         *
         * sets the voltage of the tilter motor
         */
        void move( int target_voltage, bool scale_voltage=false );

        /**
         * @param: int encoder_pos -> the setpoint for the motor to move to
         * @param: bool scale_voltage -> if the quadratic equation should be used to scale the voltage as the encoder position increases
         * @param: bool timeout -> the max amount of time the tilter is allowed to move for
         * @return: None
         *
         * @see: Motors.hpp
         *
         * moves tilter to within a range of the setpoint using voltage control 
         * to get the ~20% speed increase
         */
        void move_to( int encoder_pos, bool scale_voltage=false, int timeout=INT32_MAX );
};



#endif
