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
    debug_auton_num = 17;
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

    intakes.rocket_outward();
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
 * skills autonomous
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
    chassis.set_turn_gains({4, 0.0001, 20, INT32_MAX, INT32_MAX});
    
    // tower 1
    int uid = chassis.okapi_pid_straight_drive(-770, 6000, 2500, true, 0);
    indexer.run_upper_roller();
    while(!chassis.is_finished(uid)) {
        pros::delay(5);
    }
    
    uid = chassis.turn_right(43, 300, 2500, true);
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    indexer.stop();
    
    uid = chassis.okapi_pid_straight_drive(1100, 6000, 3000, true, 2200);   // pick up next ball
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    indexer.stop();
    
    // score first tower
    indexer.index();  // score first two
    pros::delay(700);
    indexer.stop();
    
    uid = chassis.okapi_pid_straight_drive(-1200, 6000, 2000, false, 0);

    chassis.turn_to_angle(93, 300, 3000, false);
    chassis.okapi_pid_straight_drive(-6000, 6000, 10000, false, 1000);
    chassis.okapi_pid_straight_drive(700, 6000, 2000, false, 1000);
    
    chassis.turn_right(93, 200, 4000, false);
    
    uid = chassis.okapi_pid_straight_drive(3500, 5000, 6500, true, 0);   // pick up next ball
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        intakes.intake();
        pros::delay(5);
    }
    indexer.stop();
    intakes.stop();
    
    uid = chassis.turn_right(45, 300, 2500, true);
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    indexer.stop();
    
    uid = chassis.okapi_pid_straight_drive(1000, 6000, 3000, true, 2200);   // pick up next ball
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    indexer.stop();
    
    indexer.index();  // score first two
    pros::delay(700);
    indexer.stop();
    
    
    uid = chassis.okapi_pid_straight_drive(-700, 6000, 2000, false, 0);

    chassis.turn_left(42, 300, 2500, false);
    chassis.okapi_pid_straight_drive(-1050, 6000, 3000, false, 0);
    chassis.turn_left(90, 300, 2500, false);
    chassis.okapi_pid_straight_drive(-2000, 6000, 3000, false, 0);
    
    uid = chassis.okapi_pid_straight_drive(2800, 5000, 7000, true, 0);   // pick up next ball
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        intakes.intake();
        pros::delay(5);
    }
    indexer.stop();
    intakes.stop();
    for(int i = 0; i < 100; i++) {
        indexer.increment();
        intakes.intake();
        pros::delay(10);
    }
    indexer.stop();
    intakes.stop();
    
    chassis.turn_left(90, 300, 2500, false);
    intakes.rocket_outward();
    chassis.okapi_pid_straight_drive(1000, 4000, 5000, false, 0);

    indexer.index_no_backboard();  // score first two
    pros::delay(700);
    indexer.stop();
    
    chassis.okapi_pid_straight_drive(-500, 4000, 4000, false, 0);
    chassis.turn_left(10, 300, 1000, false);
    intakes.intake();
    pros::delay(500);
    chassis.move(127);
    pros::delay(1000);
    chassis.move(0);
    intakes.stop();
    chassis.okapi_pid_straight_drive(-500, 4000, 4000, false, 0);
    
    
    
    
    
    
    
    
