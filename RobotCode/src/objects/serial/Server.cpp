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

#include "main.h"
#include "pros/apix.h"

#include "Server.hpp"
#include "Logger.hpp"

std::queue<server_request> Server::request_queue;
std::atomic<bool> Server::lock = ATOMIC_VAR_INIT(false);
pros::Task *Server::read_thread = NULL;
int Server::num_instances = 0;
bool Server::debug = false;


Server::Server() { 
    if(read_thread == NULL) {
        read_thread = new pros::Task( read_stdin, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "server_thread");
        read_thread->suspend();
        // pros::c::serctl(SERCTL_ACTIVATE, 0);
        // pros::c::serctl(SERCTL_DISABLE_COBS, NULL);
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
    
    while(1) {
        char byte = getchar();

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
            uint8_t return_msb = getchar();
            uint8_t return_lsb = getchar();
            uint8_t command_msb = getchar();
            uint8_t command_lsb = getchar();
                        
            uint16_t return_id = (return_msb << 8) | return_lsb;
            uint16_t command_id = (command_msb << 8) | command_lsb;

            for(int i=0; i<len_msg; i++) {  // read rest message
                msg.push_back(getchar());
            }

            char checksum = getchar();  // checksum is directly after end of message
            std::cout << "here6\n";
            pros::delay(10);
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
                while ( lock.exchange( true ) ); //aquire lock
                
                server_request request;
                request.return_id = return_id;
                request.command_id = command_id;
                request.msg = msg;
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
        pros::delay(10);        
    }
}



int Server::handle_request(server_request request) {
    // cases are defined in commands.ods
    Logger logger;
    log_entry entry;
    entry.stream = "clog";

    // std::string return_msg = "";
    std::string return_msg = (
        std::to_string('\xAA')
        + std::to_string('\x55')
        + std::to_string('\x1E')
    );
    
    std::string return_msg_body;

    switch(request.command_id) {
        // motor interaction post cases
        case 45232:  //0xB0 0xB0
            break;
        case 45233:  //0xB0 0xB1
            break;
        case 45234:  //0xB0 0xB2
            break;
        case 45235:  //0xB0 0xB3
            break;
        case 45236:  //0xB0 0xB4
            break;
        case 45237:  //0xB0 0xB5 
            break;
        case 45238:  //0xB0 0xB6
            break;
        case 45239:  //0xB0 0xB7
            break;
        case 45240:  //0xB0 0xB8
            break;
        case 45241:  //0xB0 0xB9
            break;
            
        // motor interaction post cases
        
        
        // encoder interaction post cases
        // encoder iteraction get cases
        
        // analog in sensor interaction post cases
        // analog in sensor interaction get cases
        
        // imu interaction post cases
        // imu interaction get cases
        
        // position tracker post cases
        // position tracker get cases
        
        // sd card interaction post cases
        
        default:
            return_msg_body = " [INFO], " + std::to_string(pros::millis()) + ", Invalid Command: " + request.msg;
            break;
                    
        
    }
    
    return_msg = (
        std::to_string(return_msg_body.length() + 2)
        + std::to_string(((request.return_id >> 8) & 0xFF))  // high byte
        + std::to_string((request.return_id & 0xFF))  // low byte
        + return_msg_body
        + std::to_string('\xC6')
    );
    
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

void Server::set_debug_mode(bool debug_mode) {
    debug = debug_mode;
}



int Server::handle_requests(int max_requests) {
    for(int i=0; i<max_requests; i++) {
        if ( !request_queue.empty() ) {
            while ( lock.exchange( true ) ); //aquire lock
            server_request request = request_queue.front();
            request_queue.pop();
            lock.exchange( false ); //release lock
            
            handle_request(request);
        }
    }
    
    return 1;
}
