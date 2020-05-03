/**
 * @file: ./RobotCode/src/objects/lcdCode/TemporaryScreen.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: deprecate, possibly move somewhere else, file does very little and could be merged elsewhere
 *
 * contains a global static screen that can be loaded so that the one screen needs to
 * be loaded at all times rule is not broken
 *
 */

#ifndef __TEMPORARYSCREEN_HPP__
#define __TEMPORARYSCREEN_HPP__

#include "../../../include/main.h"


struct tempScreen
{
    static lv_obj_t *temp_screen;
};



#endif
