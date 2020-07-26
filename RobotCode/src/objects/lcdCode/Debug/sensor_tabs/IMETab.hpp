#ifndef __IMETAB_HPP__
#define __IMETAB_HPP__

#include <string>
#include <vector>

#include "main.h"

#include "../../Styles.hpp"
#include "../../../motors/Motor.hpp"

/**
 * @see: ../Styles.hpp
 *
 * shows tab of IMEs and allows user to tare encoders and see values
 */
class IMEsDebugger :
    virtual Styles
{
    private:
        lv_obj_t *container;
        lv_obj_t *title;
        lv_obj_t *info;

        lv_obj_t *btn_tare;
        lv_obj_t *btn_tare_label;
        
        static std::vector<Motor*> motors;
        static std::vector<std::string> names;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to tare all IMEs
         */
        static lv_res_t btn_tare_action(lv_obj_t *btn);

    public:
        IMEsDebugger(lv_obj_t *parent, int x_dim, int y_dim, std::vector<Motor*> motors_vec, std::vector<std::string> names_vec);
        ~IMEsDebugger();
        
        /**
         * @return: None
         *
         * updates values for IMEs
         */
        void update_info();


};


#endif
