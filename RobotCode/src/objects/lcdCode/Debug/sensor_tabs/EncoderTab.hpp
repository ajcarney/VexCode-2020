#ifndef __ENCODERTAB_HPP__
#define __ENCODERTAB_HPP__

#include <string>
#include <vector>

#include "main.h"

#include "../../../sensors/Encoder.hpp"
#include "../../Styles.hpp"


class EncoderDebugger :
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

        lv_obj_t *btn_tare;
        lv_obj_t *btn_tare_label;
        
        static std::vector<Encoder*> encoders;
        static std::vector<std::string> names;
        static std::vector<int> unique_ids;

        /**
         * @param: lv_obj_t* btn -> button that called the funtion
         * @return: lv_res_t -> LV_RES_OK on successfull completion because object still exists
         *
         * button callback function used to tare encoder
         */
        static lv_res_t btn_tare_action(lv_obj_t *btn);

    public:
        EncoderDebugger(lv_obj_t *parent, int x_dim, int y_dim, std::vector<Encoder*> encoders_vec, std::vector<std::string> names_vec);
        ~EncoderDebugger();
        
        /**
         * @return: None
         *
         * updates value of sensors
         */
        void update_info();

};



#endif
