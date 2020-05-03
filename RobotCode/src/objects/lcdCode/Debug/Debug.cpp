/**
 * @file: ./RobotCode/src/lcdCode/Debug/Debug.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: Debug.hpp
 *
 * contains function for selecting debug screen
 *
 */

#include "../../../../include/main.h"
#include "../../../../include/api.h"

#include "Debug.hpp"



/**
 * loads all screens at beginning
 * when on titlescreen a tab number is selected and a switch statement is used
 * to let a tab take over
 */
void debug()
{
    bool cont = true;

    TitleScreen dbg1;
    MotorsDebug dbgM;
    SensorsDebug dbgS;
    ControllerDebug dbgC;
    BatteryDebug dbgB;
    FieldControlDebug dbgF;
    Wiring dbgW;
    InternalMotorDebug dbgP;


    while ( cont )
    {
        dbg1.chooseOption();
        if ( dbg1.option == -1 ) //-1 means go back
        {
            cont = false;
            break;
        }

        switch (dbg1.option) //go to selected debug screen
        {
            case 1:
                dbgM.debug();
                break;

            case 2:
                dbgS.debug();
                break;

            case 3:
                dbgC.debug();
                break;

            case 4:
                dbgB.debug();
                break;

            case 5:
                dbgF.debug();
                break;

            case 6:
                dbgW.debug();
                break;
            case 7:
                dbgP.debug();
                break;
        }

    }

}
