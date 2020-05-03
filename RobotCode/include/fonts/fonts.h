/**
 * @file: ./RobotCode/include/fonts/fonts.h
 * @author: Aiden Carney
 * @reviewed_on: 10/16/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: ../src/objects/lcdCode/fonts/
 *
 * contains definitions for fonts to use for lvgl
 * this is used to make gui more interesting and provide more contrast and have
 * the ability to fit more data because of a smaller font
 */

#ifndef __FONTS_H__
#define __FONTS_H__


#ifdef USE_DEJAVU_9
LV_FONT_DECLARE(dejavu_9);
extern lv_font_t dejavu_9;
#endif

#ifdef USE_DEJAVU_12
LV_FONT_DECLARE(dejavu_12);
extern lv_font_t dejavu_12;
#endif

#ifdef USE_DEJAVU_16
LV_FONT_DECLARE(dejavu_16);
extern lv_font_t dejavu_16;
#endif

#endif
