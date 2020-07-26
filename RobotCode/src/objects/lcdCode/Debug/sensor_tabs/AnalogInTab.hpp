#ifndef __ANALOGINTAB_HPP__
#define __ANALOGINTAB_HPP__

#include <string>
#include <vector>

#include "main.h"

#include "../../../sensors/AnalogInSensor.hpp"
#include "../../Styles.hpp"


class AnalogInDebugger :
    virtual Styles
{
    private:
        lv_obj_t *container;

        lv_obj_t *title1;
        lv_obj_t *title2;
        lv_obj_t *title3;

        lv_obj_t *info1;
        lv_obj_t *info2;
        lv_obj_t *info3;

        lv_obj_t *btn_calibrate;
        lv_obj_t *btn_calibrate_label;
        
        static std::vector<AnalogInSensor*> sensors;
        static std::vector<std::string> names;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to calibrate sensor
         */
        static lv_res_t btn_calibrate_action(lv_obj_t *btn);

    public:
        AnalogInDebugger(lv_obj_t *parent, int x_dim, int y_dim, std::vector<AnalogInSensor*> sensors_vec, std::vector<std::string> names_vec);
        ~AnalogInDebugger();
        
        /**
         * @return: None
         *
         * updates value of sensors
         */
        void update_info();

};



#endif
