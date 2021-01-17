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


class Profile {
    private:
        std::vector<double> acceleration_profile;
        std::vector<double> deceleration_profile;
        int ticks_to_accelerate;
        int ticks_to_decelerate;
        
    public:
        Profile();
        ~Profile();
        
        void generate_profile(const std::function<double(double)>& acceleration_equation, const std::function<double(double)>& deceleration_equation, int ticks_accel, int ticks_decel, int max_velocity, int min_velocity);
        bool is_generated();
        double get_target_velocity(int current_enc_value, int max_enc_value, int max_velocity);
};



typedef enum {
    e_pid_straight_drive,
    e_profiled_straight_drive,
    e_turn,
    e_drive_to_point,
    e_turn_to_point,
    e_turn_to_angle
} chassis_commands;

typedef struct {
    long double x;
    long double y;
    long double dx;
    long double dy;
    long double radius;
    long double dtheta;
    std::string get_string() {
        std::string str = (
            + "{x: " + std::to_string(this->x)
            + " y: " + std::to_string(this->y)
            + " dx: " + std::to_string(this->dx)
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
    double kP=1;
    double kI=.001;
    double kD=.001;
    double I_max=INT32_MAX;
    int max_velocity=150;
    int timeout=INT32_MAX;
    int recalculations=0;
    int explicit_direction=0;
    double motor_slew=INT32_MAX;
    bool correct_heading=true;
    bool log_data=false;
    Profile profile;
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
        
        static void t_pid_straight_drive(chassis_params args);  // functions called by thread for asynchronous movement
        static void t_profiled_straight_drive(chassis_params args);
        static void t_turn(chassis_params args);
        static void t_move_to_waypoint(chassis_params args, waypoint point);
        
        static double wheel_diameter;
        static double width;
        static double gear_ratio;
        
        static void chassis_motion_task(void*);
        
        // profiles for straight driving
        static Profile profile_1;

    public:
        Chassis( Motor &front_left, Motor &front_right, Motor &back_left, Motor &back_right, Encoder &l_encoder, Encoder &r_encoder, pros::Imu Imu, double chassis_width, double gearing=1, double wheel_size=4.05);
        ~Chassis();

        void generate_profiles();

        int pid_straight_drive(double encoder_ticks, int relative_heading=0, int max_velocity=150, int timeout=INT32_MAX, bool asynch=false, bool correct_heading=true, double slew=10, bool log_data=true);
        int profiled_straight_drive(double encoder_ticks, int max_velocity=150, int profile=0, int timeout=INT32_MAX, bool asynch=false, bool correct_heading=true, int relative_heading=0, double slew=10, bool log_data=true);
        int uneven_drive(double l_enc_ticks, double r_enc_ticks, int max_velocity=150, int timeout=INT32_MAX, bool asynch=false, double slew=10, bool log_data=false);
        int turn_right(double degrees, int max_velocity=150, int timeout=INT32_MAX, bool asynch=false, double slew=10, bool log_data=true);
        int turn_left(double degrees, int max_velocity=150, int timeout=INT32_MAX, bool asynch=false, double slew=10, bool log_data=true);
        int drive_to_point(double x, double y, int recalculations=0, int explicit_direction=0, int max_velocity=150, int timeout=INT32_MAX, bool correct_heading=true, bool asynch=false, double slew=10, bool log_data=true);
        int turn_to_point(double x, double y, int max_velocity=150, int timeout=INT32_MAX, bool asynch = false, double slew=10, bool log_data=true);
        int turn_to_angle(double theta, int max_velocity=150, int timeout=INT32_MAX, bool asynch = false, double slew=10, bool log_data=true);
        
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
        
        void wait_until_finished(int uid);
        bool is_finished(int uid);


};


#endif
