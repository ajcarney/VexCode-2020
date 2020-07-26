#ifndef __DIGITALINTAB_HPP__
#define __DIGITALINTAB_HPP__

#include <string>
#include <vector>

#include "main.h"

#include "../../Styles.hpp"


/**
 * @see: ../Styles.
 *
 * show value for limit switch
 */
class DigitalInDebugger :
    virtual Styles
{
private:
    lv_obj_t *container;

    lv_obj_t *title1;
    lv_obj_t *title2;

    lv_obj_t *info1;
    lv_obj_t *info2;
    
    static std::vector<pros::ADIDigitalIn*> sensors;
    static std::vector<std::string> names;

    public:
        DigitalInDebugger(lv_obj_t *parent, int x_dim, int y_dim, std::vector<pros::ADIDigitalIn*> sensors_vec, std::vector<std::string> names_vec);
        ~DigitalInDebugger();

        /**
         * @return: None
         *
         * updates value of limit switch
         */
        void update_info();
};


#endif 