//     Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
//     Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
//     Intakes intakes(Motors::left_intake, Motors::right_intake);
//     PositionTracker* tracker = PositionTracker::get_instance();
//     tracker->start_thread();
//     tracker->enable_imu();
//     tracker->set_log_level(0);
//     tracker->set_position({0, 0, 0});
//     chassis.set_turn_gains({4, 0.0001, 20, INT32_MAX, INT32_MAX});    
// 
// // tower 1
//     int uid = chassis.turn_right(30, 600, 1250, true);  // turn to tower
//     indexer.run_upper_roller();
//     chassis.wait_until_finished(uid);
// 
//     uid = chassis.okapi_pid_straight_drive(400, 600, true, 1000);  // drive to tower
//     while(!chassis.is_finished(uid)) {
//         indexer.auto_increment();
//         pros::delay(5);
//     }
//     indexer.hard_stop();
// 
//     indexer.index();   // score
//     pros::delay(600);
//     indexer.stop();
// 
// // tower 2
//     chassis.okapi_pid_straight_drive(-500, 600, false, 900);  // back up and turn to next ball
//     chassis.turn_right(181, 450, 1500);
// 
//     uid = chassis.okapi_pid_straight_drive(1100, 500, true, 2000);   // pick up next ball
//     while(!chassis.is_finished(uid)) {
//         indexer.auto_increment();
//         intakes.intake();
//         pros::delay(5);
//     }
//     intakes.stop();
//     indexer.stop();
// 
//     chassis.turn_left(68, 450, 1500);   // turn and drive to corner tower
//     chassis.okapi_pid_straight_drive(800, 350, false, 1200);
// 
//     indexer.index();  // score
//     pros::delay(1000);
//     indexer.stop();
// 
//     for(int i = 0; i < 90; i++) {  // pick up two balls from corner tower
//         indexer.increment();
//         intakes.intake();
//         pros::delay(10);
//     }
//     intakes.rocket_outward();  // hold outward while indexed unwanted balls so they don't get picked back up
//     indexer.stop();
// 
// // tower 3 
//     uid = chassis.okapi_pid_straight_drive(-2000, 550, true, 3000);  // drive backwards to next tower and index two blue balls
//     pros::delay(200);  // wait to get moving before indexing out blue balls
//     indexer.run_upper_roller_reverse();
//     indexer.run_lower_roller_reverse();
//     chassis.wait_until_finished(uid);
//     indexer.stop();
//     intakes.stop();
// 
//     // chassis.turn_to_angle(245, 525, 2000);  // turns to pick up ball near 3rd tower
//     // 
//     // uid = chassis.okapi_pid_straight_drive(1500, 550, true, 1000);  // picks up ball to score in 3rd tower
//     uid = chassis.drive_to_point(-61, -23, 0, 1, 600, 2000, true, true);
//     while(!chassis.is_finished(uid)) {
//         indexer.auto_increment();
//         intakes.intake();
//         pros::delay(5);
//     }
//     chassis.okapi_pid_straight_drive(200, 400, false, 900);
//     while(!chassis.is_finished(uid)) {
//         indexer.auto_increment();
//         intakes.intake();
//         pros::delay(5);
//     }
//     indexer.stop();
//     intakes.stop();
// 
//     chassis.turn_to_angle(185, 400, 1500);  // turn and drive to 3rd tower
//     chassis.okapi_pid_straight_drive(500, 550, false, 1000);
// 
//     indexer.index();  //score ball
//     pros::delay(1000);
//     indexer.stop();
// 
//     for(int i = 0; i < 60; i++) {  // pick up blue ball
//         indexer.increment();
//         intakes.intake();
//         pros::delay(10);
//     }
// 
//     intakes.hold_outward();
//     indexer.stop();
// 
// // tower 4
//     chassis.pid_straight_drive(-1000, 550);
//     intakes.stop();
//     chassis.turn_to_angle(274, 525, 2500);  // turns to pick up ball near 4th tower
// 
//     uid = chassis.okapi_pid_straight_drive(1750, 400, true, 3000);   // pick up next ball
//     while(!chassis.is_finished(uid)) {
//         indexer.auto_increment();
//         intakes.intake();
//         pros::delay(5);
//     }
//     intakes.stop();
//     indexer.stop();
// 
//     chassis.turn_left(48, 450, 1500);   // turn and drive to corner tower
//     chassis.okapi_pid_straight_drive(1500, 550, 1800, false);
// 
//     indexer.index();  // score
//     pros::delay(1000);
//     indexer.stop();
// 
//     for(int i = 0; i < 90; i++) {  // pick up two balls from corner tower
//         indexer.increment();
//         intakes.intake();
//         pros::delay(10);
//     }
//     indexer.stop();
//     intakes.rocket_outward();  // hold outward while indexed unwanted balls so they don't get picked back up
// 
// // tower 5
//     uid = chassis.okapi_pid_straight_drive(-1000, 550, true, 3000);  // drive backwards to next tower and index two blue balls
//     pros::delay(200);  // wait to get moving before indexing out blue balls
//     indexer.run_upper_roller_reverse();
//     indexer.run_lower_roller_reverse();
//     chassis.wait_until_finished(uid);
//     indexer.stop();
//     intakes.stop();
}



