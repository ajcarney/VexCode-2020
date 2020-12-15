/**
 * @file: ./RobotCode/src/objects/position_tracking/PositionTracker.cpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 * TODO:
 *
 * contains implementation for functions that track position
 */

#include <atomic>

#include "main.h"

#include "../serial/Logger.hpp"
#include "../sensors/Sensors.hpp"
#include "PositionTracker.hpp"


PositionTracker *PositionTracker::tracker_obj = NULL;
std::atomic<bool> PositionTracker::lock = ATOMIC_VAR_INIT(false);
position PositionTracker::current_position;

long double PositionTracker::initial_l_enc;
long double PositionTracker::initial_r_enc; 
long double PositionTracker::initial_theta;
long double PositionTracker::imu_offset;

long double PositionTracker::prev_l_enc;
long double PositionTracker::prev_r_enc;
long double PositionTracker::delta_theta_rad;

int PositionTracker::l_id = -1;  // -1 is used as an invalid id
int PositionTracker::r_id = -1;

bool PositionTracker::log_data = false;



PositionTracker::PositionTracker() {
    thread = new pros::Task( calc_position, (void*)NULL, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "position_tracking");
    current_position = {
        0,
        0,
        0
    };
    thread->suspend();
}


PositionTracker::~PositionTracker() {
    thread->remove();
    delete thread;
}


/**
 * inits object if object is not already initialized based on a static bool
 * sets bool if it is not set
 */
PositionTracker* PositionTracker::get_instance() {
    if ( tracker_obj == NULL )
    {
        tracker_obj = new PositionTracker;
    }
    return tracker_obj;
}




long double PositionTracker::to_inches( long double encoder_ticks, long double wheel_size ) {
    long double circumference = (wheel_size * M_PI);
    long double revolutions = encoder_ticks / 360.0;
    long double inches = circumference * revolutions;
    
    return inches;
}


long double PositionTracker::to_encoder_ticks(long double inches, long double wheel_size) {
    long double circumference = (wheel_size * M_PI);
    long double revolutions = inches / circumference;
    long double encoder_ticks = revolutions * 360;
    
    return encoder_ticks;
}


long double PositionTracker::to_degrees(long double radians) {
    return (radians * (M_PI/180));
}


long double PositionTracker::to_radians(long double degrees) {
    return (degrees * (180/M_PI));
}



