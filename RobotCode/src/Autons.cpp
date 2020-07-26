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
#include "objects/subsystems/chassis.hpp"
#include "objects/subsystems/Lift.hpp"


Autons::Autons( )
{
    debug_auton_num = 4;
    driver_control_num = 1;
}



Autons::~Autons( ) {

}


/**
 * deploys by outtaking and bringing the lift up
 */
void Autons::deploy() {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
}


/**
 * drives forward to score in the zone, then drive backward
 * to stop touching the cube
 */
void Autons::one_pt( autonConfig cnfg ) {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    
    chassis.straight_drive(-900, 9000, 4000);
    chassis.straight_drive(900, 12000, 4000);
    
    deploy();
}





/**
 * runs unit test
 * 180 degree, 90 degree, 45 degree, 45 degree
 * tilter movement
 * straight drive moving
 */
void Autons::skills( autonConfig cnfg ) {
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    
    //seven_cube_red_small_zone();  //start with small zone auton
    chassis.turn_right(180, 12000, 2000);
    chassis.straight_drive(700);

}
