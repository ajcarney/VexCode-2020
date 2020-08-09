/**
 * @file: ./RobotCode/src/objects/subsystems/chassis.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/16/2020
 * @reviewed_by: Aiden Carney
 *
 * @see: chassis.hpp
 *
 * contains implementation for chassis subsytem class
 */

#include "main.h"


#include "../serial/Logger.hpp"
#include "chassis.hpp"



Chassis::Chassis( Motor &front_left, Motor &front_right, Motor &back_left, Motor &back_right, double chassis_size, double wheel_size /*4.05*/ )
{
    front_left_drive = &front_left;
    front_right_drive = &front_right;
    back_left_drive = &back_left;
    back_right_drive = &back_right;
    
    wheel_diameter = wheel_size;
    chassis_width = chassis_size;
    
    integral_r = 0;
    integral_l = 0;
    prev_error_r = 0;
    prev_error_l = 0;
    setpoint_r = 0;
    setpoint_l = 0;
    t = pros::millis();

    front_left_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    front_right_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    back_left_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    back_right_drive->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    front_left_drive->disable_velocity_pid();
    front_right_drive->disable_velocity_pid();
    back_left_drive->disable_velocity_pid();
    back_right_drive->disable_velocity_pid();
    
    front_left_drive->disable_slew();
    front_right_drive->disable_slew();
    back_left_drive->disable_slew();
    back_right_drive->disable_slew();
}




Chassis::~Chassis()
{

}




/**
 * calculates the voltage for the each side of the chassis for a given setpoint
 * runs a separate pid calculation for each side of the chassis
 * performs logging at the end if flag is set 
 */
std::tuple<int, int> Chassis::calc_pid( int left_encoder_ticks, int right_encoder_ticks, bool log_data /*false*/ )
{
    Configuration* config = Configuration::get_instance();
    double kP = config->chassis_pid.kP;
    double kI = config->chassis_pid.kI;
    double kD = config->chassis_pid.kD;
    double I_max = config->chassis_pid.I_max;

    int left_error =  left_encoder_ticks - std::get<0>(get_average_encoders());
    int right_error = right_encoder_ticks - std::get<1>(get_average_encoders());
    
    if ( std::abs(right_error) < 5 || std::abs(integral_r) > I_max )
    {
        integral_r = 0;  //reset integral if error is too small or greater than max value
    }
    else if ( setpoint_r != right_encoder_ticks )
    {
        integral_r = 0;  //reset integral for new setpoint
        setpoint_r = right_encoder_ticks;
    }
    else 
    {
        int delta_t = pros::millis() - t;
        t = pros::millis();
        integral_r = integral_r + (right_error * delta_t);
    }
    
    
    if ( std::abs(left_error) < 5 || std::abs(integral_l) > I_max )
    {
        integral_l = 0;  //reset integral if error is too small or greater than max value
    }
    else if ( setpoint_l != left_encoder_ticks )
    {
        integral_l = 0;  //reset integral for new setpoint
        setpoint_l = left_encoder_ticks;
    }
    else 
    {
        int delta_t = pros::millis() - t;
        t = pros::millis();
        integral_l = integral_l + (left_error * delta_t);
    }
    
    
    double derivative_r = right_error - prev_error_r;
    prev_error_r = right_error;
    double derivative_l = left_error - prev_error_l;
    prev_error_r = left_error;
    
    
    int right_voltage = (kP * right_error) + (kI * integral_r) + (kD * derivative_r);
    int left_voltage = (kP * left_error) + (kI * integral_l) + (kD * derivative_l);

    if ( log_data )
    {
        Logger logger;
        log_entry entry;
        entry.content = (
            "[INFO] " + std::string("CHASSIS_PID ")
            + ", Actual_Vol1: " + std::to_string(front_left_drive->get_actual_voltage())
            + ", Actual_Vol2: " + std::to_string(front_right_drive->get_actual_voltage())
            + ", Actual_Vol3: " + std::to_string(back_left_drive->get_actual_voltage())
            + ", Actual_Vol4: " + std::to_string(back_right_drive->get_actual_voltage())
            + ", Brake: " + std::to_string(front_left_drive->get_brake_mode())
            + ", Gear: " + std::to_string(front_left_drive->get_gearset())
            + ", I_max: " + std::to_string(I_max)
            + ", I: " + std::to_string(integral_r)
            + ", kD: " + std::to_string(kD)
            + ", kI: " + std::to_string(kI)
            + ", kP: " + std::to_string(kP)
            + ", Time: " + std::to_string(pros::millis())
            + ", Enc_Sp: " + std::to_string(setpoint_r)
            + ", L_Enc_Value: " + std::to_string(std::get<0>(get_average_encoders()))
            + ", R_Enc_Value: " + std::to_string(std::get<1>(get_average_encoders()))
            + ", Actual_Vel1: " + std::to_string(front_left_drive->get_actual_velocity())
            + ", Actual_Vel2: " + std::to_string(front_right_drive->get_actual_velocity())
            + ", Actual_Vel3: " + std::to_string(back_left_drive->get_actual_velocity())
            + ", Actual_Vel4: " + std::to_string(back_right_drive->get_actual_velocity())
        );
        entry.stream = "clog";
        logger.add(entry);            
    }
    
    return {left_voltage, right_voltage};

}




