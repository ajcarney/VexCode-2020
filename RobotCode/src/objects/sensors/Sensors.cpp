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
#include "../motors/Motors.hpp"
#include "../../Configuration.hpp"
#include "../serial/Logger.hpp"


namespace Sensors
{
    Encoder right_encoder{RIGHT_ENC_TOP_PORT, RIGHT_ENC_BOTTOM_PORT, false};
    Encoder left_encoder{LEFT_ENC_TOP_PORT, LEFT_ENC_BOTTOM_PORT, false};
    Encoder strafe_encoder{STRAFE_ENC_TOP_PORT, STRAFE_ENC_BOTTOM_PORT, true};
    
    AnalogInSensor line_tracker_top{DETECTOR_TOP_PORT};
    AnalogInSensor line_tracker_middle{DETECTOR_MIDDLE_PORT}; 
    AnalogInSensor line_tracker_bottom{DETECTOR_BOTTOM_PORT};
    
    // BallDetector ball_detector{DETECTOR_TOP_PORT, DETECTOR_MIDDLE_PORT, DETECTOR_BOTTOM_PORT, VISIONSENSOR_PORT, Configuration::get_instance()->filter_threshold};
    BallDetector ball_detector{
        line_tracker_top,
        line_tracker_middle,
        line_tracker_bottom,
        OPTICAL_PORT,
        Configuration::get_instance()->filter_threshold
    };
    
    pros::Imu imu{IMU_PORT};
    bool imu_is_calibrated = false;

    void calibrate_imu() {
        bool calibrated = false;
        while(!calibrated) {  // block until imu is connected and calibrated
            imu.reset();  // calibrate imu
            while(imu.is_calibrating()) {
                pros::delay(10);
                calibrated = true;
            }
        }
        imu_is_calibrated = true;
    }

    void log_data() {
        Logger logger;
        log_entry entry;
        entry.content = ("[INFO], " + std::to_string(pros::millis())
            + ", Sensor Data"
            +  ", Right_Enc: " + std::to_string(right_encoder.get_absolute_position(false))
            +  ", Left_Enc: " + std::to_string(left_encoder.get_absolute_position(false))
            +  ", Top Detector" + std::to_string(ball_detector.locate_balls().at(0))
            +  ", Middle Detector" + std::to_string(ball_detector.locate_balls().at(1))
            +  ", Bottom Detector" + std::to_string(ball_detector.locate_balls().at(2))
        );
        entry.stream = "clog";
        logger.add(entry);
    }
    
    /**
     * takes the average of each side of the drive encoders
     * hopefully to reduce error of encoders
     * returns tuple of encoder values
     */
    std::tuple<double, double> get_average_encoders(int l_id, int r_id) {
        // use a weighted average to merge all encoders on the robot for a hopefully more accurate reading
        double left_encoder_val = (0 * Motors::front_left.get_encoder_position() * Motors::chassis_gear_ratio) + (0 * Motors::back_left.get_encoder_position() * Motors::chassis_gear_ratio) + (1 * Sensors::left_encoder.get_position(l_id));
        double right_encoder_val = (0 * Motors::front_right.get_encoder_position() * Motors::chassis_gear_ratio) + (0 * Motors::back_right.get_encoder_position() * Motors::chassis_gear_ratio) + (1 * Sensors::right_encoder.get_position(r_id));

        return {left_encoder_val, right_encoder_val};
    }


}
