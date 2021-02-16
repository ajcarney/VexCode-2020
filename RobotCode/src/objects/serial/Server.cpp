/**
 * @file: ./RobotCode/src/objects/serial/Server.cpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 *
 * contains implementation for server implementation
 */
#include <atomic>
#include <cstdint>
#include <queue>
#include <string>

#include "main.h"
#include "pros/apix.h"

#include "../../Configuration.hpp"
#include "../motors/Motors.hpp"
#include "../motors/MotorThread.hpp"
#include "Logger.hpp"
#include "Server.hpp"

std::queue<server_request> Server::request_queue;
std::atomic<bool> Server::lock = ATOMIC_VAR_INIT(false);
pros::Task *Server::read_thread = NULL;
int Server::num_instances = 0;
bool Server::debug = false;
int Server::delay = 100;


Server::Server() { 
    if(read_thread == NULL) {
        read_thread = new pros::Task( read_stdin, (void*)NULL, 2, TASK_STACK_DEPTH_DEFAULT, "server_thread");
        read_thread->suspend();
    }

    num_instances += 1;
}




Server::~Server() {
    std::cout << "destructor called on server\n";
    num_instances -= 1;
    if(num_instances == 0) {
        read_thread->remove();
        delete read_thread;
        read_thread = NULL;        
    }
}



void Server::read_stdin(void*) {
    int read_check = 0;
    Logger logger;
    log_entry entry;
    int wait_check = 0;
    
    while(1) {
        char byte = getchar_unlocked();

        if(debug) {
            entry.stream = "clog";
            entry.content = "[INFO] " + std::to_string(pros::millis()) + " Byte read from stdin: " + byte;
            logger.add(entry);
        }

        if(read_check == 0 && byte == '\xAA') {
            read_check = 1;
        } else if(read_check == 1 && byte == '\x55') {
            read_check = 2;
        } else if(read_check == 2 && byte == '\x1E') {
            read_check = 3;
        } else if(read_check == 3) {
            std::string msg;
            int len_msg = (int)byte - 4;  // byte read will be length of bytes to follow 
                                          // subtract 4 because next four bytes are handled different
                                          // because they are identifiers
            uint8_t return_msb = getchar_unlocked();
            uint8_t return_lsb = getchar_unlocked();
            uint8_t command_msb = getchar_unlocked();
            uint8_t command_lsb = getchar_unlocked();
                        
            uint16_t return_id = (return_msb << 8) | return_lsb;
            uint16_t command_id = (command_msb << 8) | command_lsb;

            for(int i=0; i<len_msg; i++) {  // read rest message
                msg.push_back(getchar_unlocked());
            }

            char checksum = getchar_unlocked();  // checksum is directly after end of message

            if(debug) {
                entry.stream = "clog";
                entry.content = (
                    "[INFO], " 
                    + std::to_string(pros::millis()) 
                    + ", Return ID read: " + std::to_string(return_id)
                    + ", Command ID read: " + std::to_string(command_id)
                    + ", Msg read: " + msg
                    + ", Checksum read: " + checksum
                );
                logger.add(entry);
            }

            if(checksum == '\xC6')
            {
                server_request request;
                request.return_id = return_id;
                request.command_id = command_id;
                request.msg = msg;
                
                while ( lock.exchange( true ) ); //aquire lock
                request_queue.push(request);
                msg = '\0';
                lock.exchange( false ); //release lock
            }
            
            read_check = 0;
            len_msg = 0;
            msg[0] = '\0';
            
        } else {
            read_check = 0;
        }
        
        wait_check += 1;
        if(wait_check > 1024) {  // wait after 1024 interactions
            wait_check = 0;
            pros::delay(10);
        }
    }
}



