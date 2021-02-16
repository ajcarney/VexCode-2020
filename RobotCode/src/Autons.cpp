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
    debug_auton_num = 13;
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
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "none");
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
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});
    
    deploy();
    
    // tower one
    int uid = chassis.profiled_straight_drive(1000, 450, 3000, true);  // will sometimes error
    while(!chassis.is_finished(uid)) {
        intakes.intake();
        indexer.auto_increment();
        pros::delay(10);
    }

    intakes.stop();
    indexer.stop();
    
    chassis.turn_left(82, 450, 2500);
    
    chassis.profiled_straight_drive(1000, 450, 2750);
    
    indexer.index();
    pros::delay(2500);
    indexer.stop();
    
    // tower two
    chassis.profiled_straight_drive(-1000, 450, 2500);
    chassis.turn_right(107, 450, 3500);
    
    intakes.hold_outward();
    chassis.pid_straight_drive(1225, 0, 450, 3500);
    
    uid = chassis.pid_straight_drive(500, 0, 300, 3000, true);
    while(!chassis.is_finished(uid)) {
        intakes.intake();
        indexer.auto_increment();
        pros::delay(10);
    }
    
    intakes.stop();
    indexer.stop();
    
    chassis.turn_left(62, 450, 2000);
    
    chassis.pid_straight_drive(345, 0, 450, 2000);
    
    indexer.index();
    pros::delay(400);
    indexer.stop();
    
    pros::delay(500);
    
    indexer.index();
    pros::delay(400);
    indexer.stop();
    
    // tower 3 
    chassis.pid_straight_drive(-450, 0, 450, 2000);
    chassis.turn_right(78, 450, 2000);
    
    intakes.hold_outward();
    chassis.pid_straight_drive(1500, 0, 450, 3500);
    
    uid = chassis.pid_straight_drive(600, 0, 300, 3000, true);
    while(!chassis.is_finished(uid)) {
        intakes.intake();
        indexer.auto_increment();
        pros::delay(10);
    }
    
    
    chassis.turn_left(62, 450, 2000);
    
    chassis.pid_straight_drive(1175, 0, 450, 2000);
    
    indexer.index();
    pros::delay(400);
    indexer.stop();
    
    // 
    // chassis.drive_to_point(15, 15, 0, 0, 150, INT32_MAX, false);
    // chassis.pid_straight_drive(400);
}

void Autons::skills2() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});

    // tower 1
    int uid = chassis.turn_right(30, 600, 1250, true);
    indexer.run_upper_roller();
    while(!chassis.is_finished(uid)) {
        pros::delay(5);
    }

    uid = chassis.pid_straight_drive(400, 0, 600, 1000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        pros::delay(5);
    }
    indexer.hard_stop();

    indexer.index();
    pros::delay(1000);
    indexer.stop();
    
    // tower 2
    chassis.pid_straight_drive(-500, 0, 600, 1000);
    chassis.turn_right(181, 450, 1500);
    
    uid = chassis.pid_straight_drive(1500, 0, 500, 2000, true);  
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
    }
    intakes.stop();
    indexer.stop();
    
    chassis.turn_left(68, 450, 1500);
    chassis.pid_straight_drive(800, 0, 600, 1500);
    
    indexer.index();
    pros::delay(1000);
    indexer.stop();
    
    // tower 3 
    chassis.pid_straight_drive(-750, 0, 600, 1500);
    chassis.turn_right(136, 240, 1500);
    chassis.pid_straight_drive(1300, 0, 450, 2000);
    uid = chassis.pid_straight_drive(1700, 0, 500, 4250, true);  // will sometimes error
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
    }
    intakes.stop();
    indexer.stop();
    
    chassis.pid_straight_drive(1100, 0, 450, 1500);

    chassis.turn_left(47, 450, 1000);
    chassis.pid_straight_drive(780, 0, 450, 1500);
    
    indexer.reset_command_queue();
    indexer.index();
    pros::delay(1000);
    indexer.stop();
    
    // tower 4
    chassis.pid_straight_drive(-625, 0, 600, 1500);
    chassis.turn_right(131, 240, 1500);
    chassis.pid_straight_drive(1300, 0, 450, 2000);
    chassis.pid_straight_drive(700, 0, 450, 2000);
    uid = chassis.pid_straight_drive(1700, 0, 500, 4250, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
    }
    intakes.stop();
    indexer.stop();
    
    chassis.pid_straight_drive(300, 0, 450, 1500);

    chassis.turn_left(48, 450, 1000);
    chassis.pid_straight_drive(730, 0, 450, 1500);
    
    indexer.reset_command_queue();
    indexer.index();
    pros::delay(1000);
    indexer.stop();
    
    
    // tower 5
    chassis.pid_straight_drive(-625, 0, 600, 1500);
    chassis.turn_right(130, 240, 1500);
    chassis.pid_straight_drive(1300, 0, 450, 2000);
    uid = chassis.pid_straight_drive(1700, 0, 500, 4250, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
    }
    intakes.stop();
    indexer.stop();
    chassis.pid_straight_drive(1050, 0, 450, 2000);

    chassis.turn_left(46, 450, 1000);
    chassis.pid_straight_drive(730, 0, 450, 1500);
    
    indexer.reset_command_queue();
    indexer.index();
    pros::delay(1000);
    indexer.stop();
    
    
    
    
}

