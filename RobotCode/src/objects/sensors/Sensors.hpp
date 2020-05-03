/**
 * @file: ./RobotCode/src/objects/sensors/Sensors.hpp
 * @author: Aiden Carney
 * @reviewed_on: 12/4/19
 * @reviewed_by: Aiden Carney
 * TODO: move defines to configuration file as well as make a singleton class
 *
 * contains a class for interacting with the ADI sensors on the robot
 */

#ifndef __SENSORS_HPP__
#define __SENSORS_HPP__

#include "../../../include/main.h"
#include "../../../include/api.h"
#include "../../../include/pros/rtos.hpp"
#include "../../../include/pros/motors.hpp"


//sensor port definitions
#define GYRO1_PORT            'A'
#define GYRO2_PORT            'B'
#define ACCELLEROMETERX_PORT  'C'
#define ACCELLEROMETERY_PORT  'D'
#define ACCELLEROMETERZ_PORT  'E'
#define POTENTIOMETER_PORT    'F'
#define LIMITSWITCH_PORT      'G'
#define LED_PORT              'H'
#define VISIONSENSOR_PORT      20


//structs used to return sensor readings from functions
struct gyroValues
{
    float gyro1 = 0;
    float gyro2 = 0;
};

struct accelValues
{
    float Xaxis = 0;
    float Yaxis = 0;
    float Zaxis = 0;
};



/**
 * @see: pros docs
 *
 * contains methods for accessing ADI sensor values
 */
class Sensors
{
    private:


    public:
        Sensors();
        Sensors(bool calibrate);
        ~Sensors();

        static const pros::ADIAnalogIn chassisGyro1;
        static const pros::ADIAnalogIn chassisGyro2;
        static const pros::ADIAnalogIn accellerometerX;
        static const pros::ADIAnalogIn accellerometerY;
        static const pros::ADIAnalogIn accellerometerZ;
        static const pros::ADIAnalogIn potentiometer;
        static const pros::ADIDigitalIn limitSwitch;
        static const pros::ADIDigitalOut led;
        static const pros::Vision vision_sensor;

        //for other areas of code to know if analog sensors have
        //been calibrated
        static bool gyroCalibrated;
        static bool accelCalibrated;
        static bool potCalibrated;

        //calibrates analog sensors

        /**
         * @return: None
         *
         * @see: pros docs
         *
         * calibrates two gyros by making a call to the pros api
         * this is a blocking function
         * calibrated flag for the sensor is set
         */
        static void calibrateGyro();

        /**
         * @return: None
         *
         * @see: pros docs
         *
         * calibrates acceleraometer by making a call to the pros api
         * this is a blocking function
         * calibrated flag for the sensor is set
         */
        static void calibrateAccel();

        /**
         * @return: None
         *
         * @see: pros docs
         *
         * calibrates the potentiometer by making a call to the pros api
         * this is a blocking function
         * calibrated flag for the sensor is set
         */
        static void calibratePot();

        static bool led_status;

        /**
         * @return: gyroValues -> struct that contains the current raw gyro readings
         *
         * @see: pros docs
         *
         * returns the uncorrected gyro reading
         * if the gyro has not been calibrated, return INT32_MAX
         */
        gyroValues getRawGyro();

        /**
         * @return: gyroValues -> struct that contains the current corrected gyro readings
         *
         * @see: pros docs
         *
         * returns the calibrated/corrected gyro reading
         * if the gyro has not been calibrated, return INT32_MAX
         */
        gyroValues getCorrectedGyro();




        /**
         * @return: accelValues -> struct that contains the current raw accelerometer readings
         *
         * @see: pros docs
         *
         * returns the uncorrected accelerometer reading
         * if the accelerometer has not been calibrated, return INT32_MAX
         */
        accelValues getRawAccelerometer();

        /**
         * @return: accelValues -> struct that contains the current corrected accelerometer readings
         *
         * @see: pros docs
         *
         * returns the corrected accelerometer reading
         * if the accelerometer has not been calibrated, return INT32_MAX
         */
        accelValues getCorrectedAccelerometer();




        /**
         * @return: float -> current raw potentiometer readings
         *
         * @see: pros docs
         *
         * returns the uncorrected potentiometer reading
         * if the potentiometer has not been calibrated, return INT32_MAX
         */
        float getRawPot();

        /**
         * @return: float -> current corrected potentiometer readings
         *
         * @see: pros docs
         *
         * returns the corrected potentiometer reading
         * if the potentiometer has not been calibrated, return INT32_MAX
         */
        float getCorrectedPot();




        /**
         * @return: bool -> current value of the limit switch
         *
         * @see: pros docs
         *
         * returns true if the limit switch is touched and false if otherwise
         */
        bool getLimitSwitch();




        /**
         * @return: None
         *
         * @see: pros docs
         *
         * sets value of led to on
         */
        void set_led();

        /**
         * @return: None
         *
         * @see: pros docs
         *
         * sets value of led to off
         */
        void clear_led();

};




#endif
