
#include "main.h"

#include "../../Configuration.hpp"
#include "Motor.hpp"



Motor::Motor( int port, pros::motor_gearset_e_t gearset, bool reversed )
{
    lock = ATOMIC_VAR_INIT(false);
    allow_driver_control = true;
    
    while ( lock.exchange( true ) ); //aquire motor lock
    
    motor_port = port;
    
    motor = new pros::Motor(port, gearset, reversed, pros::E_MOTOR_ENCODER_DEGREES);
        
    velocity_pid_enabled = true; //default motor velocity pid controller
    prev_velocity = 0;
    
    log_level = 0;
    
    slew_enabled = true;
    slew_rate = 30;  //approx. 5% voltage per 20ms == 400ms to reach full voltage
    
    prev_target_voltage = 0;
    target_voltage = 0;
    
    Configuration *configuration = Configuration::get_instance();
    internal_motor_pid.kP = configuration->internal_motor_pid.kP;
    internal_motor_pid.kI = configuration->internal_motor_pid.kI;
    internal_motor_pid.kD = configuration->internal_motor_pid.kD;
    internal_motor_pid.I_max = configuration->internal_motor_pid.I_max;
    integral = 0;
    prev_error = 0;
    
    lock.exchange(false);
}


Motor::Motor(int port, pros::motor_gearset_e_t gearset, bool reversed, pid pid_consts)
{
    lock = ATOMIC_VAR_INIT(false);
    allow_driver_control = true;
    
    while ( lock.exchange( true ) ); //aquire motor lock
    
    motor_port = port;
    
    motor = new pros::Motor(port, gearset, reversed, pros::E_MOTOR_ENCODER_DEGREES);
        
    velocity_pid_enabled = true; //default motor velocity pid controller
    prev_velocity = 0;
    
    log_level = 0; 
    
    slew_enabled = true;
    slew_rate = 30;  //approx. 5% voltage per 20ms == 400ms to reach full voltage
    
    prev_target_voltage = 0;
    target_voltage = 0;
    
    internal_motor_pid.kP = pid_consts.kP;
    internal_motor_pid.kI = pid_consts.kI;
    internal_motor_pid.kD = pid_consts.kD;
    internal_motor_pid.I_max = pid_consts.I_max;
    integral = 0;
    prev_error = 0;
    
    lock.exchange(false);
}


Motor::~Motor( )
{
    delete motor;
}




int Motor::calc_target_rate( int target, int previous, int delta_t)
{
    int delta_v = target - previous;
    int rate;
    if ( delta_t == 0 && delta_v == 0 )
    {
        rate = 0;
    }
    else if ( delta_t == 0 && delta_v != 0 )
    {
        rate = INT32_MAX;  //essentially undefined but still represented as integer
    }
    else
    {
        rate = delta_v / delta_t;
    }
    
    return rate;
}


int Motor::calc_target_velocity( int voltage )
{
    int prev_max = 12000;
    int prev_min = -12000;
    
    pros::motor_gearset_e_t gearset = motor->get_gearing();
    
    int new_max;
    int new_min;
    
    if ( gearset == pros::E_MOTOR_GEARSET_36 )  //100 RPM Motor
    {
        new_max = 120;
        new_min = -120;
    }
    if ( gearset == pros::E_MOTOR_GEARSET_06 ) //600 RPM Motor
    {
        new_max = 720;
        new_min = -720;
    }
    else //default to 200 RPM motor because that is most commonly used
    {
        new_max = 240;
        new_min = -240;
    }
    
    int velocity = (((voltage - prev_min) * (new_max - new_min)) / (prev_max - prev_min)) + new_min;
    

    return velocity;
}


int Motor::get_target_voltage( int delta_t )
{    
    double kP = internal_motor_pid.kP;
    double kI = internal_motor_pid.kI;
    double kD = internal_motor_pid.kD;
    double I_max = internal_motor_pid.I_max;
    
    //int voltage = get_actual_voltage();
    int voltage;
    int calculated_target_voltage = target_voltage;
    
    //velocity pid is enabled when the target voltage does not change
    if ( velocity_pid_enabled && target_voltage == prev_target_voltage )
    {
        int error =  calc_target_velocity(target_voltage) - get_actual_velocity();
        if ( integral == 0 || std::abs(integral) > I_max)
        {
            integral = 0;
        }
        else 
        {
            integral = integral + error;
        }
        double derivative = error - prev_error;
        prev_error = error;
        
        calculated_target_voltage = kP * error + kI * integral + kD * derivative;
        
    } 
    
    //ensure that voltage range is allowed by the slew rate set
    int rate = calc_target_rate(calculated_target_voltage, prev_target_voltage, delta_t);
    if ( slew_enabled && std::abs(rate) > slew_rate )
    {
        int max_delta_v = slew_rate * delta_t;

        int polarity = 1;                // rate will be positive or negative if motor is gaining
        if ( rate < 0 )                  // or losing velocity 
        {                                // the polarity ensures that the max voltage is added
            polarity = -1;               // in the correct direction so that the motor's velocity 
        }                                // will increase in the correct direction
        
        voltage = prev_target_voltage + (polarity * max_delta_v);
    }
    else 
    {
        voltage = calculated_target_voltage;
    }
    
    
    return voltage;
}