// 
// 
// void Autons::blue_north() {
//     Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
//     Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
//     indexer.update_filter_color("red");
//     Intakes intakes(Motors::left_intake, Motors::right_intake);
//     PositionTracker* tracker = PositionTracker::get_instance();
//     tracker->start_thread();
//     tracker->enable_imu();
//     tracker->set_log_level(0);
//     tracker->set_position({0, 0, 0});
// 
//     deploy();    
// 
//     chassis.drive_to_point(0, 27);
// 
//     chassis.drive_to_point(27.2, 5.5, 0, 1, 125, INT32_MAX, false);
// 
//     chassis.turn_right(51);
//     chassis.pid_straight_drive(235);
// 
//     // chassis.pid_straight_drive(200, 0, 80, INT32_MAX, false, false);
// 
//     for(int i=0; i < 50; i++) {  // score preload
//         indexer.auto_index();
//         pros::delay(10);
//     }
// 
//     chassis.drive_to_point(0, 27);
// 
//     intakes.hold_outward();
// 
//     chassis.drive_to_point(-17.7, 9.7, 0, 1, 100, INT32_MAX, false);
//     // chassis.pid_straight_drive(1100, 0, 150, INT32_MAX, false, false);
// 
//     int uid = chassis.pid_straight_drive(450, 0, 80, 2000, true, false);
//     while(!chassis.is_finished(uid)) {
//         indexer.auto_increment();
//         intakes.intake();
//     }
// 
//     // intakes.intake();
//     // indexer.index_until_filtered();
//     // pros::delay(100);
//     // intakes.stop();
//     // 
//     for(int i=0; i < 100; i++) {  // index a little bit longer
//         indexer.auto_index();
//         pros::delay(10);
//     }
// 
//     chassis.pid_straight_drive(-500, 0, 200, INT32_MAX, false, false);
// }



void Autons::blue_one_tower_left() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "red");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    

    indexer.run_upper_roller();
    pros::delay(400);
    indexer.stop();

    int uid = chassis.pid_straight_drive(400, 0, 450, 1500, true);
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        intakes.intake();
        pros::delay(5);
    }
    pros::delay(500);  // delay to pick up ball
    intakes.stop();
    indexer.stop();

    indexer.run_upper_roller();  // for if ball is in bad spot and will accidentally be dropped out
    indexer.run_lower_roller_reverse();
    pros::delay(75);
    indexer.stop();

    chassis.turn_left(30, 600, 600);

    chassis.pid_straight_drive(350, 0, 450, 1000);

    // score 2
    indexer.index();
    pros::delay(450);
    indexer.stop();

    pros::delay(500);
    indexer.run_upper_roller_reverse();
    pros::delay(200);
    indexer.stop();

    indexer.index();
    pros::delay(750);
    indexer.stop();

    // grab red 
    intakes.intake();
    pros::delay(1000);
    intakes.stop();

    indexer.index();
    pros::delay(800);
    indexer.stop();

    // grab blue 
    intakes.intake();
    pros::delay(1000);
    intakes.hold_outward();

    chassis.pid_straight_drive(-1000);
    chassis.turn_right(115, 450, 1500);
    chassis.pid_straight_drive(1200, 0, 450, 1250);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
}