/**
 * sets the setpoint for each side of the chassis
 * calculates voltage using calc_pid and caps it to max voltage
 * will block if that flag is set 
 * returns if the motors are settled regardless of if it is blocking or not
 */
bool Chassis::straight_drive( int encoder_ticks, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool tare_encoders /*true*/, bool wait_until_settled /*true*/, bool log_data /*false*/ )
{
    int stop = pros::millis() + timeout;
    
    if ( tare_encoders )
    {
        front_left_drive->tare_encoder();
        front_right_drive->tare_encoder();
        back_left_drive->tare_encoder();
        back_right_drive->tare_encoder();
    }
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    
    bool settled = false;
    
    do
    {
        std::tuple<int, int> voltages = calc_pid( encoder_ticks, encoder_ticks, log_data );
        
        int left_voltage = std::get<0>(voltages);
        int right_voltage = std::get<1>(voltages);
        
        //cap voltage to max voltage 
        //TODO: make sure this doesn't screw up PID controller
        if ( std::abs(left_voltage) > max_voltage )
        {
            left_voltage = left_voltage > 0 ? max_voltage : -max_voltage;
        }
        if ( std::abs(right_voltage) > max_voltage )
        {
            right_voltage = right_voltage > 0 ? max_voltage : -max_voltage;
        }

        front_left_drive->set_voltage(left_voltage);
        front_right_drive->set_voltage(right_voltage);
        back_left_drive->set_voltage(left_voltage);
        back_right_drive->set_voltage(right_voltage);
        
        int left_error = encoder_ticks - std::get<0>(get_average_encoders());
        int right_error = encoder_ticks - std::get<1>(get_average_encoders());
        
        //settled is when error is almost zero and voltage is minimal
        if ( ( std::abs(left_error) < 25 ) && ( std::abs(right_error) < 25 ) )
        {
            settled = true;
        }
        else if ( !wait_until_settled )
        {
            settled = true;
        }
        
        if ( wait_until_settled )
        {
            pros::delay(10);
        }
    } while ( !settled && pros::millis() < stop );    
    
    if ( wait_until_settled )
    {
        front_left_drive->set_voltage(0);
        front_right_drive->set_voltage(0);
        back_left_drive->set_voltage(0);
        back_right_drive->set_voltage(0);
    }
    
    front_left_drive->enable_driver_control();
    front_right_drive->enable_driver_control();
    back_left_drive->enable_driver_control();
    back_right_drive->enable_driver_control();

    return settled;
}




/**
 * sets the setpoint for each side of the chassis
 * performs unit conversion to go from degrees to encoder ticks of each side
 * calculates voltage using calc_pid and caps it to max voltage
 * will block if that flag is set 
 * returns if the motors are settled regardless of if it is blocking or not
 */
