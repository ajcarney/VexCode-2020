/**
 * @file: ./RobotCode/src/objects/subsystems/intakes.hpp
 * @author: Aiden Carney
 * @reviewed_on: 
 * @reviewed_by: 
 *
 * Contains class for the front intakes
 * has methods for intaking
 */

#ifndef __INTAKES_HPP__
#define __INTAKES_HPP__

#include <tuple>
#include <queue>

#include "main.h"

#include "../motors/Motor.hpp"
#include "../sensors/Sensors.hpp"
#include "../sensors/BallDetector.hpp"


typedef enum e_intake_command {
    e_intake,
    e_outtake,
    e_stop,
    e_secure
} intake_command;


/**
 * @see: Motors.hpp
 *
 * contains methods to allow for control of the indexer
 */
class Intakes
{
    private:
        static Motor *l_intake;
        static Motor *r_intake;
        
        static int num_instances;
        
        pros::Task *thread;  // the motor thread
        static std::queue<intake_command> command_queue;
        static std::atomic<bool> lock;

        static void intake_motion_task(void*);
                
    public:
        Intakes(Motor &left, Motor &right);
        ~Intakes();
    
        void intake();
        void outtake();
        void stop();
        void intake_until_secure();        

        void reset_queue();
};




#endif