void Autons::blue_one_tower_right() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "red");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    

    indexer.run_upper_roller();
    pros::delay(400);
    indexer.stop();

    int uid = chassis.pid_straight_drive(400, 0, 450, 1500, true);
    intakes.intake();
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    for(int i = 0; i < 100; i++) {  // add delay to pick up ball
        indexer.increment();
        pros::delay(5);
    }
    intakes.stop();
    indexer.stop();

    indexer.run_upper_roller();  // for if ball is in bad spot and will accidentally be dropped out
    indexer.run_lower_roller_reverse();
    pros::delay(75);
    indexer.stop();

    chassis.turn_right(30, 600, 600);

    chassis.pid_straight_drive(350, 0, 450, 1000);

    // score 2
    indexer.index();
    pros::delay(450);
    indexer.stop();

    pros::delay(500);
    indexer.run_upper_roller_reverse();
    pros::delay(200);
    indexer.stop();

    indexer.index();
    pros::delay(750);
    indexer.stop();
    
    // grab red 
    intakes.intake();
    pros::delay(1000);
    intakes.stop();

    indexer.index();
    pros::delay(800);
    indexer.stop();

    // grab blue 
    intakes.intake();
    pros::delay(1000);
    intakes.hold_outward();

    chassis.pid_straight_drive(100, 0, 450, 500);
    chassis.pid_straight_drive(-1000);
    chassis.turn_left(115, 450, 1500);
    chassis.pid_straight_drive(1200, 0, 450, 1250);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();  
}


void Autons::red_one_tower_left() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    

    indexer.run_upper_roller();
    pros::delay(400);
    indexer.stop();

    int uid = chassis.pid_straight_drive(400, 0, 450, 1500, true);
    intakes.intake();
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    for(int i = 0; i < 100; i++) {  // add delay to pick up ball
        indexer.increment();
        pros::delay(5);
    }
    intakes.stop();
    indexer.stop();
    
    indexer.run_upper_roller();  // for if ball is in bad spot and will accidentally be dropped out
    indexer.run_lower_roller_reverse();
    pros::delay(75);
    indexer.stop();
    
    chassis.turn_left(30, 600, 600);
    
    chassis.pid_straight_drive(350, 0, 450, 1000);
    
    // score 2
    indexer.index();
    pros::delay(450);
    indexer.stop();

    pros::delay(500);
    indexer.run_upper_roller_reverse();
    pros::delay(200);
    indexer.stop();

    indexer.index();
    pros::delay(750);
    indexer.stop();
    
    // grab red 
    intakes.intake();
    pros::delay(1000);
    intakes.stop();
    
    indexer.index();
    pros::delay(800);
    indexer.stop();
    
    // grab blue 
    intakes.intake();
    pros::delay(1000);
    intakes.hold_outward();
    
    chassis.pid_straight_drive(-1000);
    chassis.turn_right(115, 450, 1500);
    chassis.pid_straight_drive(1200, 0, 450, 1250);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
    
    
}


void Autons::red_one_tower_right() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    

    indexer.run_upper_roller();
    pros::delay(400);
    indexer.stop();

    int uid = chassis.pid_straight_drive(400, 0, 450, 1500, true);
    intakes.intake();
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    for(int i = 0; i < 100; i++) {  // add delay to pick up ball
        indexer.increment();
        pros::delay(5);
    }
    intakes.stop();
    indexer.stop();

    indexer.run_upper_roller();  // for if ball is in bad spot and will accidentally be dropped out
    indexer.run_lower_roller_reverse();
    pros::delay(75);
    indexer.stop();

    chassis.turn_right(30, 600, 600);

    chassis.pid_straight_drive(350, 0, 450, 1000);

    // score 2
    indexer.index();
    pros::delay(450);
    indexer.stop();

    pros::delay(500);
    indexer.run_upper_roller_reverse();
    pros::delay(200);
    indexer.stop();

    indexer.index();
    pros::delay(750);
    indexer.stop();

    // grab red 
    intakes.intake();
    pros::delay(1000);
    intakes.stop();

    indexer.index();
    pros::delay(800);
    indexer.stop();

    // grab blue 
    intakes.intake();
    pros::delay(1000);
    intakes.hold_outward();

    chassis.pid_straight_drive(-1000);
    chassis.turn_right(115, 450, 1500);
    chassis.pid_straight_drive(1200, 0, 450, 1250);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
}



void Autons::blue_two_tower_left() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    

    // tower 1
    int uid = chassis.turn_right(30, 600, 1250, true);
    indexer.run_upper_roller();
    while(!chassis.is_finished(uid)) {
        pros::delay(5);
    }
    
    uid = chassis.pid_straight_drive(400, 0, 600, 1000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        pros::delay(5);
    }
    indexer.hard_stop();

    indexer.index();
    pros::delay(350);
    indexer.stop();

    // tower 2
    chassis.pid_straight_drive(-500, 0, 600, 1000);
    chassis.turn_left(160, 450, 1500);
    
    chassis.pid_straight_drive(1100, 0, 450, 2250);
    chassis.turn_right(35, 550, 1000);
    
    uid = chassis.pid_straight_drive(875, 0, 450, 2000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
        pros::delay(5);
    }
    intakes.stop();
    indexer.stop();
    
    for(int i=0; i < 100; i++) {
        indexer.auto_increment();
        intakes.intake();
        pros::delay(7);
    }
    indexer.stop();
    
    // score
    indexer.index();
    pros::delay(400);
    indexer.stop();
    intakes.stop();
    
    pros::delay(300);
    
    indexer.index();
    pros::delay(700);
    indexer.stop();
    
    intakes.hold_outward();
    
    chassis.pid_straight_drive(-1000, 0, 600, 1200);
    chassis.turn_left(120, 450, 1500);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
    indexer.stop();
    
}


