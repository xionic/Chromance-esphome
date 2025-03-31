#pragma once

#include "esphome/components/light/light_output.h"
#include "../cube_light.h"

namespace esphome
{
    namespace cube_light
    {

        class CubeLightOutput : public light::LightOutput, public Component, public Parented<CubeLight>
        {
        public:
            void setup() override;
            light::LightTraits get_traits() override;
            void write_state(light::LightState *state) override;
            void dump_config() override;
            void set_parent(CubeLight *cl){
                parent_ = cl;
            }
        protected:
            CubeLight *parent_;
        };
    } // namespace cube_light
} // namespace esphome