bool Chassis::turn_right( int degrees, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool tare_encoders /*true*/, bool wait_until_settled /*true*/, bool log_data /*false*/ )
{
    //convert degrees to num encoder ticks each side has to move
    long double circumference = chassis_width * 3.14159265358;               // circumference of turning arc 
    long double inches_to_turn = circumference * ( degrees / 360.0 );        // number of inches to turn to complete arc 
    long double inches_per_tick = (3.14159265358 * wheel_diameter) / 360.0;  // inches per tick = circumference of wheel / num encoder ticks per revolution
    int encoder_ticks = inches_to_turn / inches_per_tick;                    // calculate the encoder ticks each side must move

    int stop = pros::millis() + timeout;
    
    bool settled = false;
    
    if ( tare_encoders )
    {
        front_left_drive->tare_encoder();
        front_right_drive->tare_encoder();
        back_left_drive->tare_encoder();
        back_right_drive->tare_encoder();
    }
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    std::cout << encoder_ticks << "\n";
    do
    {
        std::tuple<int, int> voltages = calc_pid( encoder_ticks, -encoder_ticks, log_data );
        
        int left_voltage = std::get<0>(voltages);
        int right_voltage = std::get<1>(voltages);
        
        //cap voltage to max voltage 
        //TODO: make sure this doesn't screw up PID controller
        if ( std::abs(left_voltage) > max_voltage )
        {
            left_voltage = left_voltage > 0 ? max_voltage : -max_voltage;
        }
        if ( std::abs(right_voltage) > max_voltage )
        {
            right_voltage = right_voltage > 0 ? max_voltage : -max_voltage;
        }

        front_left_drive->set_voltage(left_voltage);
        front_right_drive->set_voltage(right_voltage);
        back_left_drive->set_voltage(left_voltage);
        back_right_drive->set_voltage(right_voltage);
        
        int left_error = encoder_ticks - std::get<0>(get_average_encoders());
        int right_error = encoder_ticks - std::get<1>(get_average_encoders());
        std::cout << left_error << " " << right_error << "\n";
        //settled is when error is almost zero and voltage is minimal
        if ( ( std::abs(left_error) < 25 ) && ( std::abs(right_error) < 25 ) )
        {
            settled = true;
        }
        else if ( !wait_until_settled )
        {
            settled = true;
        }
        
        if ( wait_until_settled )
        {
            pros::delay(10);
        }
    } while ( !settled && pros::millis() < stop );    
    
    if ( wait_until_settled )
    {
        front_left_drive->move(0);
        front_right_drive->move(0);
        back_left_drive->move(0);
        back_right_drive->move(0);
    }
    
    front_left_drive->enable_driver_control();
    front_right_drive->enable_driver_control();
    back_left_drive->enable_driver_control();
    back_right_drive->enable_driver_control();
    
    
    return settled;
}




/**
 * sets the setpoint for each side of the chassis
 * performs unit conversion to go from degrees to encoder ticks of each side
 * calculates voltage using calc_pid and caps it to max voltage
 * will block if that flag is set 
 * returns if the motors are settled regardless of if it is blocking or not
 */
bool Chassis::turn_left( int degrees, int max_voltage /*12000*/, int timeout /*INT32_MAX*/, bool tare_encoders /*true*/,  bool wait_until_settled /*true*/, bool log_data /*false*/ )
{
    //convert degrees to num encoder ticks each side has to move
    long double circumference = chassis_width * 3.14159265358;               // circumference of turning arc 
    long double inches_to_turn = circumference * ( degrees / 360.0 );        // number of inches to turn to complete arc 
    long double inches_per_tick = (3.14159265358 * wheel_diameter) / 360.0;  // inches per tick = circumference of wheel / num encoder ticks per revolution
    int encoder_ticks = inches_to_turn / inches_per_tick;                    // calculate the encoder ticks each side must move
    
    int stop = pros::millis() + timeout;
    
    bool settled = false;
    
    if ( tare_encoders )
    {
        front_left_drive->tare_encoder();
        front_right_drive->tare_encoder();
        back_left_drive->tare_encoder();
        back_right_drive->tare_encoder();
    }
    
    front_left_drive->disable_driver_control();
    front_right_drive->disable_driver_control();
    back_left_drive->disable_driver_control();
    back_right_drive->disable_driver_control();
    
    do
    {
        std::tuple<int, int> voltages = calc_pid( -encoder_ticks, encoder_ticks, log_data );
        
        int left_voltage = std::get<0>(voltages);
        int right_voltage = std::get<1>(voltages);
        
        //cap voltage to max voltage 
        //TODO: make sure this doesn't screw up PID controller
        if ( std::abs(left_voltage) > max_voltage )
        {
            left_voltage = left_voltage > 0 ? max_voltage : -max_voltage;
        }
        if ( std::abs(right_voltage) > max_voltage )
        {
            right_voltage = right_voltage > 0 ? max_voltage : -max_voltage;
        }

        
        front_left_drive->set_voltage(left_voltage);
        front_right_drive->set_voltage(right_voltage);
        back_left_drive->set_voltage(left_voltage);
        back_right_drive->set_voltage(right_voltage);
        
        int left_error = -encoder_ticks - std::get<0>(get_average_encoders());
        int right_error = encoder_ticks - std::get<1>(get_average_encoders());
        
        //settled is when error is almost zero and voltage is minimal
        if ( ( std::abs(left_error) < 10 ) && ( std::abs(right_error) < 10 ) )
        {
            settled = true;
        }
        else if ( !wait_until_settled )
        {
            settled = true;
        }
        
        if ( wait_until_settled )
        {
            pros::delay(10);
        }
    } while ( !settled && pros::millis() < stop );    
    
    if ( wait_until_settled )
    {
        front_left_drive->move(0);
        front_right_drive->move(0);
        back_left_drive->move(0);
        back_right_drive->move(0);
    }
    
    front_left_drive->enable_driver_control();
    front_right_drive->enable_driver_control();
    back_left_drive->enable_driver_control();
    back_right_drive->enable_driver_control();
    
    return settled;
}




