/**
 * @file: ./RobotCode/src/objects/lcdCode/Styles.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * contains base class for styles of gui objects
 *
 */

#ifndef __STYLES__
#define __STYLES__


#include "../../../include/main.h"

//defines colors to use for each style
#define BLUE_BORDER LV_COLOR_BLUE
#define RED_BORDER LV_COLOR_RED
#define BG LV_COLOR_GRAY
#define BUTTON_REL LV_COLOR_SILVER
#define BUTTON_PR LV_COLOR_NAVY
#define TEXT LV_COLOR_WHITE
#define BODY_TEXT LV_COLOR_BLACK
#define SW_INDIC LV_COLOR_HEX(0x9fc8ef)

//allows use of other fonts
#define USE_DEJAVU_12
#define USE_DEJAVU_16
#include "../../../include/fonts/fonts.h"


/**
 * @see: ../../include/fonts/fonts.hpp
 * @see ../fonts/
 *
 * base class that contains different colors and styles to be used throughout
 * the gui
 * designed so that there is no repetion of styles and so they are all in one place
 * designed to be inherited
 */
class Styles
{
    protected:
        //styles
        lv_style_t blue;
        lv_style_t red;
        lv_style_t gray;

        lv_style_t toggle_btn_released;
        lv_style_t toggle_btn_pressed;

        lv_style_t toggle_tabbtn_released;
        lv_style_t toggle_tabbtn_pressed;

        lv_style_t sw_toggled;
        lv_style_t sw_off;
        lv_style_t sw_bg;
        lv_style_t sw_indic;

        lv_style_t heading_text;
        lv_style_t body_text;
        lv_style_t subheading_text;

        lv_style_t lines;

        lv_style_t warn_box_bg;
        lv_style_t warn_box_pressed;
        lv_style_t warn_box_released;

        lv_style_t loader_style;

    public:
        Styles();
        virtual ~Styles();

};

#endif