//accessor functions
double Motor::get_actual_velocity( )
{
    return motor->get_actual_velocity();
}


double Motor::get_actual_voltage( )
{
    return motor->get_voltage();
}


int Motor::get_current_draw( )
{
    return motor->get_current_draw();
}


double Motor::get_encoder_position( )
{
    return motor->get_position();
}


pros::motor_gearset_e_t Motor::get_gearset( )
{
    return motor->get_gearing();
}


pros::motor_brake_mode_e_t Motor::get_brake_mode( )
{
    return motor->get_brake_mode();
}


int Motor::get_port( )
{
    return motor_port;
}


pid Motor::get_pid( )
{
    return internal_motor_pid;
}


int Motor::get_slew_rate( )
{
    return slew_rate;
}


double Motor::get_power( )
{
    return motor->get_power();
}


double Motor::get_temperature( )
{
    return motor->get_temperature();
}


double Motor::get_torque( )
{
    return motor->get_torque();
}

    
int Motor::get_direction( )
{
    return motor->get_direction();
}


int Motor::get_efficiency( )
{
    return motor->get_efficiency();
}


int Motor::is_stopped( )
{
    return motor->is_stopped();
}


int Motor::is_reversed( )
{
    return motor->is_reversed();
}





//setter functions        
int Motor::set_port( int port )
{
    pros::motor_gearset_e_t gearset = motor->get_gearing();
    bool reversed = motor->is_reversed();
    
    while ( lock.exchange( true ) );
    
    try
    {
        delete motor;
        motor = new pros::Motor(port, gearset, reversed, pros::E_MOTOR_ENCODER_DEGREES);
        motor_port = port;
    }
    catch(...) //ensure lock will be released
    {
        std::cerr << "[ERROR] " << pros::millis() << "could not set port on motor port " << motor_port << "\n";
        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);
    return 1;
}


int Motor::tare_encoder( )
{
    while ( lock.exchange( true ) );
    
    try
    {
        motor->tare_position();
    }
    catch(...) //ensure lock will be released
    {
        std::cerr << "[ERROR] " << pros::millis() << "could not tare encoder on motor port " << motor_port << "\n";
        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);
    
    return 1;
}


int Motor::set_brake_mode( pros::motor_brake_mode_e_t brake_mode )
{
    while ( lock.exchange( true ) );
    
    try 
    {
        motor->set_brake_mode(brake_mode);
    }
    catch(...) //ensure lock will be released
    {
        std::cerr << "[ERROR] " << pros::millis() << "could not set brakemode on motor port " << motor_port << "\n";
        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);
    
    return 1;
}


int Motor::set_gearing( pros::motor_gearset_e_t gearset )
{
    while ( lock.exchange( true ) );
    
    try 
    {
        motor->set_gearing(gearset);
    }
    catch(...) //ensure lock will be released
    {
        std::cerr << "[ERROR] " << pros::millis() << "could not set gearing on motor port " << motor_port << "\n";
        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);
    
    return 1;
}


int Motor::reverse_motor( )
{
    while ( lock.exchange( true ) );
    
    try 
    {
        motor->set_reversed(!motor->is_reversed());
    }
    catch(...) //ensure lock will be released
    {
        std::cerr << "[ERROR] " << pros::millis() << "could not reverse motor on port " << motor_port << "\n";
        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);    
    
    return 1;
}


int Motor::set_pid( pid pid_consts )
{
    while ( lock.exchange( true ) );
    
    try 
    {
        internal_motor_pid.kP = pid_consts.kP;
        internal_motor_pid.kI = pid_consts.kI;
        internal_motor_pid.kD = pid_consts.kD;
        internal_motor_pid.I_max = pid_consts.I_max;
    }
    catch(...) //ensure lock will be released
    {
        std::cerr << "[ERROR] " << pros::millis() << "could not set motor pid on motor port " << motor_port << "\n";
        lock.exchange(false);
        return 0;
    }
    
    lock.exchange(false);
    
    return 1;    
}


