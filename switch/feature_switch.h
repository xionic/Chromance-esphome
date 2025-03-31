#pragma once

#include "esphome/components/switch/switch.h"
#include "../cube_light.h"


namespace esphome
{
    namespace cube_light
    {

        class FeatureSwitch : public switch_::Switch, public Parented<CubeLight>
        {
        public:
            FeatureSwitch() = default;
            void set_feature(int feature){feature_ = feature;}

        protected:
            void write_state(bool state) override;

            int feature_;
        };

    } // namespace cube_light
} // namespace esphome