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
#include "okapi/api.hpp"

#include "Autons.hpp"
#include "objects/motors/Motors.hpp"
#include "objects/motors/MotorThread.hpp"
#include "objects/subsystems/chassis.hpp"
#include "objects/lcdCode/DriverControl/AutonomousLCD.hpp"

using namespace okapi::literals;


int Autons::selected_number = 1;

Autons::Autons( )
{
    debug_auton_num = 4;
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
 * deploys by outtaking and bringing the lift up
 */
void Autons::deploy() {

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
    // unregister drive motors because the okapi chassis will be used for skills
    MotorThread* motor_thread = MotorThread::get_instance();
    motor_thread->unregister_motor(Motors::front_right);
    motor_thread->unregister_motor(Motors::front_left);
    motor_thread->unregister_motor(Motors::back_right);
    motor_thread->unregister_motor(Motors::back_left);
    
    Configuration* config = Configuration::get_instance();
    config->filter_color = "blue";
    
    Motors::odom_chassis->setState({0_in, 0_in, 0_deg});
    Motors::odom_chassis->driveToPoint({3_ft, 0_ft});
    
    pros::delay(1000);
    
    Motors::profile_controller->setTarget("A");
    Motors::profile_controller->waitUntilSettled();
    
    pros::delay(1000);
    
    Motors::odom_chassis->driveToPoint({0_ft, 0_ft});  // drive back to start


    // re-register motors for driver control
    motor_thread->register_motor(Motors::front_right);
    motor_thread->register_motor(Motors::front_left);
    motor_thread->register_motor(Motors::back_right);
    motor_thread->register_motor(Motors::back_left);
    
}



void Autons::setup_odometry() {
//                       Wheel diameter
// 
//                        +-+      Center of rotation
//                        | |      |
//                        v v      +----------+ Length to middle wheel
//                                 |          | from center of rotation
//               +--->    ===      |      === |
//               |         +       v       +  |
//               |        ++---------------++ |
//               |        |                 | v
// Wheel track   |        |                 |
//               |        |        x        |+|  <-- Middle wheel
//               |        |                 |
//               |        |                 |
//               |        ++---------------++
//               |         +               +
//               +--->    ===             ===
    AutonomousLCD auton_lcd;
    Configuration* config = Configuration::get_instance();
    
    int8_t bl = config->back_left_reversed ? -config->back_left_port : config->back_left_port;
    int8_t fl = config->front_left_reversed ? -config->front_left_port : config->front_left_port;
    int8_t br = config->back_right_reversed ? -config->back_right_port : config->back_right_port;
    int8_t fr = config->front_right_reversed ? -config->front_right_port : config->front_right_port;
    
    Motors::odom_chassis =
        okapi::ChassisControllerBuilder()
            .withMotors({bl, fl}, {br, fr})
            .withGains(
                {0.001, 0, 0.0001}, // distance controller gains
                {0.001, 0, 0.0001}, // turn controller gains
                {0.001, 0, 0.0001}  // angle controller gains (helps drive straight)
            )
            .withSensors(
                okapi::ADIEncoder{'A', 'B'}, // left encoder in ADI ports A & B
                okapi::ADIEncoder{'C', 'D', true},  // right encoder in ADI ports C & D (reversed)
                okapi::ADIEncoder{'E', 'F'}  // middle encoder in ADI ports E & F
            )
            // green gearset, tracking wheel diameter (3.25 in), track (12.7 in), and TPR (360)
            // 0 inch middle encoder distance, and 2.75 inch middle wheel diameter
            .withDimensions(okapi::AbstractMotor::gearset::green, {{3.25_in, 12.7_in, 0_in, 2.75_in}, okapi::quadEncoderTPR})
            .withOdometry() // use the same scales as the chassis (above)
            .buildOdometry(); // build an odometry chassis


    Motors::profile_controller = 
        okapi::AsyncMotionProfileControllerBuilder()
        .withLimits({
          2.882, // Maximum linear velocity of the Chassis in m/s
          2.882, // Maximum linear acceleration of the Chassis in m/s/s
          2.882 // Maximum linear jerk of the Chassis in m/s/s/s
        })
        .withOutput(Motors::odom_chassis)
        .buildMotionProfileController();


    // generate paths in initialize because it takes a while
    auton_lcd.log_to_lcd("generating path a");
    Motors::profile_controller->generatePath({
          {0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
          {3_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
          "A" // Profile name
    );
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

    }
}
