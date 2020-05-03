/**
 * @file: ./RobotCode/src/objects/lcdCode/Styles.cpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 *
 * @see: Styles.hpp
 *
 * contains base class for styles for gui
 *
 */

#include "../../../include/main.h"
#include "../../../include/api.h"

#include "Styles.hpp"


Styles::Styles()
{
//red style
    lv_style_copy(&red, &lv_style_scr);
    red.body.main_color = LV_COLOR_RED;
    red.body.grad_color = LV_COLOR_RED;
    red.body.border.color = LV_COLOR_RED;

//blue style
    lv_style_copy(&blue, &lv_style_scr);
    blue.body.main_color = LV_COLOR_BLUE;
    blue.body.grad_color = LV_COLOR_BLUE;
    blue.body.border.color = LV_COLOR_BLUE;

//gray style
    lv_style_copy(&gray, &lv_style_scr);
    gray.body.main_color = BG;
    gray.body.grad_color = BG;
    gray.body.border.color = BG;
    gray.body.border.width = 10;

//style for when the button is not pressed
    lv_style_copy(&toggle_btn_released, &lv_style_plain);
    toggle_btn_released.body.main_color = BUTTON_REL;
    toggle_btn_released.body.grad_color = BUTTON_REL;
    toggle_btn_released.body.border.color = BUTTON_REL;
    toggle_btn_released.body.border.width = 2;
    toggle_btn_released.body.border.opa = LV_OPA_0;
    toggle_btn_released.body.radius = 5;
    toggle_btn_released.text.color = TEXT;

//style for when the button is pressed
    lv_style_copy(&toggle_btn_pressed, &lv_style_plain);
    toggle_btn_pressed.body.main_color = BUTTON_PR;
    toggle_btn_pressed.body.grad_color = BUTTON_PR;
    toggle_btn_pressed.body.border.color = BUTTON_REL;
    toggle_btn_pressed.text.color = TEXT;

//style for when tabview button is not pressed
    lv_style_copy(&toggle_tabbtn_released, &lv_style_plain);
    toggle_tabbtn_released.body.main_color = BUTTON_REL;
    toggle_tabbtn_released.body.grad_color = BUTTON_REL;
    toggle_tabbtn_released.body.border.color = BUTTON_REL;
    toggle_tabbtn_released.body.border.width = 2;
    toggle_tabbtn_released.body.border.opa = LV_OPA_0;
    toggle_tabbtn_released.text.color = TEXT;
    toggle_tabbtn_released.text.font = &dejavu_12;

//style for when tabview button is pressed
    lv_style_copy(&toggle_tabbtn_pressed, &lv_style_plain);
    toggle_tabbtn_pressed.body.main_color = BUTTON_PR;
    toggle_tabbtn_pressed.body.grad_color = BUTTON_PR;
    toggle_tabbtn_pressed.body.border.color = BUTTON_REL;
    toggle_tabbtn_pressed.text.color = TEXT;
    toggle_tabbtn_pressed.text.font = &dejavu_12;

//switch on
    lv_style_copy(&sw_toggled, &lv_style_pretty_color);
    sw_toggled.body.radius = LV_RADIUS_CIRCLE;
    sw_toggled.body.shadow.width = 4;
    sw_toggled.body.shadow.type = LV_SHADOW_BOTTOM;

//switch off
    lv_style_copy(&sw_off, &lv_style_pretty);
    sw_off.body.radius = LV_RADIUS_CIRCLE;
    sw_off.body.shadow.width = 4;
    sw_off.body.shadow.type = LV_SHADOW_BOTTOM;

//switch background
    lv_style_copy(&sw_bg, &lv_style_pretty);
    sw_bg.body.radius = LV_RADIUS_CIRCLE;

//switch indicator
    lv_style_copy(&sw_indic, &lv_style_pretty_color);
    sw_indic.body.radius = LV_RADIUS_CIRCLE;
    sw_indic.body.main_color = SW_INDIC;
    sw_indic.body.grad_color = SW_INDIC;
    sw_indic.body.padding.hor = 0;
    sw_indic.body.padding.ver = 0;

//heading text
    lv_style_copy(&heading_text, &lv_style_plain);
    heading_text.text.letter_space = 2;
    heading_text.text.line_space = 1;
    heading_text.text.color = TEXT;
    heading_text.text.font = &lv_font_dejavu_20;

//body text
    lv_style_copy(&body_text, &lv_style_plain);
    body_text.text.letter_space = 2;
    body_text.text.line_space = 1;
    body_text.text.color = BODY_TEXT;
    body_text.text.font = &dejavu_12;

//subheading text
    lv_style_copy(&subheading_text, &lv_style_plain);
    subheading_text.text.letter_space = 2;
    subheading_text.text.line_space = 1;
    subheading_text.text.color = BODY_TEXT;
    subheading_text.text.font = &dejavu_16;

//style for lines
    lv_style_copy(&lines, &lv_style_plain);
    lines.line.color = BUTTON_PR;
    lines.line.width = 5;

//styles for warning box
    //background
    lv_style_copy(&warn_box_bg, &lv_style_pretty);
    warn_box_bg.body.main_color = LV_COLOR_MAKE(0xf5, 0x45, 0x2e);
    warn_box_bg.body.grad_color = LV_COLOR_MAKE(0xb9, 0x1d, 0x09);
    warn_box_bg.body.border.color = LV_COLOR_MAKE(0x3f, 0x0a, 0x03);
    warn_box_bg.text.color = LV_COLOR_WHITE;
    warn_box_bg.body.padding.hor = 12;
    warn_box_bg.body.padding.ver = 8;
    warn_box_bg.body.shadow.width = 8;

    //button not pressed
    lv_style_copy(&warn_box_released, &lv_style_btn_rel);
    warn_box_released.body.empty = 1;
    warn_box_released.body.border.color = LV_COLOR_WHITE;
    warn_box_released.body.border.width = 2;
    warn_box_released.body.border.opa = LV_OPA_70;
    warn_box_released.body.padding.hor = 12;
    warn_box_released.body.padding.ver = 8;

    //button being pressed
    lv_style_copy(&warn_box_pressed, &warn_box_released);
    warn_box_pressed.body.empty = 0;
    warn_box_pressed.body.main_color = LV_COLOR_MAKE(0x5d, 0x0f, 0x04);
    warn_box_pressed.body.grad_color = LV_COLOR_MAKE(0x5d, 0x0f, 0x04);

//style for loader
    lv_style_copy(&loader_style, &lv_style_plain);
    loader_style.line.width = 10;  //10 px thick arc
    loader_style.line.color = LV_COLOR_HEX3(0x258); //Blueish arc color

    loader_style.body.border.color = LV_COLOR_HEX3(0xBBB); //Gray background color
    loader_style.body.border.width = 10;
    loader_style.body.padding.hor = 0;

}

Styles::~Styles()
{

}
