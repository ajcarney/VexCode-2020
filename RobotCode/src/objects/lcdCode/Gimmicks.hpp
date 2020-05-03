/**
 * @file: ./RobotCode/src/objects/lcdCode/Gimmicks.hpp
 * @author: Aiden Carney
 * @reviewed_on: 10/15/2019
 * @reviewed_by: Aiden Carney
 * TODO: fix loading screen, it sometimes does not work
 *
 * contains lcd gimmicks that are used to enhance interface
 *
 */

#ifndef __GIMMICKS_HPP__
#define __GIMMICKS_HPP__

#include <string>

#include "../../../include/main.h"

#include "Styles.hpp"


/**
 * @see: Styles.hpp
 * @see: ./lcdCode
 *
 * used to display warning box
 */
class WarningMessage : virtual Styles
{
    protected:
        /**
         * @param: lv_obj_t* mbox -> message box object
         * @param: const char* txt -> text for message box
         * @return: LV_RES_OK -> if finishes successfully
         *
         * @see: Styles.hpp
         * @see: ./lcdCode
         *
         * sets static int option to positive or negative based on feedback
         *
         */
        static lv_res_t mbox_apply_action(lv_obj_t * mbox, const char * txt);
        static const char* buttons[];
        static int option;

        lv_obj_t *warn_box;


    public:
        WarningMessage();
        virtual ~WarningMessage();

        /**
         * @param: std::string warn_msg -> message that will appear as option
         * @param: lv_obj_t* parent -> the parent that the message box will appear on
         * @return: bool -> if user selected yes or no
         *
         * returns true or false based on what user selects
         * implementation of this is up to user
         *
         */
        bool warn(std::string warn_msg, lv_obj_t *parent);

};


/**
 * @see: Styles.hpp
 * @see: ./lcdCode
 *
 * methods and objects for a loading bar
 */
class Loading : virtual Styles
{
    protected:
        lv_obj_t *loader;

    public:
        Loading();
        ~Loading();

        /**
         * @param: int estimated_duration -> duration that loading should take used to set speed of bar
         * @param: lv_obj_t* parent -> parent object that loading bar will go on
         * @param: int x -> x position of loading bar relative to parent
         * @param int y -> y position of loading bar relative to parent
         * @return: None
         *
         * shows the loader and starts the action of it moving
         *
         */
        void show_load(int estimated_duration, lv_obj_t *parent, int x, int y); //starts the loader

        /**
         * @return: None
         *
         * hides the loader
         * this should be about when the loader is finished
         * Used to keep a smooth transition
         *
         */
        void hide_load(); //ends the loader and hides it
};







#endif
