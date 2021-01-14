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
    debug_auton_num = 5;
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
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(5);
    tracker->set_position({0, 0, 0});
    
    deploy();    
    
    chassis.drive_to_point(0, 30);
    
    chassis.turn_right(125);
    
    chassis.pid_straight_drive(1100, 0, 150, INT32_MAX, false, false);
    
    for(int i=0; i < 100; i++) {  // score preload
        indexer.auto_index();
        pros::delay(10);
    }
    
    chassis.pid_straight_drive(-1100, 0, 150, INT32_MAX, false, false);
    
    chassis.turn_right(90);
    
    intakes.hold_outward();
    // chassis.pid_straight_drive(1100, 0, 150, INT32_MAX, false, false);
    
    int uid = chassis.pid_straight_drive(1300, 0, 150, INT32_MAX, true, false);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
    }
    // chassis.turn_to_point(24, 10.6, 0, 1);
    
    // chassis.profiled_straight_drive(1500);
    // chassis.turn_right(25);
    // chassis.pid_straight_drive(150);
    
    for(int i=0; i < 100; i++) {  // score preload
        indexer.auto_index();
        pros::delay(10);
    }
    
    // chassis.pid_straight_drive(-500);
    // 
    // chassis.drive_to_point(0, 30, 0, -1);
    // 
    // intakes.hold_outward();
    // chassis.drive_to_point(-24.5, 11.5);
    // // int uid = chassis.drive_to_point(-24.5, 11.5, 0, 1, 150, INT32_MAX, true);
    // // while(!chassis.is_finished(uid)) {
    // //     indexer.auto_increment();
    // // }
    // 
    // int uid = chassis.pid_straight_drive(150, 0, 130, INT32_MAX, true);  // drive to ball
    // while(!chassis.is_finished(uid)) {
    //     indexer.auto_increment();
    //     intakes.intake();
    // }
    // 
    // for(int i=0; i < 100; i++) {  // make sure to pick up ball
    //     indexer.auto_increment();
    //     intakes.intake();
    //     pros::delay(10);
    // }
    // 
    // // uid = chassis.turn_left(10, 130, INT32_MAX, true);  // turn to goal
    // // while(!chassis.is_finished(uid)) {
    // //     indexer.auto_increment();
    // //     intakes.intake();
    // // }
    // 
    // chassis.pid_straight_drive(350);  // drive into goal
    // 
    // for(int i=0; i < 200; i++) {  // score balls
    //     indexer.auto_index();
    //     pros::delay(10);
    // }
    // 
    // chassis.drive_to_point(0, 30, 0, -1);
    
    
    
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

    }
}