void Autons::blue_two_tower_right() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "red");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    

    // tower 1
    int uid = chassis.turn_right(30, 600, 1250, true);
    indexer.run_upper_roller();
    while(!chassis.is_finished(uid)) {
        pros::delay(5);
    }
    
    uid = chassis.pid_straight_drive(400, 0, 600, 1000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        pros::delay(5);
    }
    indexer.hard_stop();

    indexer.index();
    pros::delay(350);
    indexer.stop();

    // tower 2
    chassis.pid_straight_drive(-500, 0, 600, 1000);
    chassis.turn_right(160, 450, 1500);
    
    chassis.pid_straight_drive(1100, 0, 450, 2250);
    chassis.turn_left(35, 550, 1000);
    
    uid = chassis.pid_straight_drive(875, 0, 450, 2000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
        pros::delay(5);
    }
    intakes.stop();
    indexer.stop();
    
    for(int i=0; i < 100; i++) {
        indexer.auto_increment();
        intakes.intake();
        pros::delay(7);
    }
    indexer.stop();
    
    // score
    indexer.index();
    pros::delay(400);
    indexer.stop();
    intakes.stop();
    
    pros::delay(300);
    
    indexer.index();
    pros::delay(700);
    indexer.stop();
    
    intakes.hold_outward();
    
    chassis.pid_straight_drive(-1000, 0, 600, 1200);
    chassis.turn_right(120, 450, 1500);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
    indexer.stop();
    
    
}

void Autons::red_two_tower_left() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    

    // tower 1
    int uid = chassis.turn_right(30, 600, 1250, true);
    indexer.run_upper_roller();
    while(!chassis.is_finished(uid)) {
        pros::delay(5);
    }
    
    uid = chassis.pid_straight_drive(400, 0, 600, 1000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        pros::delay(5);
    }
    indexer.hard_stop();

    indexer.index();
    pros::delay(350);
    indexer.stop();

    // tower 2
    chassis.pid_straight_drive(-500, 0, 600, 1000);
    chassis.turn_right(160, 450, 1500);
    
    chassis.pid_straight_drive(1100, 0, 450, 2250);
    chassis.turn_left(35, 550, 1000);
    
    uid = chassis.pid_straight_drive(875, 0, 450, 2000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
        pros::delay(5);
    }
    intakes.stop();
    indexer.stop();
    
    for(int i=0; i < 100; i++) {
        indexer.auto_increment();
        intakes.intake();
        pros::delay(7);
    }
    indexer.stop();
    
    // score
    indexer.index();
    pros::delay(400);
    indexer.stop();
    intakes.stop();
    
    pros::delay(300);
    
    indexer.index();
    pros::delay(700);
    indexer.stop();
    
    intakes.hold_outward();
    
    chassis.pid_straight_drive(-1000, 0, 600, 1200);
    chassis.turn_right(120, 450, 1500);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
    indexer.stop();
    
    
}


void Autons::red_two_tower_right() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    

    // tower 1
    int uid = chassis.turn_left(30, 600, 1250, true);
    indexer.run_upper_roller();
    while(!chassis.is_finished(uid)) {
        pros::delay(5);
    }

    uid = chassis.pid_straight_drive(400, 0, 600, 1000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        pros::delay(5);
    }
    indexer.hard_stop();

    indexer.index();
    pros::delay(350);
    indexer.stop();

    // tower 2
    chassis.pid_straight_drive(-500, 0, 600, 1000);
    chassis.turn_left(160, 450, 1500);

    chassis.pid_straight_drive(1100, 0, 450, 2250);
    chassis.turn_right(35, 550, 1000);

    uid = chassis.pid_straight_drive(1000, 0, 450, 2000, true);
    while(!chassis.is_finished(uid)) {
        indexer.auto_increment();
        intakes.intake();
        pros::delay(5);
    }
    intakes.stop();
    indexer.stop();

    for(int i=0; i < 100; i++) {
        indexer.auto_increment();
        intakes.intake();
        pros::delay(5);
    }
    indexer.stop();
    intakes.stop();

    // score
    indexer.index();
    pros::delay(350);
    indexer.stop();

    pros::delay(300);

    indexer.index();
    pros::delay(425);
    indexer.stop();

    intakes.hold_outward();

    chassis.pid_straight_drive(-1000, 0, 600, 1200);
    chassis.turn_left(120, 450, 1500);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
    indexer.stop();
}