void PositionTracker::calc_position(void*)
{
    Logger logger;
    log_entry entry;

    int s_id = Sensors::strafe_encoder.get_unique_id();

    prev_l_enc = std::get<0>(Sensors::get_average_encoders(l_id, r_id));
    prev_r_enc = std::get<1>(Sensors::get_average_encoders(l_id, r_id));
    long double prev_s_enc = Sensors::strafe_encoder.get_position(s_id);
    
    while(1)
    {
        long double l_enc = std::get<0>(Sensors::get_average_encoders(l_id, r_id));
        long double r_enc = std::get<1>(Sensors::get_average_encoders(l_id, r_id));
        long double s_enc = Sensors::strafe_encoder.get_position(s_id);
        std::cout << l_enc << " " << r_enc << " " << s_enc << "\n";
        long double delta_l_in = to_inches(l_enc - prev_l_enc, 3.25);  // calculate change in each encoder in inches
        long double delta_r_in = to_inches(r_enc - prev_r_enc, 3.25);
        long double delta_s_in = to_inches(s_enc - prev_s_enc, 3.25);
                
        prev_l_enc = l_enc;  // update previous encoder values
        prev_r_enc = r_enc;
        prev_s_enc = s_enc;
        
        // calculate total change in encoders
        long double delta_l_total = to_inches(l_enc, 3.25) - to_inches(initial_l_enc, 3.25);
        long double delta_r_total = to_inches(r_enc, 3.25) - to_inches(initial_r_enc, 3.25);

        // calculate absolute orientation
        long double new_abs_theta_rad = initial_theta + ((delta_l_total - delta_r_total) / (WHEEL_TRACK_L + WHEEL_TRACK_R)); // wheel track length
        // wrap angle to [-pi, pi]
        if(new_abs_theta_rad > M_PI) {
            new_abs_theta_rad -= M_PI;
        } else if(new_abs_theta_rad < -M_PI) {
            new_abs_theta_rad += M_PI;
        }
        
        // merge with imu
        long double imu_reading_rad = imu_offset + to_radians(Sensors::imu.get_heading());
        if(imu_reading_rad > M_PI) {  // ensure that imu is wrapped to [-pi,pi] when adding offset
            imu_reading_rad -= M_PI;
        } else if(imu_reading_rad < -M_PI) {
            imu_reading_rad += M_PI;
        }
        // make sure that imu_reading and theta from encoders have the same sign
        // to ensure that they are telling the same reading when merging
        // ie. imu = -359, enc = 1    == bad merge
        if(new_abs_theta_rad > 0 && imu_reading_rad < 0) {
            imu_reading_rad += M_PI;
        } else if(new_abs_theta_rad < 0 && imu_reading_rad > 0) {
            imu_reading_rad -= M_PI;
        }
        std::cout << "theta from imu: " << to_degrees(imu_reading_rad) << " | theta from encoders: " << to_degrees(new_abs_theta_rad) << "\n";
        
        new_abs_theta_rad = (0.0 * imu_reading_rad) + (1 * new_abs_theta_rad);
        
        
        // calculate the change in angle from the previous position
        delta_theta_rad = new_abs_theta_rad - current_position.theta;

        // calculate local offset
        long double delta_local_x;
        long double delta_local_y;
        if(std::abs(delta_theta_rad) < 0.000001) {
            delta_local_x = delta_s_in;
            delta_local_y = delta_r_in;  // note: delta_l == delta_r
        } else {
            delta_local_x = (2 * std::sin((delta_theta_rad / 2))) * ((delta_s_in / delta_theta_rad) + S_ENC_OFFSET);
            delta_local_y = (2 * std::sin((delta_theta_rad / 2))) * ((delta_r_in / delta_theta_rad) + WHEEL_TRACK_R);
        }
        
        // calculate average orientation for the cycle
        double avg_theta_rad = current_position.theta + (delta_theta_rad / 2);
        
        // calculate global change in coordinates as the change in the local offset 
        // rotated by -(avg_theta_rad)
        // Converts to polar coordinates, changes the angle, and converts back to cartesian
        long double radius_pol = std::sqrt((std::pow(delta_local_x, 2) + std::pow(delta_local_y, 2)));
        long double theta_pol = std::atan2(delta_local_y, delta_local_x);
        theta_pol = theta_pol - avg_theta_rad;
        long double delta_global_x = radius_pol * std::cos(theta_pol);
        long double delta_global_y = radius_pol * std::sin(theta_pol);
        
        
        // std::cout << delta_global_x << " " << delta_global_y << "\n";
        position new_position = {
            current_position.x_pos + delta_global_x,  // x position 
            current_position.y_pos + delta_global_y,  // y_position
            new_abs_theta_rad                         // new absolute orientation
        };
        
        while ( lock.exchange( true ) );
        current_position = new_position;
        lock.exchange(false);
        
        if(log_data)
        {
            entry.content = ("[INFO], " + std::string("Position Tracking Data")
                + ", Time: " + std::to_string(pros::millis())
                + ", X_POS: " + std::to_string(current_position.x_pos)
                + ", Y_POS: " + std::to_string(current_position.y_pos)
                + ", Angle: " + std::to_string(to_degrees(current_position.theta))
            );
            entry.stream = "clog";
            logger.add(entry);
            std::cout << entry.content << "\n";
        }
        
        pros::delay(5);
    }
}




void PositionTracker::start_thread()
{
    thread->resume();
}

void PositionTracker::stop_thread()
{
    thread->suspend();
}




void PositionTracker::start_logging()
{
    log_data = true;
}




void PositionTracker::stop_logging()
{
    log_data = false;
}



long double PositionTracker::get_delta_theta_rad() {
    return delta_theta_rad;
}

long double PositionTracker::get_heading_rad() {
    return current_position.theta;
}

position PositionTracker::get_position()
{
    return current_position;
}




void PositionTracker::set_position(position robot_coordinates)
{
    while ( lock.exchange( true ) );
    
    if(l_id != -1) {
        Sensors::left_encoder.forget_position(l_id);
    }
    if(r_id != -1) {
        Sensors::right_encoder.forget_position(r_id);
    }
    l_id = Sensors::left_encoder.get_unique_id();
    r_id = Sensors::right_encoder.get_unique_id();
    
    initial_l_enc = std::get<0>(Sensors::get_average_encoders(l_id, r_id));
    initial_r_enc = std::get<1>(Sensors::get_average_encoders(l_id, r_id));
    initial_theta = robot_coordinates.theta;
    
    imu_offset = initial_theta - to_radians(Sensors::imu.get_heading());  // offset + imu_reading = initial_theta
    
    prev_l_enc = initial_l_enc;
    prev_r_enc = initial_r_enc;
    
    delta_theta_rad = 0;

    current_position = robot_coordinates;
    
    lock.exchange(false);
}
