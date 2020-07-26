/**
 * @file: ./RobotCode/src/objects/sensors/Sensors.cpp
 * @author: Aiden Carney
 * @reviewed_on: 2/29/2020
 * @reviewed_by: Aiden Carney
 *
 * @see: Sensors.hpp
 *
 * contains definitions for sensors and implementation for sensor class
 */

#include "Sensors.hpp"
#include "../../Configuration.hpp"
#include "../serial/Logger.hpp"


namespace Sensors 
{
    Encoder right_encoder{RIGHT_ENC_TOP_PORT, RIGHT_ENC_BOTTOM_PORT, false};
    Encoder left_encoder{LEFT_ENC_TOP_PORT, LEFT_ENC_BOTTOM_PORT, true};
    Encoder strafe_encoder{STRAFE_ENC_TOP_PORT, STRAFE_ENC_BOTTOM_PORT, false};
    AnalogInSensor potentiometer{POTENTIOMETER_PORT};
    pros::ADIDigitalIn limit_switch{LIMITSWITCH_PORT};
    pros::Vision vision_sensor{VISIONSENSOR_PORT};
    pros::Imu imu{IMU_PORT};
    
    
    
    
    void log_data()
    {
        Logger logger;
        log_entry entry;
        entry.content = ("[INFO], " + std::to_string(pros::millis()) 
            + ", ADI Sensor Data"
            +  ", Right_Enc: " + std::to_string(right_encoder.get_absolute_position(false))
            +  ", Left_Enc: " + std::to_string(left_encoder.get_absolute_position(false))
            +  ", Strafe_Enc: " + std::to_string(strafe_encoder.get_absolute_position(false))
            //+  " Pot: " + std::to_string(potentiometer.get_value(false))
            +  ", Limit_Switch: " + std::to_string(limit_switch.get_value()));
        entry.stream = "clog";
        logger.add(entry);
    }
    
    
    
    
    int check_for_cube()
    {
        //the color signature when checking for each of the cube colors
        //pros::vision_object_s_t check_green = vision_sensor.get_by_sig(0, 1);
        //pros::vision_object_s_t check_orange = vision_sensor.get_by_sig(0, 2);
        //pros::vision_object_s_t check_purple = vision_sensor.get_by_sig(0, 3);

        //std::cout << check_green.signature << " " << "\n";//check_purple.signature << "\n";
        if ( Sensors::vision_sensor.get_by_sig(0, 1).signature != 255 )
        {
            Sensors::vision_sensor.set_led(0x4CBB17);
            return 1;
        }
        else if ( Sensors::vision_sensor.get_by_sig(0, 2).signature != 255 )
        {
            Sensors::vision_sensor.set_led(0xFFAE42);
            return 2;
        }
        else if ( Sensors::vision_sensor.get_by_sig(0, 3).signature != 255 )
        {
            Sensors::vision_sensor.set_led(0x800080);
            return 3;
        }
        else 
        {
            Sensors::vision_sensor.set_led(0xFFFFFF);
            return 0;        
        }

    }

}

