/**
 * @file: ./RobotCode/src/objects/subsystems/chassis.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * Contains class for the chassis subsystem
 * has methods for driving during autonomous including turning and driving straight
 */

#ifndef __CHASSIS_HPP__
#define __CHASSIS_HPP__

#include <tuple>
#include <queue>

#include "main.h"

#include "../motors/Motor.hpp"
#include "../sensors/Sensors.hpp"


typedef enum {
    e_straight_drive
} chassis_commands;

typedef struct {
    double setpoint;
    int relative_heading=0;
    int max_voltage=12000;
    int timeout=INT32_MAX;
    bool motor_slew=false;
    bool correct_heading=true;
    bool asynch=false;
    bool log_data=false;
} chassis_params;

typedef struct {
    chassis_params args;
    int command_uid;
    chassis_commands command;
} chassis_action;


/**
 * @see: Motors.hpp
 *
 * contains methods to allow for easy control of the robot during
 * the autonomous period
 */
class Chassis
{
    private:        
        static Motor *front_left_drive;
        static Motor *front_right_drive;
        static Motor *back_left_drive;
        static Motor *back_right_drive;       
         
        static Encoder* left_encoder;
        static Encoder* right_encoder;
        static pros::Imu* imu;
        
        pros::Task *thread;  // the motor thread
        static std::queue<chassis_action> command_queue;
        static std::vector<int> commands_finished;
        static std::atomic<bool> send_lock;
        static std::atomic<bool> receive_lock;
        static int num_instances;
        
        static void straight_drive_task(chassis_params args);  // functions called by thread for asynchronous movement
        static void turn_right_task(chassis_params args);
        static void turn_right_task(chassis_params args);
        
        /**
         * @param: int left_encoder_ticks -> the setpoint in encoder ticks for the left side of the drive
         * @param: int right_encoder_ticks -> the setpoint in encoder ticks for the right side of the drive
         * @param: bool log_data -> log data to the logger queue or not
         * @return: std::tuple<int, int> -> the left and right voltages to set motor to 
         *
         * uses PID algorithm to calculate the voltage to set the motor to in order
         * to reach the setpoint
         * runs a separate pid loop for each motor with the hopes of it driving straighter
         */
        std::tuple<int, int> calc_pid( int left_encoder_ticks, int right_encoder_ticks, bool log_data=false );
                
        static double wheel_diameter;
        static double width;
        static double gear_ratio;
        
        static void chassis_motion_task(void*);
        

    public:
        Chassis( Motor &front_left, Motor &front_right, Motor &back_left, Motor &back_right, Encoder &l_encoder, Encoder &r_encoder, pros::Imu Imu, double chassis_width, double gearing=1, double wheel_size=4.05);
        ~Chassis();

        /**
         * @param: int encoder_ticks -> the number of encoder ticks to travel
         * @param: int max_voltage -> the max voltage allowed by the motor 
         * @param: int timeout -> the max amount of time the chassis will be allowed to run for
         * @param: bool tare_encoders -> tare the encoders before starting movement, use if function is blocking
         * @param: bool wait_until_settled -> if true, functions blocks until setpoint is reached
         * @param: bool log_data -> if true, data is logged, useful for graphing and debugging
         * @return: bool -> if the motor is settled or not
         *
         * runs a pid controller for each side of the drive so that it will hit 
         * setpoint at the same time
         * can be used as a blocking or non blocking function so that other commands
         * can be run at the same time as this command
         */
        bool drive( int encoder_ticks, int max_voltage=12000, int timeout=INT32_MAX, bool tare_encoders=true, bool wait_until_settled=true, bool log_data=false );
        
        
        /**
         * @param: int degrees -> the number of degrees to turn
         * @param: int max_voltage -> the max voltage allowed by the motor 
         * @param: int timeout -> the max amount of time the chassis will be allowed to run for
         * @param: bool tare_encoders -> tare the encoders before starting movement, use if function is blocking
         * @param: bool wait_until_settled -> if true, functions blocks until setpoint is reached
         * @param: bool log_data -> if true, data is logged, useful for graphing and debugging
         * @return: bool -> if the motor is settled or not
         *
         * runs a pid controller for each side of the drive so that it will hit 
         * setpoint at the same time
         * setpoint is calculated by taking into account degrees, chassis width, and wheel size
         * unit conversion is performed to go from degrees to encoder ticks for each side
         * can be used as a blocking or non blocking function so that other commands
         * can be run at the same time as this command
         */
        bool turn_right( int degrees, int max_voltage=12000, int timeout=INT32_MAX, bool tare_encoders=true, bool wait_until_settled=true, bool log_data=false );
        
        
        /**
         * @param: int degrees -> the number of degrees to turn
         * @param: int max_voltage -> the max voltage allowed by the motor 
         * @param: int timeout -> the max amount of time the chassis will be allowed to run for
         * @param: bool tare_encoders -> tare the encoders before starting movement, use if function is blocking
         * @param: bool wait_until_settled -> if true, functions blocks until setpoint is reached
         * @param: bool log_data -> if true, data is logged, useful for graphing and debugging
         * @return: bool -> if the motor is settled or not
         *
         * runs a pid controller for each side of the drive so that it will hit 
         * setpoint at the same time
         * setpoint is calculated by taking into account degrees, chassis width, and wheel size
         * unit conversion is performed to go from degrees to encoder ticks for each side
         * can be used as a blocking or non blocking function so that other commands
         * can be run at the same time as this command
         */
        bool turn_left( int degrees, int max_voltage=12000, int timeout=INT32_MAX, bool tare_encoders=true, bool wait_until_settled=true, bool log_data=false );
        
        
        
        int straight_drive(int encoder_ticks, int relative_heading=0, int max_voltage=12000, int timeout=INT32_MAX, bool asynch=false, bool correct_heading=true, bool slew=false, bool log_data=true);
        
        /**
         * @param: int voltage -> the voltage on interval [-127, 127] to set the motor to
         * @return: None
         *
         * sets voltage of chassis
         */
        void move( int voltage );
        
        
        
        
        /**
         * @return: std::tuple<int, int> -> a tuple containing encoder values for each side of chassis
         *
         * takes average of front and back encoders
         * first value is the left side, second value is the right side
         */
        static std::tuple<double, double> get_average_encoders(int l_uid, int r_uid);
        
        static double calc_delta_theta(double prev_angle, double delta_l, double delta_r);
        
        /**
         * @param: pros::motor_brake_mode_e_t new_brake_mode -> the new brakemode for the chassis
         * @return: None
         *
         * sets brake mode of all motors
         */
        void set_brake_mode( pros::motor_brake_mode_e_t new_brake_mode );



        /**
         * @return: None
         *
         * @see: Motors.hpp
         *
         * changes the direction at the api motor level so that all the
         * motors in the chassis system are reversed
         * useful for allowing to change direction of drive in user control
         */
        void change_direction();

        /**
         * @param: int speed -> the new speed the slew rate controller
         * @return: None
         *         
         * sets the internal slew rate of the motor and enables it
         */
        void enable_slew( int rate=120 );
        
        /**
         * @return: None
         *         
         * disables internal slew rate of the motor
         */
        void disable_slew( );



// Soon to be deprecated functions
        /**
         * @param: int revolutions -> how the setpoint in encoder ticks for the robot position
         * @param: int voltage -> voltage of how fast the robot will move
         * @return: None
         *
         * @see: Motors.hpp
         *
         * allows the robot to move straight towards a setpoint
         */
        void scaled_drive(float left, float right, float distance, bool pid = true, float timeout=INT32_MAX /*INT32_MAX*/); //drive functions
};




#endif
