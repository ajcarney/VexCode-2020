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
#include "objects/subsystems/Tilter.hpp"
#include "objects/subsystems/Lift.hpp"


Autons::Autons( )
{
    debug_auton_num = 10;
    driver_control_num = 1;
}



Autons::~Autons( )
{

}


/**
 * deploys by outtaking and bringing the lift up
 */
void Autons::deploy()
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    Tilter tilter( Motors::tilter );
    
    Motors::lift.set_brake_mode(pros::E_MOTOR_BRAKE_COAST); // set to coast so lift can go down
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    
    Motors::right_intake.move(-127);
    lift.move_to(1050, true);
    lift.move_to(0, true);
    Motors::right_intake.move(0);
    
    Motors::lift.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}




/**
 * automatically scores a stack
 */
void Autons::dump_stack()
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    Tilter tilter( Motors::tilter );
    
    tilter.move_to(3030, true, 4000);
    
    Motors::right_intake.move(-90);
    Motors::left_intake.move(-90);
    pros::delay(200);
    Motors::right_intake.move(-30);
    Motors::left_intake.move(-30);
    
    
    chassis.straight_drive(-300, 8000, 2000, true, false, false);
    bool settled = false;
    int stop = pros::millis() + 400;
    while ( !settled && pros::millis() < stop )
    {
        tilter.move(-127);
        settled = chassis.straight_drive(-300, 8000, 2000, false, false, false );
        pros::delay(10);
    }
    tilter.move(0);
    
    Motors::right_intake.move(0);
    Motors::left_intake.move(0);
}




/**
 * scores five cubes in smaller zone for red side
 */
void Autons::five_cube_red_small_zone( autonConfig cnfg )
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    Tilter tilter( Motors::tilter );

    deploy();
    Motors::right_intake.move(127);
    Motors::left_intake.move(127);

    chassis.straight_drive(1200, 6500);
    
    chassis.straight_drive(-850);
    
    pros::delay(400);
    
    chassis.turn_right(152.0, 8000, 1500);
    
    pros::delay(200);
    
    chassis.straight_drive(150, 9000, 750);
    
    Motors::right_intake.move(-127);
    Motors::left_intake.move(-127);
    pros::delay(350);
    Motors::right_intake.move(0);
    Motors::left_intake.move(0);
    
    dump_stack();
    
    chassis.straight_drive(50, 12000, 750);
    
    chassis.straight_drive(-800, 12000, 2000);

}



/**
 * scores five cubes in smaller zone for blue side
 */
void Autons::five_cube_blue_small_zone( autonConfig cnfg )
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    Tilter tilter( Motors::tilter );

    deploy();
    Motors::right_intake.move(127);
    Motors::left_intake.move(127);

    chassis.straight_drive(1200, 6500);
    
    chassis.straight_drive(-850);
    
    pros::delay(400);
    
    chassis.turn_left(152.0, 12000, 1500);
    
    chassis.straight_drive(150, 9000, 750);
    
    Motors::right_intake.move(-127);
    Motors::left_intake.move(-127);
    pros::delay(350);
    Motors::right_intake.move(0);
    Motors::left_intake.move(0);
    
    dump_stack();
    
    chassis.straight_drive(50, 12000, 750);
    
    chassis.straight_drive(-800, 12000, 2000);

}




/**
 * scores seven cubes in smaller zone for red side
 */
void Autons::seven_cube_red_small_zone( autonConfig cnfg )
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    Tilter tilter( Motors::tilter );
    
    deploy();
    
    Motors::right_intake.move(127);
    Motors::left_intake.move(127);
    
    chassis.straight_drive(825, 6500, 5000);
    pros::delay(300);
    
    chassis.straight_drive(-325);
    
    chassis.turn_left(105, 12000, 1300);
    
    chassis.straight_drive(-800);
    
    chassis.turn_right(115, 12000, 1300);
    
    chassis.straight_drive(1060, 6500);
    
    Motors::right_intake.move(5);
    Motors::left_intake.move(5);
    
    chassis.straight_drive(-800);
    
    chassis.turn_right(200, 12000, 1000);
    
    chassis.straight_drive(400, 9000);
}




/**
 * scores seven cubes in smaller zone for blue side
 */
