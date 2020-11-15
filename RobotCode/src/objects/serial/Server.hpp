/**
 * @file: ./RobotCode/src/objects/serial/Server.hpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by: 
 *
 * contains class for a server that works over serial communication
 */

#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <atomic>
#include <queue>
#include <cstdint>


typedef struct
{
    uint16_t return_id;
    uint16_t command_id;
    std::string msg;
} server_request;

class Server
{
    private:
        static std::atomic<bool> lock;
        static std::queue<server_request> request_queue;
        
        static pros::Task *read_thread;  // the thread for reading stdin
        
        static void read_stdin(void*);
        
        static int num_instances;
        static bool debug;
        
        static int delay;
        
        int handle_request(server_request request);
        
    public:
        Server();
        ~Server();
        
        /**
         * @return: None
         *
         * starts the thread or resmes it if it was stopped
         */
        void start_server();
        
        /**
         * @return: None
         *
         * stops the thread from being scheduled
         */
        void stop_server();
        
        void set_server_task_priority(int new_prio);
        
        void set_debug_mode(bool debug_mode);
        
        void clear_stdin();
        
        int handle_requests(int max_requests=10);
};
        
        
#endif
