/**
 * @file: ./RobotCode/src/objects/robotChassis/chassis.hpp
 * @author: Aiden Carney
 * @reviewed_on: 12/4/19
 * @reviewed_by: Aiden Carney
 * TODO: Clean up includes
 *
 * Contains class for the chassis subsystem
 * has methods for driving during autonomous including turning and driving straight
 */

#ifndef __CHASSIS_HPP__
#define __CHASSIS_HPP__

#include "../../../include/main.h"
#include "../../../include/api.h"
#include "../../../include/pros/rtos.hpp"
#include "../../../include/pros/motors.hpp"

#include "../motors/Motors.hpp"
#include "../sensors/Sensors.hpp"


/**
 * @see: Motors.hpp
 *
 * contains methods to allow for easy control of the robot during
 * the autonomous period
 */
class Chassis :
    public Sensors
{
    private:
        bool reversed;
        bool motorSlew;
        int slewMaxSpeed;
        int numMotors;


    public:
        Chassis();
        ~Chassis();

        /**
         * @param: int revolutions -> how the setpoint in encoder ticks for the robot position
         * @param: int velocity -> percentage of how fast the robot will move
         * @return: None
         *
         * @see: Motors.hpp
         *
         * TODO: add custom PID distance control, acceleration control, and straight drive
         *
         * allows the robot to move straight towards a setpoint
         */
        void straight(int revolutions, int velocity, int timeout=INT32_MAX); //drive functions


        /**
         * @param: int revolutions -> the setpoint for the motor encoders
         * @return: None
         *
         * @see: Motors.hpp
         *
         *
         * TODO: add velocity parameter, custom PID controller
         *
         * turns left at full speed by turning both wheels of the robot
         * in opposite directions
         */
        void turnLeft(int revolutions, int timeout=INT32_MAX); //turn functions

        /**
         * @param: int revolutions -> the setpoint for the motor encoders
         * @return: None
         *
         * @see: Motors.hpp
         *
         *
         * TODO: add velocity parameter, custom PID controller
         *
         * turns right at full speed by turning both wheels of the robot
         * in opposite directions
         */
        void turnRight(int revolutions, int timeout=INT32_MAX);

        /**
         * @param: int revolutions -> the setpoint for the motor encoders
         * @return: None
         *
         * @see: Motors.hpp
         *
         * TODO: add functionality
         *
         * does nothing
         */
        void leftSide(int revolutions);

        /**
         * @param: int revolutions -> the setpoint for the motor encoders
         * @return: None
         *
         * @see: Motors.hpp
         *
         * TODO: add functionality
         *
         * does nothing
         */
        void rightSide(int revolutions);




        /**
         * @return: None
         *
         * @see: Motors.hpp
         *
         * changes the direction at the api motor level so that all the
         * motors in the chassis system are reversed
         * useful for allowing to change direction of drive in user control
         */
        void changeDirection(); //misc functions

        /**
         * @param: int speed -> the new speed the slew rate controller
         * @return: None
         *
         * TODO: add data validation
         *
         * Setter function for private variable int slewMaxSpeed
         */
        void setSlewMaxSpeed(int speed);
};




#endif
