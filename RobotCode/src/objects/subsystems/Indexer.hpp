/**
 * @file: ./RobotCode/src/objects/subsystems/Indexer.hpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * Contains class for the differential subsystem
 * has methods for brake and indexing
 */

#ifndef __INDEXER_HPP__
#define __INDEXER_HPP__

#include <tuple>
#include <queue>

#include "main.h"

#include "../motors/Motor.hpp"
#include "../sensors/Sensors.hpp"
#include "../sensors/BallDetector.hpp"


typedef enum e_indexer_command {
    e_index,
    e_filter,
    e_auto_index,
    e_index_no_backboard,
    e_index_until_filtered,
    e_increment,
    e_auto_increment,
    e_raise_brake,
    e_lower_brake,
    e_fix_ball,
    e_run_upper,
    e_run_lower,
    e_stop
} indexer_command;

typedef struct {
    int uid;
    indexer_command command;
} indexer_action;

typedef struct {
    bool top;
    bool middle;
    std::string middle_color;
} ball_positions;


/**
 * @see: Motors.hpp
 *
 * contains methods to allow for control of the indexer
 */
class Indexer
{
    private:
        static Motor *upper_indexer;
        static Motor *lower_indexer;
        static BallDetector *ball_detector;
        static AnalogInSensor *potentiometer;
        static std::string filter_color;
        
        static int num_instances;
                
        pros::Task *thread;  // the motor thread
        static std::queue<indexer_action> command_queue;
        static std::vector<int> commands_finished;
        static std::atomic<bool> command_start_lock;
        static std::atomic<bool> command_finish_lock;
        
        int send_command(indexer_command command);

        static void indexer_motion_task(void*);
                
    public:
        Indexer(Motor &upper, Motor &lower, BallDetector &detector, AnalogInSensor &pot, std::string color);
        ~Indexer();
    
        void index();
        void filter();
        void auto_index();
        void index_no_backboard();
        void index_until_filtered(bool asynch=false);
        
        void increment();
        void auto_increment();
        
        void lower_brake();
        void raise_brake();
        
        void run_upper_roller();
        void run_lower_roller();
        
        void fix_ball();
        
        void stop();

        ball_positions get_state();

        void reset_command_queue();
        void update_filter_color(std::string new_color);
        
        void wait_until_finished(int uid);
        bool is_finished(int uid);

        
};




#endif
