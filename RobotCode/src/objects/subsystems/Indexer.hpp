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



struct ball_positions{
    bool top;
    bool middle;
    std::string middle_color;
    bool operator== (const ball_positions &state) {
        bool top_equal = (top == state.top);
        bool middle_equal = (middle == state.middle);
        bool middle_color_equal;
        if(middle_color == "present" && state.middle_color != "none") {
            middle_color_equal = true;
        } else if (middle_color == "any") {
            middle_color_equal = true;
        }else if (middle_color == "red" && state.middle_color == "red") {
            middle_color_equal = true;
        } else if (middle_color == "blue" && state.middle_color == "blue") {
            middle_color_equal = true;
        } else if (middle_color == "none" && state.middle_color == "none") {
            middle_color_equal = true;
        }
        
        return (top_equal && middle_equal && middle_color_equal);
    }
    bool operator!= (const ball_positions &state) {
        bool top_equal = (top == state.top);
        bool middle_equal = (middle == state.middle);
        bool middle_color_equal;
        if(middle_color == "any" && state.middle_color != "none") {
            middle_color_equal = true;
        } else if (middle_color == "red" && state.middle_color == "red") {
            middle_color_equal = true;
        } else if (middle_color == "blue" && state.middle_color == "blue") {
            middle_color_equal = true;
        } else if (middle_color == "none" && state.middle_color == "none") {
            middle_color_equal = true;
        }
        
        return !(top_equal && middle_equal && middle_color_equal);
    }
};

typedef enum e_indexer_command {
    e_index,
    e_filter,
    e_auto_index,
    e_index_no_backboard,
    e_auto_staggered_index,
    e_staggered_index,
    e_index_until_filtered,
    e_increment,
    e_auto_increment,
    e_index_to_state,
    e_fix_ball,
    e_run_upper,
    e_run_lower,
    e_run_lower_reverse,
    e_run_upper_reverse,
    e_stop
} indexer_command;

typedef struct {
    ball_positions end_state;
    bool allow_filter;
}indexer_args;

typedef struct {
    int uid;
    indexer_command command;
    indexer_args args;
} indexer_action;

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
        static std::string filter_color;
        
        static int num_instances;
                
        pros::Task *thread;  // the motor thread
        static std::queue<indexer_action> command_queue;
        static std::vector<int> commands_finished;
        static std::atomic<bool> command_start_lock;
        static std::atomic<bool> command_finish_lock;
        
        int send_command(indexer_command command, indexer_args args={});

        static bool auto_filter_ball();
        static void indexer_motion_task(void*);
                
    public:
        Indexer(Motor &upper, Motor &lower, BallDetector &detector, std::string color);
        ~Indexer();
    
        void index();
        void filter();
        void auto_index();
        void index_no_backboard();
        void auto_staggered_index();
        void staggered_index();
        int index_until_filtered(bool asynch=false);
        int index_to_state(bool allow_filter, ball_positions end_state, bool asynch=false);
        
        void increment();
        void auto_increment();
        
        void run_upper_roller();
        void run_lower_roller();
        void run_lower_roller_reverse();
        void run_upper_roller_reverse();
        
        int fix_ball(bool asynch=true);
        
        void hard_stop();
        void stop();

        static ball_positions get_state();

        void reset_command_queue();
        void update_filter_color(std::string new_color);
        
        void wait_until_finished(int uid);
        bool is_finished(int uid);

        
};




#endif
