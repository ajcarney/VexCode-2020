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



BallDetector::BallDetector(char line_sensor_port_top_port, char ball_detector_filter_port, char line_sensor_port_bottom_port, char vision_port, int detector_threshold) {
    ball_detector_top.set_port(line_sensor_port_top_port);
    ball_detector_filter.set_port(ball_detector_filter_port);
    ball_detector_bottom.set_port(line_sensor_port_bottom_port);
    vision_sensor = new pros::Vision(vision_port);

    threshold = detector_threshold;

    time_since_last_ball = 0;

    pros::vision_signature_s_t RED_SIG = pros::Vision::signature_from_utility(2, 1917, 5595, 3756, -211, 1591, 690, 1.000, 0);
    vision_sensor->set_signature(1, &RED_SIG);

    pros::vision_signature_s_t BLUE_SIG = pros::Vision::signature_from_utility(1, -2449, -825, -1638, 5173, 9899, 7536, 1.500, 0);
    vision_sensor->set_signature(2, &BLUE_SIG);

}


BallDetector::~BallDetector() {
    delete vision_sensor;
}


int BallDetector::set_threshold(int new_threshold) {
    threshold = new_threshold;
    return 1;
}


int BallDetector::check_filter_level() {
    if(ball_detector_filter.get_raw_value() < threshold) {  // ball is detected
        time_since_last_ball = 0;  // ball detected so there is no time since last ball

        pros::vision_object_s_t red = vision_sensor->get_by_sig(0, 1);
        pros::vision_object_s_t blue = vision_sensor->get_by_sig(0, 2);
        std::cout << red.signature << " " << blue.signature << "\n";
        if(red.signature == 255) {
            vision_sensor->set_led(0x4287f5);  // blue
            // std::cout << "color is not red, color is blue\n";
            return 1;
        } else if(blue.signature == 255) {
            vision_sensor->set_led(0xf7070f);  // red
            // std::cout << "color is not blue, color is red\n";
            return 2;
        } else {
            vision_sensor->set_led(0xf2ff00);  // yellow for error
            return -1;
        }
    }
    vision_sensor->set_led(0x4cbb17);  // kelly green
    
    time_since_last_ball = pros::millis() - time_since_last_ball;  // get time elapsed
    return 0;  // no ball is detected
}


std::vector<bool> BallDetector::locate_balls() {
    std::vector<bool> locations;
    if(ball_detector_top.get_raw_value() < threshold) {
        locations.push_back(true);
    } else {
        locations.push_back(false);
    }
    
    if(ball_detector_filter.get_raw_value() < threshold) {
        locations.push_back(true);
    } else {
        locations.push_back(false);
    }
    
    if(ball_detector_bottom.get_raw_value() < threshold) {
        locations.push_back(true);
    } else {
        locations.push_back(false);
    }
    
    return locations;
}

std::tuple<int, int> BallDetector::debug_color() {
    return std::make_tuple(ball_detector_filter.get_raw_value(), check_filter_level());
}