void Autons::skills_old() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, "blue");    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});
    chassis.set_turn_gains({4, 0.0001, 20, INT32_MAX, INT32_MAX});  // this was written with slow turns in mind
    
}

/*
    turn_direction = 1 will run auton for left side, turn_direction = -1 will run right side
*/
void Autons::one_tower_auton(std::string filter_color, int turn_direction) {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, filter_color);    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});  
    chassis.set_turn_gains({4, 0.0001, 20, INT32_MAX, INT32_MAX});    
    
    indexer.run_upper_roller();
    pros::delay(400);
    indexer.stop();

    int uid = chassis.pid_straight_drive(400, 0, 450, 1500, true);
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        intakes.intake();
        pros::delay(5);
    }
    for(int i = 0; i < 100; i++) {
        indexer.increment();
        intakes.intake();
        pros::delay(5);
    }
    intakes.stop();
    indexer.stop();

    indexer.run_upper_roller();  // for if ball is in bad spot and will accidentally be dropped out
    indexer.run_lower_roller_reverse();
    pros::delay(75);
    indexer.stop();

    chassis.turn_left(turn_direction * 30, 600, 600);

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
    chassis.turn_right(turn_direction * 115, 450, 1500);
    chassis.pid_straight_drive(1200, 0, 450, 1250);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
}


/*
    turn_direction = 1 will run auton for left side, turn_direction = -1 will run right side
*/
void Autons::two_tower_auton(std::string filter_color, int turn_direction) {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, filter_color);    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    
    chassis.set_turn_gains({4, 0.0001, 20, INT32_MAX, INT32_MAX});
    
    // tower 1
    int uid = chassis.turn_right(turn_direction * 30, 600, 1250, true);
    indexer.run_upper_roller();
    while(!chassis.is_finished(uid)) {
        pros::delay(5);
    }

    uid = chassis.pid_straight_drive(500, 0, 600, 1500, true);
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    indexer.stop();

    indexer.index();
    pros::delay(500);
    indexer.stop();

    // tower 2
    chassis.pid_straight_drive(-500, 0, 600, 1000);
    chassis.turn_right(turn_direction * 160, 450, 1500);

    chassis.pid_straight_drive(1100, 0, 450, 2250);
    chassis.turn_left(turn_direction * 35, 550, 1000);

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
        pros::delay(5);
    }
    indexer.stop();
    intakes.stop();

    // score
    indexer.index();
    pros::delay(300);
    pros::delay(250);
    indexer.stop();
    
    // pros::delay(250);
    // indexer.index();
    // pros::delay(400);
    // indexer.stop();

    // pros::delay(300);
    // 
    // indexer.index();
    // pros::delay(400);
    // indexer.stop();

    intakes.hold_outward();

    chassis.pid_straight_drive(-1000, 0, 600, 1200);
    chassis.turn_right(turn_direction * 120, 450, 1500);
    intakes.intake();
    indexer.filter();
    pros::delay(1000);
    intakes.stop();
    indexer.stop();
}