/**
 * sets scaled voltage of each drive motor
 */
void Chassis::move( int voltage )
{
    front_left_drive->move(voltage);
    front_right_drive->move(voltage);
    back_left_drive->move(voltage);
    back_right_drive->move(voltage);
}




/**
 * takes the average of each side of the drive encoders
 * hopefully to reduce error of encoders
 * returns tuple of encoder values
 */
std::tuple<int, int> Chassis::get_average_encoders()
{
    int left_encoder = ( front_left_drive->get_encoder_position() + back_left_drive->get_encoder_position() ) / 2;
    int right_encoder = ( front_right_drive->get_encoder_position() + back_right_drive->get_encoder_position() ) / 2;
        
    return {left_encoder, right_encoder};
}




/**
 * sets a new brakemode for each drive motor
 */
void Chassis::set_brake_mode( pros::motor_brake_mode_e_t new_brake_mode )
{
    front_left_drive->set_brake_mode(new_brake_mode);
    front_right_drive->set_brake_mode(new_brake_mode);
    back_left_drive->set_brake_mode(new_brake_mode);
    back_right_drive->set_brake_mode(new_brake_mode);    
}




/**
 * sets all chassis motors to the opposite direction that they were facing
 * ie. reversed is now normal and normal is now reversed
 */
void Chassis::change_direction() 
{
    front_left_drive->reverse_motor();
    front_right_drive->reverse_motor();
    back_left_drive->reverse_motor();
    back_right_drive->reverse_motor();
}




/**
 * sets slew to enabled for each motor
 * sets the rate of the slew to the rate parameter
 */
void Chassis::enable_slew( int rate /*120*/ )
{
    front_left_drive->enable_slew();
    front_right_drive->enable_slew();
    back_left_drive->enable_slew();
    back_right_drive->enable_slew();
    
    front_left_drive->set_slew(rate);
    front_right_drive->set_slew(rate);
    back_left_drive->set_slew(rate);
    back_right_drive->set_slew(rate);
}




/**
 * sets slew to disabled for each motor
 */
void Chassis::disable_slew( ) 
{
    front_left_drive->disable_slew();
    front_right_drive->disable_slew();
    back_left_drive->disable_slew();
    back_right_drive->disable_slew();
}




/**
 * sets velocity of motor and uses mathematical scaling to maintain accuracy
 */
void Chassis::scaled_drive(float left, float right, float distance, bool pid, float timeout /*INT32_MAX*/ )
{     
    front_left_drive->tare_encoder();
    front_right_drive->tare_encoder();
    back_left_drive->tare_encoder();
    back_right_drive->tare_encoder();
     
    int stop = pros::millis() + timeout;
     
    while((fabs(std::get<0>(get_average_encoders())) <= distance && fabs(std::get<1>(get_average_encoders())) <= distance) && pros::millis() < stop )
    {
        if(pid)
        {
            left = ((distance - fabs(std::get<0>(get_average_encoders())))*(1/5.6))*(left/fabs(left)) + (25*(left/fabs(left)));
            right = ((distance - fabs(std::get<0>(get_average_encoders())))*(1/5.6))*(right/fabs(left)) + (25*(right/fabs(right)));
        }

        front_left_drive->move(left);
        front_right_drive->move(right);
        back_left_drive->move(left);
        back_right_drive->move(right);
        pros::delay(10);
    }
    
    front_left_drive->move(0);
    front_right_drive->move(0);
    back_left_drive->move(0);
    back_right_drive->move(0);
}
 