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
#include "objects/robotChassis/chassis.hpp"
#include "objects/tilter/Tilter.hpp"
#include "objects/lift/Lift.hpp"


Autons::Autons( )
{
    debug_auton_num = 8;
    driver_control_num = 1;
}



Autons::~Autons( )
{

}




/**
 * scores four cubes in smaller zone for red side
 */
void Autons::auton1( autonConfig cnfg )
{
    Motors *motors = Motors::get_instance();
    Tilter tilter;
    Chassis chassis;
    Lift lift;

    //deploy
    lift.move(360, 500);
    motors->right_intake->move(-127);
    motors->left_intake->move(-127);
    pros::delay(700);
    lift.move(-360, 500);

    chassis.straight(440, 100);

    for ( int i = 0; i < 4; i++ )
    {
        motors->right_intake->move(127);
        motors->left_intake->move(127);
        chassis.straight(160, 30);
        pros::delay(50);
        //pros::delay(150);
    }

    chassis.straight(50, 40);

    motors->right_intake->move(80);
    motors->left_intake->move(80);

    chassis.straight(-500, 100);

    chassis.turnRight(520); // 180 degrees

    chassis.straight(400, 90);

    chassis.turnLeft(200, 700); // 65 degrees

    chassis.straight(200, 80, 500);

    chassis.straight(100, 50, 500);

    //dump stack
    tilter.move(1200); //move stack upright

    pros::delay(100);

    motors->right_intake->move(-50); //outake to release
    motors->left_intake->move(-50);

    chassis.straight(-500, 30); //back up from stack

    tilter.move(-900); //move stack upright

}




/**
 * scores four cubes in smaller zone for blue side
 */
void Autons::auton2( autonConfig cnfg )
{
    Motors *motors = Motors::get_instance();
    Tilter tilter;
    Chassis chassis;
    Lift lift;

    //deploy
    lift.move(360, 500);
    motors->right_intake->move(-127);
    motors->left_intake->move(-127);
    pros::delay(700);
    lift.move(-360, 500);

    chassis.straight(440, 100);

    for ( int i = 0; i < 4; i++ )
    {
        motors->right_intake->move(127);
        motors->left_intake->move(127);
        chassis.straight(160, 30);
        pros::delay(50);
        //pros::delay(150);
    }

    chassis.straight(50, 40);

    motors->right_intake->move(80);
    motors->left_intake->move(80);

    chassis.straight(-500, 100);

    chassis.turnLeft(520); // 180 degrees

    chassis.straight(400, 90);

    chassis.turnRight(200, 700); // 65 degrees

    chassis.straight(200, 80, 500);

    chassis.straight(100, 50, 500);

    //dump stack
    tilter.move(1200); //move stack upright

    pros::delay(100);

    motors->right_intake->move(-50); //outake to release
    motors->left_intake->move(-50);

    chassis.straight(-500, 30); //back up from stack

    tilter.move(-900); //move stack upright
}




/**
 * scores cubes in the big zone for red
 */
void Autons::auton3( autonConfig cnfg )
{
    Motors *motors = Motors::get_instance();
    Tilter tilter;
    Chassis chassis;
    Lift lift;

    //push cube out of way
    chassis.straight(400, 75);
    chassis.straight(-200, 75);

    //deploy
    lift.move(360, 500);
    motors->right_intake->move(-127);
    motors->left_intake->move(-127);
    pros::delay(1200);
    lift.move(-360, 500);

    motors->right_intake->move(127);
    motors->left_intake->move(127);

    chassis.straight(600, 100);

    //move to right in front of cube and bring arms up
    chassis.straight(150, 100);
    lift.move(720, 800);
    chassis.straight(50, 50);
    
    //move arms down while going forward 
    lift.move(-720, 800);
    chassis.straight(350, 50);
    
    //back up
    chassis.straight(-630, 100);
    
    //turn towards cube 
    chassis.turnLeft(270); //approx. 90 degrees
    
    //drive up to cube
    chassis.straight(330, 100);
    
    //pick up cube
    chassis.straight(340, 60);
    
    //turn to zone
    chassis.turnLeft(150); //approx. 50 degrees

    //move to zone
    chassis.straight(430, 75, 2000);

    //dump stack
    tilter.move(1200); //move stack upright

    pros::delay(100);

    motors->right_intake->move(-50); //outake to release
    motors->left_intake->move(-50);

    chassis.straight(-500, 30); //back up from stack

    tilter.move(-900); //move stack upright

}




/**
 * scores cubes in the big zone for blue
 */
void Autons::auton4( autonConfig cnfg )
{
    Motors *motors = Motors::get_instance();
    Tilter tilter;
    Chassis chassis;
    Lift lift;

    //push cube out of way
    chassis.straight(400, 75);
    chassis.straight(-200, 75);

    //deploy
    lift.move(360, 500);
    motors->right_intake->move(-127);
    motors->left_intake->move(-127);
    pros::delay(1200);
    lift.move(-360, 500);

    motors->right_intake->move(127);
    motors->left_intake->move(127);

    chassis.straight(600, 100);

    //move to right in front of cube and bring arms up
    chassis.straight(150, 100);
    lift.move(720, 800);
    chassis.straight(50, 50);
    
    //move arms down while going forward 
    lift.move(-720, 800);
    chassis.straight(350, 50);
    
    //back up
    chassis.straight(-630, 100);
    
    //turn towards cube 
    chassis.turnRight(270); //approx. 90 degrees
    
    //drive up to cube
    chassis.straight(330, 100);
    
    //pick up cube
    chassis.straight(340, 60);
    
    //turn to zone
    chassis.turnRight(150); //approx. 50 degrees

    //move to zone
    chassis.straight(430, 75, 2000);

    //dump stack
    tilter.move(1200); //move stack upright

    pros::delay(100);

    motors->right_intake->move(-50); //outake to release
    motors->left_intake->move(-50);

    chassis.straight(-500, 30); //back up from stack

    tilter.move(-900); //move stack upright
}




/**
 * drives forward to score in the zone, then drive backward
 * to stop touching the cube
 */
void Autons::auton5( autonConfig cnfg )
{
    Chassis chassis;
    Motors *motors = Motors::get_instance();
    Lift lift;


    chassis.straight(-500, 100, 5000);
    pros::delay(500);
    chassis.straight(500, 100);
    
    pros::delay(2000);
    
    //deploy
    lift.move(360, 500);
    motors->right_intake->move(-127);
    motors->left_intake->move(-127);
    pros::delay(1200);
    lift.move(-450, 500);
}





/**
 * runs unit test
 * 180 degree, 90 degree, 45 degree, 45 degree
 * tilter movement
 * straight drive moving
 */
void Autons::auton6( autonConfig cnfg )
{
    Motors *motors = Motors::get_instance();
    Tilter tilter;
    Chassis chassis;

    //turn testing

    chassis.turnRight(700); //180
    pros::delay(750);
    chassis.turnRight(350); //90
    pros::delay(750);
    chassis.turnRight(175); //45
    pros::delay(750);
    chassis.turnRight(175); //45
    pros::delay(750);

    //tilter testing
    tilter.move(100);
    pros::delay(750);
    tilter.move(-100);
    pros::delay(750);

    //straight drive testing
    chassis.straight(500, 100);
    pros::delay(750);
    chassis.straight(-500, 50);
    pros::delay(750);
}
