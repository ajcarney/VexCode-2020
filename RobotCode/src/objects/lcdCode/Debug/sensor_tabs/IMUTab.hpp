#ifndef __IMUTAB_HPP__
#define __IMUTAB_HPP__

#include <string>
#include <vector>

#include "main.h"

#include "../../../sensors/AnalogInSensor.hpp"
#include "../../Styles.hpp"


class IMUDebugger :
    virtual Styles
{
    private:
        lv_obj_t *container;

        lv_obj_t *info1;
        lv_obj_t *info2;

        lv_obj_t *btn_calibrate;
        lv_obj_t *btn_calibrate_label;
        
        static pros::Imu *imu;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to calibrate sensor
         */
        static lv_res_t btn_calibrate_action(lv_obj_t *btn);

    public:
        IMUDebugger(lv_obj_t *parent, int x_dim, int y_dim, pros::Imu *imu_sensor);
        ~IMUDebugger();
        
        /**
         * @return: None
         *
         * updates value of sensors
         */
        void update_info();

};



#endif