void Motor::set_log_level( int logging )
{
    if ( logging > 5 )
    {
        log_level = 5;
    }
    else if ( logging < 0 )
    {
        log_level = 0;
    }
    else 
    {
        log_level = logging;
    }
}




//movement functions        
int Motor::move( int voltage  )
{
    int prev_max = 127;
    int prev_min = -127;
    int new_max = 12000;
    int new_min = -12000;
    
    int scaled_voltage = (((voltage - prev_min) * (new_max - new_min)) / (prev_max - prev_min)) + new_min;
    
    set_voltage(scaled_voltage); //dont aquire lock because it will be acquired in this function
    
    return 1;
}


int Motor::move_velocity( int velocity )
{
    pros::motor_gearset_e_t gearset = motor->get_gearing();
    
    int prev_max;
    int prev_min;
    
    if ( gearset == pros::E_MOTOR_GEARSET_36 )  //100 RPM Motor
    {
        prev_max = 120;
        prev_min = -120;
    }
    if ( gearset == pros::E_MOTOR_GEARSET_06 ) //600 RPM Motor
    {
        prev_max = 720;
        prev_min = -720;
    }
    else //default to 200 RPM motor because that is most commonly used
    {
        prev_max = 240;
        prev_min = -240;
    }

    int new_max = 12000;
    int new_min = -12000;
    
    int voltage = (((velocity - prev_min) * (new_max - new_min)) / (prev_max - prev_min)) + new_min;
    
    set_voltage(voltage); //dont aquire lock because it will be acquired in this function
    
    return 1;    
}


int Motor::set_voltage( int voltage )
{
    while ( lock.exchange( true ) );
    target_voltage = voltage;
    if ( target_voltage != prev_target_voltage )  //reset integral for new setpoint
    {
        integral = 0;
    }
    lock.exchange(false);     
    
    return 1; 
}




//velocity pid control functions
void Motor::enable_velocity_pid( )
{
    while ( lock.exchange( true ) );
    velocity_pid_enabled = true;
    lock.exchange(false);        
}


void Motor::disable_velocity_pid( )
{
    while ( lock.exchange( true ) );
    velocity_pid_enabled = false;
    lock.exchange(false);      
}


        
            
//slew control functions    
int Motor::set_slew( int rate )
{
    while ( lock.exchange( true ) );
    slew_rate = rate;
    lock.exchange(false);    
    
    return 1;
}


void Motor::enable_slew( )
{
    while ( lock.exchange( true ) );
    slew_enabled = true;
    lock.exchange(false);     
}


void Motor::disable_slew( )
{
    while ( lock.exchange( true ) );
    slew_enabled = false;
    lock.exchange(false);      
}




//driver control lock setting and clearing functions
void Motor::enable_driver_control()
{
    while ( lock.exchange( true ) );
    allow_driver_control = true;
    lock.exchange(false);
}


void Motor::disable_driver_control()
{
    while ( lock.exchange( true ) );
    allow_driver_control = false;
    lock.exchange(false);
}


int Motor::driver_control_allowed()
{
    if ( allow_driver_control )
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}




