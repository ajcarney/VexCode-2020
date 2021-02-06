/**
 * @file: ./RobotCode/src/objects/sensors/BallDetector.cpp
 * @author: Aiden Carney
 * @reviewed_on:
 * @reviewed_by:
 *
 * contains implementation for ball detector class
 */

#include "../serial/Logger.hpp"
#include "BallDetector.hpp"



BallDetector::BallDetector(
    AnalogInSensor& detector_top_left, 
    AnalogInSensor& detector_filter, 
    AnalogInSensor& detector_bottom, 
    int optical_port, 
    int detector_threshold
) {
    ball_detector_top = detector_top_left;
    ball_detector_filter = detector_filter;
    ball_detector_bottom = detector_bottom;
    optical_sensor = new pros::Optical(optical_port);
    
    optical_sensor->disable_gesture();
    optical_sensor->set_led_pwm(50);
    
    threshold = detector_threshold;
    time_since_last_ball = 0;
    log_data = false;
}

BallDetector::~BallDetector() {
    delete optical_sensor;
}


int BallDetector::set_threshold(int new_threshold) {
    threshold = new_threshold;
    return 1;
}


int BallDetector::check_filter_level() {
    int return_code = 0;
    if(ball_detector_filter.get_raw_value() < threshold) {  // ball is detected
        time_since_last_ball = 0;  // ball detected so there is no time since last ball

        double hue = optical_sensor->get_hue();
        if(hue > 170 && hue < 260) {  // color is blue
            return_code = 1;
        } else if(hue > 335 || hue < 25) {  // color is red
            return_code = 2;
        } else {  // could not determine color based on ranges
            return_code = -1;
        }
    } else {
        time_since_last_ball = pros::millis() - time_since_last_ball;  // get time elapsed
    }
    
    if(log_data) {
        Logger logger;
        log_entry entry;
        entry.content = (
            "[INFO] " + std::string("BALL_DETECT_MIDDLE")
            + ", Time: " + std::to_string(pros::millis())
            + ", ball_detected: " + std::to_string(return_code)
            + ", time_since_last_ball " + std::to_string(time_since_last_ball)
            + ", line_detector: " + std::to_string(ball_detector_filter.get_raw_value())
            + ", threshold: " + std::to_string(threshold)
            
        );
        entry.stream = "clog";
        logger.add(entry);  
    }
    
    
    return return_code;  // no ball is detected
}


std::vector<bool> BallDetector::locate_balls() {
    std::vector<bool> locations;
    if(ball_detector_top.get_raw_value() < 2850) {
        locations.push_back(true);
    } else {
        locations.push_back(false);
    }
    
    if(ball_detector_filter.get_raw_value() < 2860) {
        locations.push_back(true);
    } else {
        locations.push_back(false);
    }
    
    if(ball_detector_bottom.get_raw_value() < threshold) {
        locations.push_back(true);
    } else {
        locations.push_back(false);
    }
    
    if(log_data) {
        Logger logger;
        log_entry entry;
        entry.content = (
            "[INFO] " + std::string("BALL_DETECT_MIDDLE")
            + ", time: " + std::to_string(pros::millis())
            + ", top_present: " + std::to_string(locations.at(0))
            + ", middle_present: " + std::to_string(locations.at(1))
            + ", bottom_present: " + std::to_string(locations.at(2))
            + ", top: " + std::to_string(ball_detector_top.get_raw_value())
            + ", middle: " + std::to_string(ball_detector_filter.get_raw_value())
            + ", bottom: " + std::to_string(ball_detector_bottom.get_raw_value())
            + ", threshold: " + std::to_string(threshold)
        );
        entry.stream = "clog";
        logger.add(entry);  
    }
    
    
    return locations;
}


void BallDetector::set_led_brightness(int pct) {
    optical_sensor->set_led_pwm(pct);
}


void BallDetector::auto_set_led_brightness() {
    int pct = 100 * std::abs(1 - optical_sensor->get_brightness());  // 1 to 1 scale
    optical_sensor->set_led_pwm(pct);
}


std::tuple<int, int> BallDetector::debug_color() {
    return std::make_tuple(ball_detector_filter.get_raw_value(), check_filter_level());
}

void BallDetector::start_logging() {
    log_data = true;
}

void BallDetector::stop_logging() {
    log_data = false;
}
