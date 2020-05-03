/**
 * @file: ./RobotCode/src/objects/motors/Motor.hpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 * TODO:
 *
 * contains a wrapper class for a pros::Motor
 */
 
#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__

#include <atomic>

#include "main.h"

#include "../../Configuration.hpp"



/**
 * @see: pros::Motor
 * @see: ../../Configuration.hpp
 *
 * wrapper class for pros::Motor
 * contains implementation for better runtime port configuration
 * contains easier implementation for slew rate control 
 * contains a pid velocity controller that can be enabled for consistent motor output
 */
class Motor
{
    private:
        int motor_port;
        
        pros::Motor *motor;

        int log_level;
        
        bool slew_enabled;
        int slew_rate;
        
        bool velocity_pid_enabled;
        int prev_velocity;
        pid internal_motor_pid;
        double integral;
        double prev_error;
        
        int prev_target_voltage;
        int target_voltage;
        
        /**
         * @param int target -> the new voltage that could be requested
         * @param int previous -> the previous voltage to calculate change in voltage over time
         * @param in delta_t -> the time that has elapsed
         * @return: int -> the rate of the voltage set based on time elapsed and previous voltage
         *
         * @see: set_voltage()
         *
         * calculates the rate of change of the voltage (mv/ms) that the new
         * voltage is trying to reach
         */
        int calc_target_rate( int target, int previous, int delta_t );
        
        /**
         * @param: int voltage -> a possible motor voltage in mv on interval [-12000,12000]
         * @return: int -> the corresponding velocity for a given voltage
         *
         * TODO: add checking if the voltage is not on the interval
         *
         * calculates the corresponding velocity for a given voltage in mv
         * the velocity range corresponds to the gearset of the motor
         * velocity ranges are ~20% higher than what they are rated for 
         * because motors can achieve this velocity when supplied 12V
         */
        int calc_target_velocity( int voltage );
        
        /**
         * @return: int -> the voltage that the motor will be set at
         *
         * @see: slew rate functions contained in this class
         *
         * returns the target voltage based on the voltage set by the user
         * but that is either increased or decreased by the velocity pid if that
         * is enabled, or the slew rate code which limits the rate that the 
         * voltage can increase
         */
        int get_target_voltage( int delta_t );
        

        std::atomic<bool> lock;  //protect motor functions from concurrent access
        bool allow_driver_control;
        

    public:
        Motor(int port, pros::motor_gearset_e_t gearset, bool reversed);
        Motor(int port, pros::motor_gearset_e_t gearset, bool reversed, pid pid_consts);
        ~Motor();
    
    //accessor functions
    
        /**
         * @return: double -> the actual velocity of the motor
         *
         * @see: pros::Motor
         *
         * returns the actual velocity of the motor as calculated internally by
         * the pros::Motor
         */
        double get_actual_velocity( );
        
        /**
         * @return: double -> the actual voltage of the motor
         *
         * @see: pros::Motor
         *
         * returns the actual voltage of the motor as calculated internally by
         * the pros::Motor
         */
        double get_actual_voltage( );
        
        /**
         * @return: int -> the actual current being supplied to the motor
         *
         * @see: pros::Motor
         *
         * returns the actual current being supplied to the motor as calculated internally by
         * the pros::Motor
         */
        int get_current_draw( );
        
        /**
         * @return: double -> the encoder value of the motor
         *
         * @see: pros::Motor
         *
         * returns the encoder position of the motor in degrees as calculated internally by
         * the pros::Motor
         */
        double get_encoder_position( );
        
        /**
         * @return: pros::motor_gearset_e_t -> the gearing of the motor
         *
         * @see: pros::Motor
         *
         * returns the gearset internally used by the motor per the pros::Motor
         */
        pros::motor_gearset_e_t get_gearset( );
        
        /**
         * @return: pros::motor_brake_mode_e_t -> the brakemode of the motor
         *
         * @see: pros::Motor
         *
         * returns the brakemode internally used by the motor per the pros::Motor
         */
        pros::motor_brake_mode_e_t get_brake_mode( );
        
        /**
         * @return: int -> the port of the motor
         *         
         * returns the port that the motor is set on
         */
        int get_port( );
        
        /**
         * @return: pid -> struct of pid constants 
         *         
         * returns the pid constants in use by the motor
         */
        pid get_pid( );
        
        /**
         * @return: int -> the slew rate in use by the motor
         *         
         * returns the slew rate in mV/ms in use by the motor
         */
        int get_slew_rate( );
        
        /**
         * @return: double -> the power drawn by the motor
         *
         * @see: pros::Motor
         *
         * returns the power that the motor is drawing in Watts
         */
        double get_power( );
        
        /**
         * @return: double -> the temperature of the motor
         *
         * @see: pros::Motor
         *
         * returns the temperature of the motor in degrees C
         */
        double get_temperature( );
        
        /**
         * @return: double -> the torque output of the motor
         *
         * @see: pros::Motor
         *
         * returns the torque output of the motor in Nm
         */
        double get_torque( );
        
        /**
         * @return: int -> the direction the motor is spinning
         *
         * @see: pros::Motor
         *
         * returns the direction of the motor
         * 1 for moving in the positive direction
         * -1 for moving in the negative direction
         */    
        int get_direction( );
        
        /**
         * @return: int -> the efficiency of the motor
         *
         * @see: pros::Motor
         *
         * returns the efficiency of the motor as a percentage
         */
        int get_efficiency( );
        
        /**
         * @return: int -> if the motor is a rest
         *
         * @see: pros::Motor
         *
         * returns 1 if the motor is not moving and 0 if the motor is moving
         */
        int is_stopped( );
        
        /**
         * @return: int -> if the motor has been reversed or not
         *
         * @see: pros::Motor
         *
         * returns 1 if the motor has been reversed and 0 if the motor was not reversed
         */
        int is_reversed( );
            
            
            

    //setter functions        
        int set_port( int port );
        int tare_encoder( );
        int set_brake_mode( pros::motor_brake_mode_e_t brake_mode );
        int set_gearing( pros::motor_gearset_e_t gearset );
        int reverse_motor( );
        int set_pid( pid pid_consts );
        void set_log_level( int logging );
        

    //movement functions        
        int move( int voltage  );
        int move_velocity( int velocity );
        int set_voltage( int voltage );
    
    //slew rate control functions    
        int set_slew( int rate );
        void enable_slew( );
        void disable_slew( );
        
        
    //velocity pid control functions
        void enable_velocity_pid( );
        void disable_velocity_pid( );


    //driver control lock setting and clearing functions
        void enable_driver_control( );
        void disable_driver_control( );
        int driver_control_allowed( );
        
        
    //function to run on thread     
        int run( int delta_t );
};




#endif