void Autons::seven_cube_blue_small_zone( autonConfig cnfg )
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    Tilter tilter( Motors::tilter );
    
    deploy();
    
    Motors::right_intake.move(127);
    Motors::left_intake.move(127);
    
    chassis.straight_drive(825, 6500, 5000);
    pros::delay(300);
    
    chassis.straight_drive(-325);
    
    chassis.turn_right(105, 12000, 1300);
    
    chassis.straight_drive(-800);
    
    chassis.turn_left(115, 12000, 1300);
    
    chassis.straight_drive(1060, 6500);
    
    Motors::right_intake.move(5);
    Motors::left_intake.move(5);
    
    chassis.straight_drive(-800);
    
    chassis.turn_left(200, 12000, 1000);
    
    chassis.straight_drive(400, 9000);
    
    // Motors::right_intake.move(-85);
    // Motors::left_intake.move(-85); 
    // pros::delay(325);
    // Motors::right_intake.move(0);
    // Motors::left_intake.move(0);
    
    dump_stack();
    
    chassis.straight_drive(-800, 12000, 2000);
        
}



/**
 * scores cubes in the big zone for red
 */
void Autons::red_big_zone( autonConfig cnfg )
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    Tilter tilter( Motors::tilter );
    
    chassis.straight_drive(1200, 10000, 3500); 
    chassis.straight_drive(-100);
    
    chassis.turn_right(100, 12000, 1300);
    chassis.straight_drive(-300);
    
    deploy();

    Motors::right_intake.move(127);
    Motors::left_intake.move(127);
    chassis.straight_drive(1200);
    
    pros::delay(500);
    
    chassis.straight_drive(-300);
    
    chassis.turn_right(90, 12000, 1300);
    
    chassis.straight_drive(500);
    
    pros::delay(300);
    
    chassis.turn_right(105, 12000, 1300);
    
    chassis.straight_drive(750, 12000, 1000);
    
    chassis.turn_left(30, 6000, 1300);
    
    Motors::right_intake.move(-85);
    Motors::left_intake.move(-85); 
    pros::delay(325);
    Motors::right_intake.move(0);
    Motors::left_intake.move(0);
    
    dump_stack();
    
    chassis.straight_drive(-800, 12000, 2000);
    

}




/**
 * scores cubes in the big zone for blue
 */
void Autons::blue_big_zone( autonConfig cnfg )
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    Tilter tilter( Motors::tilter );
    
    chassis.straight_drive(1200, 10000, 3500); 
    chassis.straight_drive(-100);
    
    chassis.turn_left(100, 12000, 1300);
    chassis.straight_drive(-300);
    
    deploy();

    Motors::right_intake.move(127);
    Motors::left_intake.move(127);
    chassis.straight_drive(1200);
    
    pros::delay(500);
    
    chassis.straight_drive(-300);
    
    chassis.turn_left(90, 12000, 1300);
    
    chassis.straight_drive(500);
    
    pros::delay(300);
    
    chassis.turn_left(105, 12000, 1300);
    
    chassis.straight_drive(750, 12000, 1000);
    
    chassis.turn_right(30, 6000, 1300);
    
    Motors::right_intake.move(-85);
    Motors::left_intake.move(-85); 
    pros::delay(325);
    Motors::right_intake.move(0);
    Motors::left_intake.move(0);
    
    dump_stack();
    
    chassis.straight_drive(-800, 12000, 2000);
}




/**
 * drives forward to score in the zone, then drive backward
 * to stop touching the cube
 */
void Autons::one_pt( autonConfig cnfg )
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    Tilter tilter( Motors::tilter );
    
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
void Autons::skills( autonConfig cnfg )
{
    Chassis chassis( Motors::front_left, Motors::front_right, Motors::back_left, Motors::back_right, 12.4 );
    Lift lift( Motors::lift, {0, 500, 700} );
    lift.set_pid_constants({1, 0, 0, INT32_MAX}); // pid for not holding is different than raising
    Tilter tilter( Motors::tilter );
    
    //seven_cube_red_small_zone();  //start with small zone auton
    chassis.turn_right(180, 12000, 2000);
    chassis.straight_drive(700);
    
    
    while ( Sensors::check_for_cube() )  //move forward until a cube is in the intake
    {
        chassis.move(75);
        Motors::right_intake.move(127);
        Motors::left_intake.move(127);
    }
    chassis.move(0);
    Motors::right_intake.move(0);
    Motors::left_intake.move(0);
    
    //back up, lift up and score in the tower
    chassis.straight_drive(-200);
    lift.move_to(1200, true);
    chassis.straight_drive(150);
    
    //outtake slowly so cube doesn't launch over
    Motors::right_intake.move(-80);
    Motors::left_intake.move(-80);
    
    

}