/*
    turn_direction = 1 will run auton for left side, turn_direction = -1 will run right side
*/
void Autons::two_tower_new_auton(std::string filter_color, int turn_direction) {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, Sensors::left_encoder, Sensors::right_encoder, 16, 3/5);
    Indexer indexer(Motors::upper_indexer, Motors::lower_indexer, Sensors::ball_detector, filter_color);    
    Intakes intakes(Motors::left_intake, Motors::right_intake);
    PositionTracker* tracker = PositionTracker::get_instance();
    tracker->start_thread();
    tracker->enable_imu();
    tracker->set_log_level(0);
    tracker->set_position({0, 0, 0});    
    chassis.set_turn_gains({4, 0.0001, 20, INT32_MAX, INT32_MAX});
    
    // tower 1
    int uid = chassis.okapi_pid_straight_drive(-770, 9000, 1400, true, 0);
    indexer.run_upper_roller();
    while(!chassis.is_finished(uid)) {
        pros::delay(5);
    }
    
    uid = chassis.turn_right(turn_direction * 39, 600, 1200, true);
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        pros::delay(5);
    }
    indexer.stop();
    
    uid = chassis.okapi_pid_straight_drive(1200, 6000, 1300, true, 2500);   // pick up next ball
    while(!chassis.is_finished(uid)) {
        indexer.increment();
        intakes.intake();
        pros::delay(5);
    }
    indexer.stop();
    intakes.stop();
    // for(int i=0; i < 50; i++) {
    //     indexer.increment();
    //     pros::delay(5);
    // }
    // indexer.stop();
    
    // score first tower
    indexer.run_upper_roller();  // score first two
    pros::delay(350);
    indexer.run_lower_roller();
    pros::delay(300);
    intakes.intake();
    
    
    for(int i=0; i < 130; i++) {  // index for 1000ms or until middle ball is bad color
        if(indexer.get_state().middle_color == filter_color) {
            break;
        }
        pros::delay(10);
    }
    intakes.stop();
    indexer.stop();
    
    // indexer.run_upper_roller();  // score third ball
    // pros::delay(400);
    // indexer.stop();
    
    
    // move to second tower
    intakes.rocket_outward();
    pros::delay(500);
    chassis.okapi_pid_straight_drive(-1075, 9000, 2300, false, 0);
    
    indexer.index(); //  index while turning and backing up
    chassis.turn_right(turn_direction * 47, 500, 1500, false);
    chassis.okapi_pid_straight_drive(-1650, 10000, 2000, false, 2000);
    indexer.stop();
    intakes.stop();
    
    chassis.turn_left(turn_direction * 93, 500, 1500, false);
    chassis.okapi_pid_straight_drive(800, 12000, 900, false, 0);
    
    intakes.intake();
    indexer.index();
    int start = pros::millis();
    while(indexer.get_state().middle_color != filter_color) {
        if(pros::millis() > start + 1000) {  // timeout
            break;
        }
        pros::delay(10);
    }
    intakes.stop();
    indexer.stop();
    
    // indexer.run_upper_roller();  // score third ball
    // pros::delay(400);
    // indexer.stop();
    
    
    intakes.rocket_outward();
    pros::delay(100);
    chassis.okapi_pid_straight_drive(-800, 11000, 2500, false, 0);
    intakes.stop();
    
    
}



void Autons::blue_one_tower_left() {
    one_tower_auton("red", 1);
}


void Autons::blue_one_tower_right() {
    one_tower_auton("red", -1);
}


void Autons::red_one_tower_left() {
    one_tower_auton("blue", 1);
}


void Autons::red_one_tower_right() {
    one_tower_auton("blue", -1);
}



void Autons::blue_two_tower_left() {
    two_tower_auton("red", 1);
}


void Autons::blue_two_tower_right() {
    two_tower_auton("red", -1);
}

void Autons::red_two_tower_left() {
    two_tower_auton("blue", 1);
}


void Autons::red_two_tower_right() {
    two_tower_auton("blue", -1);
}


void Autons::blue_two_tower_new_left() {
    two_tower_new_auton("red", 1);
}


void Autons::blue_two_tower_new_right() {
    two_tower_new_auton("red", -1);
}


void Autons::red_two_tower_new_left() {
    two_tower_new_auton("blue", 1);
}


void Autons::red_two_tower_new_right() {
    two_tower_new_auton("blue", -1);
}


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
            skills_old();
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
            
        case 13:
            blue_two_tower_new_left();
            break;
            
        case 14:
            blue_two_tower_new_right();
            break;
            
        case 15:
            red_two_tower_new_left();
            break;
            
        case 16:
            red_two_tower_new_right();
            break;
            
    }
}
