/**
 * @file: ./RobotCode/src/objects/sensors/Sensors.cpp
 * @author: Aiden Carney
 * @reviewed_on: 12/4/19
 * @reviewed_by: Aiden Carney
 *
 * @see: Sensors.hpp
 *
 * contains definitions for sensors and implementation for sensor class
 */

#include "../../../include/main.h"
#include "../../../include/api.h"
#include "../../../include/pros/rtos.hpp"
#include "../../../include/pros/motors.hpp"

#include "Sensors.hpp"

bool Sensors::led_status = false;

bool Sensors::gyroCalibrated = false;
bool Sensors::accelCalibrated = false;
bool Sensors::potCalibrated = false;

//do not initialize gyros ports or calibrate accelerometer
//because they will block for 1 sec to calibrate
//init of gyros and accelerometer occurs in seperate functions
const pros::ADIAnalogIn Sensors::chassisGyro1(GYRO1_PORT);
const pros::ADIAnalogIn Sensors::chassisGyro2(GYRO2_PORT);
const pros::ADIAnalogIn Sensors::accellerometerX(ACCELLEROMETERX_PORT);
const pros::ADIAnalogIn Sensors::accellerometerY(ACCELLEROMETERY_PORT);
const pros::ADIAnalogIn Sensors::accellerometerZ(ACCELLEROMETERZ_PORT);
const pros::ADIAnalogIn Sensors::potentiometer(POTENTIOMETER_PORT);
const pros::ADIDigitalIn Sensors::limitSwitch(LIMITSWITCH_PORT);
const pros::ADIDigitalOut Sensors::led(LED_PORT);
const pros::Vision Sensors::vision_sensor(VISIONSENSOR_PORT);


Sensors::Sensors()
{
    //does not calibrate analog sensors because they will block
    //and this may be called at unwanted times
}




Sensors::~Sensors()
{

}





/**
 * calibrates gyros
 * will block for approximately 2 sec
 * sets gyro calibrated flag
 */
void Sensors::calibrateGyro()
                                 //calibrate must be explicitly called in
                                 //order for gyro to be calibrated
                                 //this is with the intent to save time having
                                 //to keep the robot still to wait for
                                 //calibration to finish or risk it failing
{
    //init gyros, must be called before
    //any gyro functions are called
    //gyros will be calibrated either by
    //debugger or in initialize before
    //auton
    chassisGyro1.calibrate();
    chassisGyro2.calibrate();

    gyroCalibrated = true; //mainly for lcd to warn user to keep robot still
                           //if gyros have not been calibrated yet


}




/**
 * calibrates accelerometer
 * blocks for approximately 1 sec
 * sets accelerometer calibrated flag
 */
void Sensors::calibrateAccel()
                            //calibrate must be explicitly called in
                            //order for accelerometer to be calibrated
                            //this is with the intent to save time having
                            //to keep the robot still to wait for
                            //calibration to finish or risk it failing
{

    //calibrate accelerometer, must be called before
    //any accelerometer functions are called
    //accelerometer will be calibrated either by
    //debugger or in initialize before
    //auton
    accellerometerX.calibrate();
    accellerometerY.calibrate();
    accellerometerZ.calibrate();

    accelCalibrated = true; //mainly for lcd to warn user to keep robot still

}




/**
 * calibrates potentiometer
 * blocks for approximately 1 sec
 * sets potentiometer calibrated flag
 */
void Sensors::calibratePot()
                        //calibrate must be explicitly called in
                        //order for potentiometer to be calibrated
                        //this is with the intent to save time having
                        //to keep the robot still to wait for
                        //calibration to finish or risk it failing
{
    //calibrate potentiometer, must be called before
    //any potentiometer functions are called
    //potentiometer will be calibrated either by
    //debugger or in initialize before
    //auton
    potentiometer.calibrate();

    potCalibrated = true; //mainly for lcd to warn user to keep robot still
}




/**
 * makes a gyro values struct and adds the uncorrected values to it
 * if gyro has not been calibrated, values are set to INT32_MAX
 */
gyroValues Sensors::getRawGyro()
{
    gyroValues gyro_vals;
    if (gyroCalibrated)
    {
        gyro_vals.gyro1 = chassisGyro1.get_value();
        gyro_vals.gyro2 = chassisGyro2.get_value();
    }
    else
    {
        gyro_vals.gyro1 = INT32_MAX;
        gyro_vals.gyro2 = INT32_MAX;
    }

    return gyro_vals;
}




/**
 * makes a gyro values struct and adds the calibrated values to it
 * if gyro has not been calibrated, values are set to INT32_MAX
 */
gyroValues Sensors::getCorrectedGyro()
{
    gyroValues gyro_vals;
    if (gyroCalibrated)
    {
        gyro_vals.gyro1 = chassisGyro1.get_value_calibrated();
        gyro_vals.gyro2 = chassisGyro2.get_value_calibrated();
    }
    else
    {
        gyro_vals.gyro1 = INT32_MAX;
        gyro_vals.gyro2 = INT32_MAX;
    }

    return gyro_vals;
}




/**
 * makes a accelerometer values struct and adds the uncorrected values to it
 * if accelerometer has not been calibrated, values are set to INT32_MAX
 */
accelValues Sensors::getRawAccelerometer()
{
    accelValues accel_vals;
    if (accelCalibrated)
    {
        accel_vals.Xaxis = accellerometerX.get_value();
        accel_vals.Yaxis = accellerometerY.get_value();
        accel_vals.Zaxis = accellerometerZ.get_value();
    }
    else
    {
        accel_vals.Xaxis = INT32_MAX;
        accel_vals.Yaxis = INT32_MAX;
        accel_vals.Zaxis = INT32_MAX;
    }

    return accel_vals;
}




/**
 * makes a accelerometer values struct and adds the calibrated values to it
 * if accelerometer has not been calibrated, values are set to INT32_MAX
 */
accelValues Sensors::getCorrectedAccelerometer()
{
    accelValues accel_vals;
    if (accelCalibrated)
    {
        accel_vals.Xaxis = accellerometerX.get_value_calibrated();
        accel_vals.Yaxis = accellerometerY.get_value_calibrated();
        accel_vals.Zaxis = accellerometerZ.get_value_calibrated();
    }
    else
    {
        accel_vals.Xaxis = INT32_MAX;
        accel_vals.Yaxis = INT32_MAX;
        accel_vals.Zaxis = INT32_MAX;
    }

    return accel_vals;
}




/**
 * makes a potentiometer values struct and adds the uncorrected values to it
 * if potentiometer has not been calibrated, values are set to INT32_MAX
 */
float Sensors::getRawPot()
{
    float val;
    if (potCalibrated)
    {
        val = potentiometer.get_value();
    }

    else
    {
        val = INT32_MAX;
    }

    return val;
}




/**
 * makes a potentiometer values struct and adds the corrected values to it
 * if potentiometer has not been calibrated, values are set to INT32_MAX
 */
float Sensors::getCorrectedPot()
{
    float val;
    if (potCalibrated)
    {
        val = potentiometer.get_value_calibrated();
    }

    else
    {
        val = INT32_MAX;
    }

    return val;
}




/**
 * gets the value of the limit switch as either true or false
 * based on if the switch is being touched
 */
bool Sensors::getLimitSwitch()
{
    return limitSwitch.get_value();
}




/**
 * turns led on and sets the led status flag
 */
void Sensors::set_led()
{
    led.set_value(true);
    led_status = true;
}




/**
 * turns led off and clears led status flag
 */
void Sensors::clear_led()
{
    led.set_value(false);
    led_status = false;
}
