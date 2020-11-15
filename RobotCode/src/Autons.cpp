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

}


/**
 * drives forward to score in the zone, then drive backward
 * to stop touching the cube
 */
void Autons::one_pt( autonConfig cnfg ) {

    deploy();
}





/**
 * runs unit test
 * 180 degree, 90 degree, 45 degree, 45 degree
 * tilter movement
 * straight drive moving
 */
void Autons::skills( autonConfig cnfg ) {

}
