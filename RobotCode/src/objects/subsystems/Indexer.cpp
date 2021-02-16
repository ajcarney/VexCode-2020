/**
 * @file: ./RobotCode/src/objects/subsystems/Indexer.cpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * Contains implementation for the differential subsystem
 * has methods for brake and indexing
 */

#include "main.h"


#include "../serial/Logger.hpp"
#include "../sensors/BallDetector.hpp"
#include "Indexer.hpp"

int Indexer::num_instances = 0;
std::queue<indexer_action> Indexer::command_queue;
std::vector<int> Indexer::commands_finished;
std::atomic<bool> Indexer::command_start_lock = ATOMIC_VAR_INIT(false);
std::atomic<bool> Indexer::command_finish_lock = ATOMIC_VAR_INIT(false);

Motor* Indexer::upper_indexer;
Motor* Indexer::lower_indexer;
BallDetector* Indexer::ball_detector;
std::string Indexer::filter_color;


Indexer::Indexer(Motor &upper, Motor &lower, BallDetector &detector, std::string color)
{    
    upper_indexer = &upper;
    lower_indexer = &lower;
    ball_detector = &detector;
    filter_color = color;

    upper_indexer->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    lower_indexer->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    
    upper_indexer->set_motor_mode(e_voltage);
    lower_indexer->set_motor_mode(e_voltage);
    
    upper_indexer->disable_slew();
    lower_indexer->disable_slew();
    
    if(num_instances == 0 || thread == NULL) {
        thread = new pros::Task( indexer_motion_task, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "indexer_thread");
    }
    
    num_instances += 1;
}


Indexer::~Indexer() {
    num_instances -= 1;
    if(num_instances == 0) {
        delete thread;
    }
}




bool Indexer::auto_filter_ball() {
    int color = ball_detector->check_filter_level();
    if((color == 1 && filter_color == "blue") || (color == 2 && filter_color == "red")) {  // ball should be filtered
        upper_indexer->set_voltage(-12000); 
        lower_indexer->set_voltage(12000);
        pros::delay(300);  // let ball filter out
        upper_indexer->set_voltage(0); 
        lower_indexer->set_voltage(0);
        
        return true;  // did filter out ball
    } else if(color < 0) {  // ball was detected but color could not be determined: print error message and default to intaking
        Logger logger;
        log_entry entry;
        entry.content = "[ERROR], " + std::to_string(pros::millis()) + ", ball was detected but color could not be determined";
        entry.stream = "cerr";
        logger.add(entry);
    }
    
    return false;  // did not filter out ball
}




