/**
 * @file: ./RobotCode/src/Autons.cpp
 * @author: Aiden Carney
 * @reviewed_on: 12/5/19
 * @reviewed_by: Aiden Carney
 *
 * @see: Autons.hpp
 *
 * contains implementation for autonomous options
 */

#include <unordered_map>

#include "main.h"

#include "Autons.hpp"
#include "objects/motors/Motors.hpp"
#include "objects/motors/MotorThread.hpp"
#include "objects/position_tracking/PositionTracker.hpp"
#include "objects/subsystems/chassis.hpp"
#include "objects/subsystems/Indexer.hpp"
#include "objects/subsystems/intakes.hpp"
#include "objects/lcdCode/DriverControl/AutonomousLCD.hpp"


int Autons::selected_number = 1;

Autons::Autons( )
{
    debug_auton_num = 7;
    driver_control_num = 1;
}



Autons::~Autons( ) {

}

void Autons::set_autonomous_number(int n) {
    selected_number = n;
}

int Autons::get_autonomous_number() {
    return selected_number;
}

/**
 * deploys by outtaking and running top roller
 */
void Autons::deploy() {
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, Sensors::potentiometer, "none");
    Intakes intakes(Motors::left_intake, Motors::right_intake);

    intakes.rocket_outwards();
    indexer.run_upper_roller();
    
    pros::delay(1000);
    
    intakes.stop();
    indexer.stop();
}


/**
 * drives forward to score in the zone, then drive backward
 * to stop touching the cube
 */
void Autons::one_pt() {

    deploy();
}




/**
 * runs unit test
 * 180 degree, 90 degree, 45 degree, 45 degree
 * tilter movement
 * straight drive moving
 */
void Autons::skills() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, Sensors::imu, 16, 5/3);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, Sensors::potentiometer, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});
    
    deploy();
    
    // int uid = chassis.drive_to_point(0, 15, 0, 1, 150, INT32_MAX, true);
    // while(!chassis.is_finished(uid)) {
    //     std::cout << uid << " " << chassis.is_finished(uid) << "\n";
    //     intakes.intake();
    //     indexer.auto_increment();
    // }
    // intakes.stop();
    // indexer.stop();
    // 
    // chassis.drive_to_point(15, 15, 0, 0, 150, INT32_MAX, false);
    // chassis.pid_straight_drive(400);
}



void Autons::blue_north() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, Sensors::imu, 16, 5/3);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, Sensors::potentiometer, "blue");    
    indexer.update_filter_color("red");
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});
    
    deploy();    
    
    chassis.drive_to_point(0, 27);
    
    chassis.drive_to_point(27.2, 5.5, 0, 1, 125, INT32_MAX, false);

    chassis.turn_right(51);
    chassis.pid_straight_drive(235);

    // chassis.pid_straight_drive(200, 0, 80, INT32_MAX, false, false);

    for(int i=0; i < 50; i++) {  // score preload
        indexer.auto_index();
        pros::delay(10);
    }
    
    chassis.drive_to_point(0, 27);
    
    intakes.hold_outward();
    
    chassis.drive_to_point(-17.7, 9.7, 0, 1, 100, INT32_MAX, false);
    // chassis.pid_straight_drive(1100, 0, 150, INT32_MAX, false, false);
    
    int uid = chassis.pid_straight_drive(450, 0, 80, 2000, true, false);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
    }
    
    // intakes.intake();
    // indexer.index_until_filtered();
    // pros::delay(100);
    // intakes.stop();
    // 
    for(int i=0; i < 100; i++) {  // index a little bit longer
        indexer.auto_index();
        pros::delay(10);
    }
    
    chassis.pid_straight_drive(-500, 0, 200, INT32_MAX, false, false);
}


void Autons::blue_north_2() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, Sensors::imu, 16, 5/3);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, Sensors::potentiometer, "blue");    
    indexer.update_filter_color("blue");
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});
    
    deploy();  
    
    chassis.pid_straight_drive(560, 0, 100);
    chassis.turn_left(136, 100, 5000);
    
    intakes.hold_outward();
    // chassis.pid_straight_drive(300, 0, 80, 2000, false, true);
    
    
    intakes.intake();
    indexer.auto_increment();
    chassis.pid_straight_drive(900, 0, 80, 2000, false, false);
    indexer.stop();
    // indexer.index_until_filtered();
    intakes.stop();
    
    for(int i=0; i < 150; i++) {  // index a little bit longer
        // intakes.intake();
        indexer.index_no_backboard();
        pros::delay(10);
    }
    
    // 
    // for(int i=0; i < 100; i++) {  // index a little bit longer
    //     // intakes.intake();
    //     indexer.index();
    //     pros::delay(10);
    // }
    
    indexer.fix_ball();
    pros::delay(1000);
    indexer.stop();
    for(int i=0; i < 40; i++) {  // index a little bit longer
        intakes.intake();
        // indexer.auto_index();
        pros::delay(10);
    }
    intakes.stop();
    
    intakes.hold_outward();
    chassis.pid_straight_drive(-400, 0, 80);
}


void Autons::red_north() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, Sensors::imu, 16, 5/3);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, Sensors::potentiometer, "blue");    
    indexer.update_filter_color("blue");
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});
    
    deploy();  
    
    chassis.pid_straight_drive(560, 0, 100);
    chassis.turn_right(136, 100, 5000);
    
    intakes.hold_outward();
    // chassis.pid_straight_drive(300, 0, 80, 2000, false, true);
    
    
    intakes.intake();
    indexer.auto_increment();
    chassis.pid_straight_drive(900, 0, 80, 2000, false, false);
    indexer.stop();
    // indexer.index_until_filtered();
    intakes.stop();
    
    for(int i=0; i < 150; i++) {  // index a little bit longer
        // intakes.intake();
        indexer.index_no_backboard();
        pros::delay(10);
    }
    
    // 
    // for(int i=0; i < 100; i++) {  // index a little bit longer
    //     // intakes.intake();
    //     indexer.index();
    //     pros::delay(10);
    // }
    
    indexer.fix_ball();
    pros::delay(1000);
    indexer.stop();
    for(int i=0; i < 40; i++) {  // index a little bit longer
        intakes.intake();
        // indexer.auto_index();
        pros::delay(10);
    }
    intakes.stop();
    
    intakes.hold_outward();
    chassis.pid_straight_drive(-400, 0, 80);
}

void Autons::run_autonomous() {
    switch(selected_number)
    {
        case 1:
           break;

        case 2:
            one_pt();
            break;

        case 3:
            skills();
            break;
            
        case 4:
            blue_north();
            break;
            
        case 5:
            blue_north_2();
            break;
            
        case 6:
            red_north();

    }
}