int Motor::run( int delta_t )
{
    int voltage = get_target_voltage( delta_t );
    motor->move_voltage(voltage);
    
    
    std::string log_msg; 
    switch ( log_level )
    {
        case 0:
            log_msg = "";
            break;
            
        case 1:
            log_msg = (
                "[INFO]" + std::string(" Motor ") + std::to_string(motor_port)
                + ", Actual_Vol: " + std::to_string(get_actual_voltage())
                + ", Brake: " + std::to_string(get_brake_mode())
                + ", Gear: " + std::to_string(get_gearset())
                + ", I_max: " + std::to_string(internal_motor_pid.I_max)
                + ", I: " + std::to_string(integral)
                + ", kD: " + std::to_string(internal_motor_pid.kD)
                + ", kI: " + std::to_string(internal_motor_pid.kI)
                + ", kP: " + std::to_string(internal_motor_pid.kP)
                + ", Slew: " + std::to_string(get_slew_rate())
                + ", Time: " + std::to_string(pros::millis())
                + ", Vel_Sp: " + std::to_string(calc_target_velocity(target_voltage))
                + ", Vel: " + std::to_string(get_actual_velocity())
            );
            break;
            
        case 2:
            log_msg = (
                "[INFO]" + std::string(" Motor ") + std::to_string(motor_port)
                + ", Actual_Vol: " + std::to_string(get_actual_voltage())
                + ", Brake: " + std::to_string(get_brake_mode())
                + ", Calc_Target_Vol: " + std::to_string(voltage)
                + ", Gear: " + std::to_string(get_gearset())
                + ", I_max: " + std::to_string(internal_motor_pid.I_max)
                + ", I: " + std::to_string(integral)
                + ", kD: " + std::to_string(internal_motor_pid.kD)
                + ", kI: " + std::to_string(internal_motor_pid.kI)
                + ", kP: " + std::to_string(internal_motor_pid.kP)
                + ", Slew: " + std::to_string(get_slew_rate())
                + ", Target_Vol: " + std::to_string(target_voltage)
                + ", Time: " + std::to_string(pros::millis())
                + ", Vel_Sp: " + std::to_string(calc_target_velocity(target_voltage))
                + ", Vel: " + std::to_string(get_actual_velocity())
            );
            break;
            
        case 3:
            log_msg = (
                "[INFO]" + std::string(" Motor ") + std::to_string(motor_port)
                + ", Actual_Vol: " + std::to_string(get_actual_voltage())
                + ", Brake: " + std::to_string(get_brake_mode())
                + ", Calc_Target_Vol: " + std::to_string(voltage)
                + ", Gear: " + std::to_string(get_gearset())
                + ", I_max: " + std::to_string(internal_motor_pid.I_max)
                + ", I: " + std::to_string(integral)
                + ", IME: " + std::to_string(get_encoder_position())
                + ", kD: " + std::to_string(internal_motor_pid.kD)
                + ", kI: " + std::to_string(internal_motor_pid.kI)
                + ", kP: " + std::to_string(internal_motor_pid.kP)
                + ", Slew: " + std::to_string(get_slew_rate())
                + ", Target_Vol: " + std::to_string(target_voltage)
                + ", Time: " + std::to_string(pros::millis())
                + ", Vel_Sp: " + std::to_string(calc_target_velocity(target_voltage))
                + ", Vel: " + std::to_string(get_actual_velocity())
            );
            break;
            
        case 4:
            log_msg = (
                "[INFO]" + std::string(" Motor ") + std::to_string(motor_port)
                + ", Actual_Vol: " + std::to_string(get_actual_voltage())
                + ", Brake: " + std::to_string(get_brake_mode())
                + ", Calc_Target_Vol: " + std::to_string(voltage)
                + ", Dir: " + std::to_string(get_direction())
                + ", Gear: " + std::to_string(get_gearset())
                + ", I_max: " + std::to_string(internal_motor_pid.I_max)
                + ", I: " + std::to_string(integral)
                + ", IME: " + std::to_string(get_encoder_position())
                + ", kD: " + std::to_string(internal_motor_pid.kD)
                + ", kI: " + std::to_string(internal_motor_pid.kI)
                + ", kP: " + std::to_string(internal_motor_pid.kP)
                + ", Reversed: " + std::to_string(is_reversed())
                + ", Slew: " + std::to_string(get_slew_rate())
                + ", Target_Vol: " + std::to_string(target_voltage)
                + ", Time: " + std::to_string(pros::millis())
                + ", Vel_Sp: " + std::to_string(calc_target_velocity(target_voltage))
                + ", Vel: " + std::to_string(get_actual_velocity())
            );
            break; 
            
        case 5:
            log_msg = (
                "[INFO]" + std::string(" Motor ") + std::to_string(motor_port)
                + ", Actual_Vol: " + std::to_string(get_actual_voltage())
                + ", Brake: " + std::to_string(get_brake_mode())
                + ", Calc_Target_Vol: " + std::to_string(voltage)
                + ", Current: " + std::to_string(get_current_draw())
                + ", Dir: " + std::to_string(get_direction())
                + ", Gear: " + std::to_string(get_gearset())
                + ", I_max: " + std::to_string(internal_motor_pid.I_max)
                + ", I: " + std::to_string(integral)
                + ", IME: " + std::to_string(get_encoder_position())
                + ", kD: " + std::to_string(internal_motor_pid.kD)
                + ", kI: " + std::to_string(internal_motor_pid.kI)
                + ", kP: " + std::to_string(internal_motor_pid.kP)
                + ", Reversed: " + std::to_string(is_reversed())
                + ", Slew: " + std::to_string(get_slew_rate())
                + ", Target_Vol: " + std::to_string(target_voltage)
                + ", Temp: " + std::to_string(get_temperature())
                + ", Time: " + std::to_string(pros::millis())
                + ", Torque: " + std::to_string(get_torque())
                + ", Vel_Sp: " + std::to_string(calc_target_velocity(target_voltage))
                + ", Vel: " + std::to_string(get_actual_velocity())
            );
            break;
        
    }

    std::clog << log_msg << "\n";  //change to actual logger class to avoid fragmentation
    
    return 1;
}