void Indexer::indexer_motion_task(void*) {
    int end_of_run_time = pros::millis();
    int start_of_run_time = pros::millis();
    bool stagger_state = false;
    bool prev_stagger_state = false;

    while(1) {
        if(command_queue.empty()) {  // delay unitl there is a command in the queue
            pros::delay(5);
            continue;
        }
        
        // take lock and get command
        while ( command_start_lock.exchange( true ) ); //aquire lock
        indexer_action action = command_queue.front();
        command_queue.pop();
        command_start_lock.exchange( false ); //release lock
                    
        // execute command
        switch(action.command) {
            case e_filter: {
                upper_indexer->set_voltage(-12000); 
                lower_indexer->set_voltage(12000);
                break;
            } case e_auto_index: {
                auto_filter_ball();     
                // fallthrough and index like normal now that it doesn't need to filter
            } case e_index: {
                upper_indexer->set_voltage(12000); 
                lower_indexer->set_voltage(12000);
                break;
            } case e_index_no_backboard: {
                upper_indexer->set_voltage(9000); 
                lower_indexer->set_voltage(12000);   
                break;
            } case e_auto_staggered_index: {
                auto_filter_ball();     
                // fallthrough and index staggered like normal now that it doesn't need to filter
            } case e_staggered_index: {
                std::cout << end_of_run_time << " " <<  start_of_run_time << " " << pros::millis() << "\n";
                std::vector<bool> locations = ball_detector->locate_balls();
                
                if(!locations.at(0)) {  // bring ball to top if it is not there
                    upper_indexer->set_voltage(9000); 
                    lower_indexer->set_voltage(9000);
                } else if(pros::millis() - end_of_run_time > 300 && pros::millis() - start_of_run_time < 300) {  // 300ms between indexing, index for 300ms
                    std::cout << "here\n";
                    prev_stagger_state = stagger_state;
                    stagger_state = true;
                    if(prev_stagger_state != stagger_state) {  // start time of indexing occurs when state switches
                        start_of_run_time = pros::millis();
                    }
                    
                    upper_indexer->set_voltage(12000); 
                    lower_indexer->set_voltage(12000); 
                } else {
                    std::cout << "here2\n";
                    prev_stagger_state = stagger_state;
                    stagger_state = false;
                    if(prev_stagger_state != stagger_state) {  // end of indexing occurs when state switches
                        end_of_run_time = pros::millis();
                    }
                    start_of_run_time = pros::millis();  // this is 0 so above condition is met only based on the time in between
                    
                    upper_indexer->set_voltage(0); 
                    lower_indexer->set_voltage(0); 
                }
                break;
            } case e_index_until_filtered: {
                upper_indexer->set_voltage(12000); 
                lower_indexer->set_voltage(12000);
                
                bool filtered = false;
                do {
                    filtered = auto_filter_ball();
                } while(!filtered);
                
                break;
            } case e_index_to_state: {
                ball_positions current_state = get_state();
                do {
                    current_state = get_state();
                    int color = ball_detector->check_filter_level();
                    if(action.args.allow_filter) {
                        auto_filter_ball();  // attempt to filter
                    } 
                    
                    if(current_state.top != action.args.end_state.top) {
                        upper_indexer->set_voltage(12000); 
                    }
                    
                    if(current_state.middle != action.args.end_state.middle) {
                        lower_indexer->set_voltage(12000);   
                    }
                } while(current_state != action.args.end_state);
                
                break;
            } case e_auto_increment: {
                // try to filter out ball at second level if necessary
                auto_filter_ball();
                // fall through if there is nothing to filter out
            } case e_increment: {
                std::vector<bool> locations = ball_detector->locate_balls();
                
                if(!locations.at(0)) {  // move ball into top position
                    upper_indexer->set_voltage(10000); 
                    lower_indexer->set_voltage(10000);
                } else if(locations.at(0) && !locations.at(1)) { // move ball from lowest/no position to middle position
                    upper_indexer->set_voltage(0); 
                    lower_indexer->set_voltage(10000);
                } else { // indexer can't do anything to increment so don't run
                    upper_indexer->set_voltage(0); 
                    lower_indexer->set_voltage(0);
                }
                break;
            } case e_fix_ball: {
                upper_indexer->set_voltage(-12000);
                pros::delay(250);
                upper_indexer->set_voltage(12000);
                pros::delay(500);
                upper_indexer->set_voltage(0);
                break;
            } case e_run_upper: {
                upper_indexer->set_voltage(12000);
                break;
            } case e_run_lower: {
                lower_indexer->set_voltage(12000);
                break;
            } case e_run_lower_reverse: {
                lower_indexer->set_voltage(-12000);
                break;
            } case e_run_upper_reverse: {
                upper_indexer->set_voltage(-12000);
                break;
            } case e_stop: {
                lower_indexer->set_voltage(0);
                upper_indexer->set_voltage(0);
                break;
            }
        }
        
        if(action.command == e_index_until_filtered || action.command == e_index_to_state || action.command == e_fix_ball) {
            while ( command_finish_lock.exchange( true ) ); //aquire lock
            commands_finished.push_back(action.uid);
            command_finish_lock.exchange( false ); //release lock
        }
    }
}