// void Autons::blue_north_2() {
//     Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
//     Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
//     indexer.update_filter_color("blue");
//     Intakes intakes(Motors::left_intake, Motors::right_intake);
//     PositionTracker* tracker = PositionTracker::get_instance();
//     tracker->start_thread();
//     tracker->enable_imu();
//     tracker->set_log_level(0);
//     tracker->set_position({0, 0, 0});
// 
//     deploy();  
// 
//     chassis.pid_straight_drive(560, 0, 100);
//     chassis.turn_left(136, 100, 5000);
// 
//     intakes.hold_outward();
//     // chassis.pid_straight_drive(300, 0, 80, 2000, false, true);
// 
// 
//     intakes.intake();
//     indexer.auto_increment();
//     chassis.pid_straight_drive(900, 0, 80, 2000, false, false);
//     indexer.stop();
//     // indexer.index_until_filtered();
//     intakes.stop();
// 
//     for(int i=0; i < 150; i++) {  // index a little bit longer
//         // intakes.intake();
//         indexer.index_no_backboard();
//         pros::delay(10);
//     }
// 
//     // 
//     // for(int i=0; i < 100; i++) {  // index a little bit longer
//     //     // intakes.intake();
//     //     indexer.index();
//     //     pros::delay(10);
//     // }
// 
//     indexer.fix_ball();
//     pros::delay(1000);
//     indexer.stop();
//     for(int i=0; i < 40; i++) {  // index a little bit longer
//         intakes.intake();
//         // indexer.auto_index();
//         pros::delay(10);
//     }
//     intakes.stop();
// 
//     intakes.hold_outward();
//     chassis.pid_straight_drive(-400, 0, 80);
// }
// 
// 
// void Autons::red_north() {
//     Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
//     Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
//     indexer.update_filter_color("blue");
//     Intakes intakes(Motors::left_intake, Motors::right_intake);
//     PositionTracker* tracker = PositionTracker::get_instance();
//     tracker->start_thread();
//     tracker->enable_imu();
//     tracker->set_log_level(0);
//     tracker->set_position({0, 0, 0});
// 
//     deploy();  
// 
//     chassis.pid_straight_drive(560, 0, 100);
//     chassis.turn_right(136, 100, 5000);
// 
//     intakes.hold_outward();
//     // chassis.pid_straight_drive(300, 0, 80, 2000, false, true);
// 
// 
//     intakes.intake();
//     indexer.auto_increment();
//     chassis.pid_straight_drive(900, 0, 80, 2000, false, false);
//     indexer.stop();
//     // indexer.index_until_filtered();
//     intakes.stop();
// 
//     for(int i=0; i < 150; i++) {  // index a little bit longer
//         // intakes.intake();
//         indexer.index_no_backboard();
//         pros::delay(10);
//     }
// 
//     // 
//     // for(int i=0; i < 100; i++) {  // index a little bit longer
//     //     // intakes.intake();
//     //     indexer.index();
//     //     pros::delay(10);
//     // }
// 
//     indexer.fix_ball();
//     pros::delay(1000);
//     indexer.stop();
//     for(int i=0; i < 40; i++) {  // index a little bit longer
//         intakes.intake();
//         // indexer.auto_index();
//         pros::delay(10);
//     }
//     intakes.stop();
// 
//     intakes.hold_outward();
//     chassis.pid_straight_drive(-400, 0, 80);
// }

void Autons::run_autonomous() {
    switch(selected_number) {
        case 1:
           break;

        case 2:
            one_pt();
            break;

        case 3:
            skills();
            break;
            
        case 4:
            skills2();
            break;
            
        case 5:
            blue_one_tower_left();
            break;
            
        case 6:
            blue_one_tower_right();
            break;
            
        case 7:
            red_one_tower_left();
            break;
            
        case 8:
            red_one_tower_right();
            break;
            
            
        case 9:
            blue_two_tower_left();
            break;
            
        case 10:
            blue_two_tower_right();
            break;
            
        case 11:
            red_two_tower_left();
            break;
            
        case 12:
            red_two_tower_right();
            break;
            
    }
}
