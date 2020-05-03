/**
 * @file: ./RobotCode/src/objects/lcdCode/gui.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/25/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: gui.hpp
 *
 * contains implementation of gui
 *
 */

#include "../../../include/main.h"

#include "gui.hpp"
#include "../../Autons.hpp"
#include "../motors/MotorThread.hpp"
#include "../../DriverControl.hpp"
#include "TemporaryScreen.hpp"


/**
 * iterates through selecting for user to go through stages selecting an auton or the debugger
 * and then all the config options
 * loads all screens at start so there are no mem management issues
 * finishes when all options are chosen
 */
int chooseAuton()
{
    Autons auton_data;
    
    //init screens so that loading time is faster
    SelectionScreen scr1;
    OptionsScreen scr2;
    PrepScreen scr3;
    DriverControlLCD scr4;

    int finalAutonChoice = 0;
    int auton = 1;
    bool confirm = false;
    int interval = 20;

    while ( !(finalAutonChoice) ) //allows user to go to previous screen
    {
        scr2.back = false;

        auton = scr1.selectAuton( auton ); //get auton option

        if ( auton == auton_data.driver_control_num ) //if prog with no auton is selected
        {
            finalAutonChoice = 1;
        }
        
        else if ( auton == auton_data.debug_auton_num ) //if debugger is selected
        {
            //starts driver control for debugging purposes
            pros::Task driver_control_task (driver_control,
                                           (void*)NULL,
                                           TASK_PRIORITY_DEFAULT,
                                           TASK_STACK_DEPTH_DEFAULT,
                                           "DriverControlTask");

            debug();

            //ends driver control because it should not be enabled when
            //auton is being selected
            driver_control_task.remove();
        }
        
        else
        {
            while ( !(scr2.back) && !(finalAutonChoice) )
            //if user selects a program with an auton
            {
                autonConfig cnfg = scr2.getOptions( auton ); //get config options

                if ( !(scr2.back) ) //if user does not want to go back from screen 2
                {

                    scr3.getConfirmation( auton ); //gets confirmation from user
                    if ( scr3.confirm )
                    {
                        finalAutonChoice = auton;
                        //scr4.updateLabels(interval, finalAutonChoice);
                    }


                }
                else 
                {
                    break;
                }


            }
        }

    }



    return finalAutonChoice;
}