int Indexer::send_command(indexer_command command, indexer_args args /*{}*/) {
    while ( command_start_lock.exchange( true ) ); //aquire lock
    indexer_action action;
    action.command = command;
    action.args = args;
    action.uid = pros::millis() + lower_indexer->get_actual_voltage() + upper_indexer->get_actual_voltage();
    command_queue.push(action);
    command_start_lock.exchange( false ); //release lock
    
    return action.uid;
}

void Indexer::index() {
    send_command(e_index);
}

void Indexer::filter() {
    send_command(e_filter);
}

void Indexer::auto_index() {
    send_command(e_auto_index);
}

void Indexer::index_no_backboard() {
    send_command(e_index_no_backboard);
}

int Indexer::index_until_filtered(bool asynch /*false*/) {
    int uid = send_command(e_index_until_filtered);
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}

int Indexer::index_to_state(bool allow_filter, ball_positions end_state, bool asynch) {
    indexer_args args;
    args.allow_filter = allow_filter;
    args.end_state = end_state;
    int uid = send_command(e_index_to_state, args);
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}

void Indexer::increment() {
    send_command(e_increment);
}

void Indexer::auto_increment() {
    send_command(e_auto_increment);
}


void Indexer::auto_staggered_index() {
    send_command(e_auto_staggered_index);
}


void Indexer::staggered_index() {
    send_command(e_staggered_index);
}




void Indexer::run_upper_roller() {
    send_command(e_run_upper);
}


void Indexer::run_lower_roller() {
    send_command(e_run_lower); 
}

void Indexer::run_lower_roller_reverse() {
    send_command(e_run_lower_reverse);
}

void Indexer::run_upper_roller_reverse() {
    send_command(e_run_upper_reverse);
}




int Indexer::fix_ball(bool asynch /*true*/) {
    int uid = send_command(e_fix_ball);
    
    if(!asynch) {
        wait_until_finished(uid);
    }
    
    return uid;
}



void Indexer::hard_stop() {
    reset_command_queue();
    send_command(e_stop);
}

void Indexer::stop() {
    send_command(e_stop);
}

ball_positions Indexer::get_state() {
    ball_positions state;
    
    int color = ball_detector->check_filter_level();
    std::vector<bool> ball_locations = ball_detector->locate_balls();
    if(ball_locations.at(0)) {
        state.top = true;
    } else {
        state.top = false;
    }
    if(ball_locations.at(1)) {
        state.middle = true;
    } else {
        state.middle = false;
    }
    
    if (color == 0) {
        state.middle_color = "none";
    } else if(color == 1) {
        state.middle_color = "blue";
    } else if (color == 2) {
        state.middle_color = "red";
    } else {
        state.middle_color = "unknown";
    }
    
    return state;
}

void Indexer::reset_command_queue() {
    while ( command_start_lock.exchange( true ) ); //aquire lock
    std::queue<indexer_action> empty_queue;
    std::swap( command_queue, empty_queue );  // replace command queue with an empty queue
    command_start_lock.exchange( false ); //release lock    
}


void Indexer::update_filter_color(std::string new_color) {
    filter_color = new_color;
}


void Indexer::wait_until_finished(int uid) {
    while(std::find(commands_finished.begin(), commands_finished.end(), uid) == commands_finished.end()) {
        pros::delay(10);
    }
    while ( command_finish_lock.exchange( true ) ); //aquire lock
    commands_finished.erase(std::remove(commands_finished.begin(), commands_finished.end(), uid), commands_finished.end()); 
    command_finish_lock.exchange( false ); //release lock
}


bool Indexer::is_finished(int uid) {
    if(std::find(commands_finished.begin(), commands_finished.end(), uid) == commands_finished.end()) {
        return false;  // command is not finished because it is not in the list
    }
    
    while ( command_finish_lock.exchange( true ) ); //aquire lock
    commands_finished.erase(std::remove(commands_finished.begin(), commands_finished.end(), uid), commands_finished.end()); 
    command_finish_lock.exchange( false ); //release lock
    
    return true;
}