int Server::handle_request(server_request request) {
    // cases are defined in commands.ods
    Logger logger;
    log_entry entry;
    entry.stream = "clog";

    std::string return_msg;
    return_msg.push_back('\xAA');
    return_msg.push_back('\x55');
    return_msg.push_back('\x1E');
    
    std::string return_msg_body;
    int status;

    switch(request.command_id) {
    // motor interaction post cases
        case 45232: {  //0xB0 0xB0  Set voltage
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                int voltage = std::stoi(request.msg);
                
                status = Motors::motor_array.at(motor_number)->set_voltage(voltage);
            }
            break;
            
        case 45233: {  //0xB0 0xB1  Set Slew Rate
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                int slew_rate = std::stoi(request.msg);
                
                status = Motors::motor_array.at(motor_number)->set_slew(slew_rate);
            }
            break;
            
        case 45234: {  //0xB0 0xB2  Set Port
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                int port = std::stoi(request.msg);
                
                status = Motors::motor_array.at(motor_number)->set_port(port);
            }
            break;
            
        case 45235: {  //0xB0 0xB3  Tare IME
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                
                status = Motors::motor_array.at(motor_number)->tare_encoder();
            }
            break;
            
        case 45236: {  //0xB0 0xB4  Set Brakemode
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                pros::motor_brake_mode_e_t new_brake_mode = static_cast<pros::motor_brake_mode_e_t>(std::stoi(request.msg));
                
                status = Motors::motor_array.at(motor_number)->set_brake_mode(new_brake_mode);
            }
            break;
            
        case 45237: {  //0xB0 0xB5  Set Gearing
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                pros::motor_gearset_e_t new_gearing = static_cast<pros::motor_gearset_e_t>(std::stoi(request.msg));
                
                status = Motors::motor_array.at(motor_number)->set_gearing(new_gearing);
            }
            break;
            
        case 45238: {  //0xB0 0xB6  Set PID
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                
                char buffer1[8];
                char buffer2[8];
                char buffer3[8];
                char buffer4[8];
                
                std::copy(request.msg.begin(), request.msg.begin() + 8, buffer1);
                std::copy(request.msg.begin() + 8, request.msg.begin() + 16, buffer2);
                std::copy(request.msg.begin() + 16, request.msg.begin() + 24, buffer3);
                std::copy(request.msg.begin() + 24, request.msg.begin() + 32, buffer4);
                
                double n1 = *reinterpret_cast<double*>(buffer1);
                double n2 = *reinterpret_cast<double*>(buffer2);
                double n3 = *reinterpret_cast<double*>(buffer3);
                double n4 = *reinterpret_cast<double*>(buffer4);
                
                pid pid_constants;
                pid_constants.kP = n1;
                pid_constants.kI = n2;
                pid_constants.kD = n3;
                pid_constants.I_max = n4;
                
                status = Motors::motor_array.at(motor_number)->set_pid(pid_constants);
            }
            break;
            
        case 45239: {  //0xB0 0xB7  Reverse Motor
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                int reveresed = std::stoi(request.msg);
                status = Motors::motor_array.at(motor_number)->reverse_motor();
            }
            break;
        case 45240: {  //0xB0 0xB8  Set Log Level
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                int new_log_level = std::stoi(request.msg);
                Motors::motor_array.at(motor_number)->set_log_level(new_log_level);
                status = 1;
            }
            break;
        case 45241: {  //0xB0 0xB9  Set Slew enabled/disabled
                int motor_number = std::stoi(std::to_string(request.msg.at(0)));
                request.msg.erase(0);
                int enabled = std::stoi(request.msg);
                if(enabled) {
                    Motors::motor_array.at(motor_number)->enable_slew();
                } else {
                    Motors::motor_array.at(motor_number)->disable_slew();
                }
                status = 1;
            }
            break;
            
            
    // motor interaction get cases
        case 41120: {  // 0xA0 0xA0  Actual Velocity
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_actual_velocity());
            }
            break;
            
        case 41121: {  // 0xA0 0xA1  Actual Voltage
                int motor_number = request.msg.at(0) - 48;  // i think this is to convert from ascii char to integer
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_actual_voltage());
            }
            break;
            
        case 41122: {  // 0xA0 0xA2  Current Draw
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_current_draw());
            }
            break;
            
        case 41123: {  // 0xA0 0xA3  Encoder Position
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_encoder_position());
            }
            break;
            
        case 41124: {  // 0xA0 0xA4  Brakemode
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_brake_mode());
            }
            break;
            
        case 41125: {  // 0xA0 0xA5  Gearset
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_gearset());
            }
            break;
            
        case 41126: {  // 0xA0 0xA6  Port
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_port());
            }
            break;
            
        case 41127: {  // 0xA0 0xA7  PID Constants
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body += std::to_string(Motors::motor_array.at(motor_number)->get_pid().kP);
                return_msg_body += " " + std::to_string(Motors::motor_array.at(motor_number)->get_pid().kI);
                return_msg_body += " " + std::to_string(Motors::motor_array.at(motor_number)->get_pid().kD);
                return_msg_body += " " + std::to_string(Motors::motor_array.at(motor_number)->get_pid().I_max);
            }
            break;
            
        case 41128: {  // 0xA0 0xA8  Slew Rate
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_slew_rate());
            }
            break;
            
        case 41129: {  // 0xA0 0xA9  Power
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_power());
            }
            break;
            
        case 41130: {  // 0xA0 0xAA  Temperature
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_temperature());
            }
            break;
            
        case 41131: {  // 0xA0 0xAB  Torque
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_torque());
            }
            break;
            
        case 41132: {  // 0xA0 0xAC  Direction
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_direction());
            }
            break;
            
        case 41133: {  // 0xA0 0xAD  Efficiency
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->get_efficiency());
            }
            break;
            
        case 41134: {  // 0xA0 0xAE  is stopped
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->is_stopped());
            }
            break;
            
        case 41135: {  // 0xA0 0xAF  is reversed
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
                return_msg_body = std::to_string(Motors::motor_array.at(motor_number)->is_reversed());
            }
            break;
            
        case 41376: {  // 0xA1 0xA0  is registered
                int motor_number = request.msg.at(0) - 48;
                request.msg.erase(0);    
                
                status = 1;
            
                MotorThread* motor_thread = MotorThread::get_instance();
                return_msg_body = std::to_string(motor_thread->is_registered(*Motors::motor_array.at(motor_number)));
            }
            break;
        
        // encoder interaction post cases
        // encoder iteraction get cases
        
        // analog in sensor interaction post cases
        // analog in sensor interaction get cases
        
        // imu interaction post cases
        // imu interaction get cases
        
        // position tracker post cases
        // position tracker get cases
        
        // sd card interaction post cases
        
        // misc 
        case 43936:  // 0xAB 0xA0  debug
            status = 1;
            return_msg_body = " debug msg received: " + request.msg;
            break;
            
        case 43937:  // 0xAB 0xA1  init server
            status = 1;
            pros::c::serctl(SERCTL_DISABLE_COBS, NULL);
            set_server_task_priority(TASK_PRIORITY_DEFAULT);  // more messages are sure to follow so give read task more CPU time
            delay = 10; // lower delay because of expected messages
            return_msg_body = "server is running";
            break;
            
        case 43938:  // 0xAB 0xA2  shutdown server
            status = 1;
            pros::c::serctl(SERCTL_ENABLE_COBS, NULL);
            set_server_task_priority(2);
            delay = 100;
            return_msg_body = "server is no longer running";
            break;            
        
        default:
            status = 1;
            return_msg_body = " [INFO], " + std::to_string(pros::millis()) + ", Invalid Command: " + request.msg;
            break;
                    
        
    }
    
    return_msg.push_back(return_msg_body.length() + 2);
    return_msg.push_back((char)(request.return_id >> 8) & 0xFF);
    return_msg.push_back((char)request.return_id & 0xFF);
    return_msg += return_msg_body;
    // return_msg += std::to_string(pros::millis());
    return_msg.push_back('\xC6');
    
    entry.content = return_msg;
    
    logger.add(entry);
    
    return 1;
}




void Server::start_server() {
    read_thread->resume();
}

void Server::stop_server() {
    read_thread->suspend();
}

void Server::set_server_task_priority(int new_prio) {
    read_thread->set_priority(new_prio);
}

void Server::set_debug_mode(bool debug_mode) {
    debug = debug_mode;
}

void Server::clear_stdin() {
    fflush(stdin);
    std::cin.clear();
}



int Server::handle_requests(int max_requests) {
    std::vector<server_request> requests;
    
    if ( !request_queue.empty() ) {
        while ( lock.exchange( true ) ); //aquire lock
        for(int i=0; i<max_requests; i++) {
            if ( !request_queue.empty() ) {
                server_request request = request_queue.front();
                request_queue.pop();
                
                requests.push_back(request);
            }
        }    
        lock.exchange( false ); //release lock
    }
    
    for (int i=0; i<requests.size(); i++) {
        handle_request(requests.at(i));
    }
    
    return requests.size();
}
