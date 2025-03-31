#pragma once

#include "esphome/components/number/number.h"
#include "../cube_light.h"

namespace esphome
{
    namespace cube_light
    {

        class CubeLightNumber : public number::Number, public Parented<CubeLight>
        {
        public:
            PresenceTimeoutNumber() = default;

        protected:
            void control(float value) override;
        };

    } // namespace cube_light
} // namespace esphome