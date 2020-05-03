/**
 * @file: ./RobotCode/src/objects/lcdCode/TemporaryScreen.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: TemporaryScreen.hpp
 *
 * global screen part of a struct that can be loaded
 * has no parent so that it is always valid
 *
 */

#include "TemporaryScreen.hpp"
#include "Styles.hpp"
#include "../../../include/main.h"


lv_obj_t *tempScreen::temp_screen = lv_obj_create(NULL, NULL);
