/**
 * @file: ./RobotCode/src/objects/subsystems/intakes.cpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * Contains implementation for the front intakes subsystem
 * has methods for intaking
 */

#include "main.h"


#include "../sensors/Sensors.hpp"
#include "../serial/Logger.hpp"
#include "intakes.hpp"

int Intakes::num_instances = 0;
std::queue<intake_command> Intakes::command_queue;
std::atomic<bool> Intakes::lock = ATOMIC_VAR_INIT(false);
Motor* Intakes::l_intake;
Motor* Intakes::r_intake;

Intakes::Intakes(Motor &left, Motor &right)
{    
    l_intake = &left;
    r_intake = &right;

    l_intake->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    r_intake->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    l_intake->set_motor_mode(e_voltage);
    r_intake->set_motor_mode(e_voltage);
    
    l_intake->disable_slew();
    r_intake->disable_slew();
    
    if(num_instances == 0 || thread == NULL) {
        thread = new pros::Task( intake_motion_task, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "intakes_thread");
    }
    
    num_instances += 1;
}


Intakes::~Intakes() {
    num_instances -= 1;
    if(num_instances == 0) {
        delete thread;
    }
}




void Intakes::intake_motion_task(void*) {
    l_intake->tare_encoder();
    r_intake->tare_encoder();
    l_intake->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    r_intake->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    int abs_position_l = 0;  // the absolute postions are calculated based on the change in encoder value
    int abs_position_r = 0;  // and capped to max and min values
    int prev_encoder_l = 0;
    int prev_encoder_r = 0;
    double prev_error_l = 0;
    double prev_error_r = 0;
    int integral_l = 0;
    int integral_r = 0;
    int dt = 0;
    int time = pros::millis();
    
    while(1) {
        while(1) { // delay unitl there is a command in the queue
            while ( lock.exchange( true ) ); //aquire lock and release it later
            if(!command_queue.empty()) {
                break;  // keep lock and release it later
            }
            
            lock.exchange( false ); //release lock
            pros::delay(5);
        }
        
        intake_command command = command_queue.front();  // lock is already owned
        command_queue.pop();
        lock.exchange( false ); //release lock
        
        if(command != e_pid_hold_outward) {  // reset integral if no longer holding outwards
            integral_l = 0;
            integral_r = 0;
            prev_error_l = 0;
            prev_error_r = 0;
        } else {  // calculate PID stuff
            dt = pros::millis() - time;  // calculate change in time since last command
            time = pros::millis();
            
            double d_enc_l = l_intake->get_encoder_position() - prev_encoder_l;
            double d_enc_r = r_intake->get_encoder_position() - prev_encoder_r;
            prev_encoder_l = l_intake->get_encoder_position();
            prev_encoder_r = r_intake->get_encoder_position();
            abs_position_l += d_enc_l;
            abs_position_r += d_enc_r;
            
            // cap encoder values. This can be done because mechanical stops stop the motion of
            // the intakes
            if (abs_position_l > 0) {  // innermost value of the encoder
                abs_position_l = 0;
            }
            
            if (abs_position_r > 0) {  // innermost value of the encoder
                abs_position_r = 0;
            }
            // std::cout << abs_position_l << " " << l_intake->get_actual_voltage() << "\n";
        }
        
        // execute command
        switch(command) {
            case e_intake: {
                l_intake->set_voltage(12000);
                r_intake->set_voltage(12000);
                break;
            } case e_stop_movement: {
                l_intake->set_voltage(0);
                r_intake->set_voltage(0);    
                break;
            } case e_secure: {
                l_intake->set_voltage(12000);
                r_intake->set_voltage(12000);
                if ((l_intake->get_torque() + r_intake->get_torque()) / 2 > 1) { // wait a little bit and then say ball is secure
                    pros::delay(300);
                    l_intake->set_voltage(0);
                    r_intake->set_voltage(0);
                }
                break;
            } case e_hold_outward: {  // PI controller to hold outwards
                l_intake->set_voltage(-4500);
                r_intake->set_voltage(-4500);
                
                break;
            } case e_pid_hold_outward: {
                double l_error = -42 - abs_position_l;  // set first number to encoder setpoint
                double r_error = -42 - abs_position_r;  // set first number to encoder setpoint
                if(Sensors::l_limit_switch.get_value()) {
                    l_error = 0;
                }
                if(Sensors::r_limit_switch.get_value()) {
                    r_error = 0;
                }
                
                integral_l = integral_l + (l_error * dt);
                integral_r = integral_r + (r_error * dt);
                if(std::abs(l_error) < 1) {  // reset integral if error is 0
                    integral_l = 0;
                }
                if(std::abs(r_error) < 1) {
                    integral_r = 0;
                }
                
                // cap voltage to max voltage with regard to velocity
                double i_max = 12000;
                if ( std::abs(integral_l) > i_max ) {
                    integral_l = integral_l > 0 ? i_max : -i_max;
                }
                if ( std::abs(integral_r) > i_max ) {
                    integral_r = integral_r > 0 ? i_max : -i_max;
                }

                double d_error_l = l_error - prev_error_l;
                double d_error_r = r_error - prev_error_r;
                
                prev_error_l = l_error;
                prev_error_r = r_error;

                int voltage_l = (1 * l_error) + (10 * integral_l) + (1 * d_error_l);  // set first number to kP, second number to kI
                int voltage_r = (1 * r_error) + (10 * integral_r) + (1 * d_error_r);  // set first number to kP, second number to kI

                // cap voltage to max and min voltages
                if ( voltage_l < -12000 ) {
                    voltage_l = -12000;
                } else if(voltage_l > -1500) {
                    voltage_l = -1500;
                }
                
                if (voltage_r < -12000 ) {
                    voltage_r = -12000;
                } else if(voltage_r > -2000) {
                    voltage_r = -2000;
                }

                std::cout << Sensors::l_limit_switch.get_value() << " " << Sensors::r_limit_switch.get_value() << " " << l_error << " " << r_error << " " << voltage_l << " " << voltage_r << "\n";

                l_intake->set_voltage(voltage_l);
                r_intake->set_voltage(voltage_r);

                break;
            } case e_rocket_outward: {
                l_intake->set_voltage(-12000);
                r_intake->set_voltage(-12000);
                break;
            }
        }
        
    }
}

void Intakes::intake() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_intake);
    lock.exchange( false ); //release lock
}

void Intakes::stop() {
    reset_queue();
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_stop_movement);
    lock.exchange( false ); //release lock
}

void Intakes::intake_until_secure() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_secure);
    lock.exchange( false ); //release lock
}

void Intakes::hold_outward() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_hold_outward);
    lock.exchange( false ); //release lock
}

void Intakes::pid_hold_outward() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_pid_hold_outward);
    lock.exchange( false ); //release lock
}

void Intakes::rocket_outward() {
    while ( lock.exchange( true ) ); //aquire lock
    command_queue.push(e_rocket_outward);
    lock.exchange( false ); //release lock
}

void Intakes::reset_queue() {
    while ( lock.exchange( true ) ); //aquire lock
    std::queue<intake_command> empty_queue;
    std::swap( command_queue, empty_queue );  // replace command queue with an empty queue
    lock.exchange( false ); //release lock    
}
