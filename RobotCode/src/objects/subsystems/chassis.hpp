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
    e_straight_drive,
    e_turn,
    e_drive_to_point
} chassis_commands;

typedef struct {
    long double dx;
    long double dy;
    long double radius;
    long double dtheta;
    std::string get_string() {
        std::string str = (
            "{dx: " + std::to_string(this->dx)
            + " dy: " + std::to_string(this->dy) 
            + " radius: " + std::to_string(this->radius)
            + " dtheta: " + std::to_string(this->dtheta)
            + "}"
        );
        return str;
    }
} waypoint;

typedef struct {
    double setpoint1=0;
    double setpoint2=0;
    int max_voltage=12000;
    int timeout=INT32_MAX;
    int recalculations=0;
    bool motor_slew=false;
    bool correct_heading=true;
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
        
        static void t_straight_drive(chassis_params args);  // functions called by thread for asynchronous movement
        static void t_turn(chassis_params args);
        static void t_move_to_waypoint(chassis_params args, waypoint point);
        
        static double wheel_diameter;
        static double width;
        static double gear_ratio;
        
        static void chassis_motion_task(void*);

    public:
        Chassis( Motor &front_left, Motor &front_right, Motor &back_left, Motor &back_right, Encoder &l_encoder, Encoder &r_encoder, pros::Imu Imu, double chassis_width, double gearing=1, double wheel_size=4.05);
        ~Chassis();


        int straight_drive(double encoder_ticks, int relative_heading=0, int max_voltage=12000, int timeout=INT32_MAX, bool asynch=false, bool correct_heading=true, bool slew=false, bool log_data=true);
        int turn_right(double degrees, int max_voltage=12000, int timeout=INT32_MAX, bool asynch=false, bool slew=false, bool log_data=true);
        int turn_left(double degrees, int max_voltage=12000, int timeout=INT32_MAX, bool asynch=false, bool slew=false, bool log_data=true);
        
        
        /**
         * @param: int voltage -> the voltage on interval [-127, 127] to set the motor to
         * @return: None
         *
         * sets voltage of chassis
         */
        void move( int voltage );
        
        
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


};


#endif
