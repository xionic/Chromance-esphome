#include "cube_light_number.h"

namespace esphome
{
    namespace cube_light
    {

        void CubeLightNumber::control(float value)
        {
            this->publish_state(value);
            this->parent_->set_presence_timeout();
        }

    } // namespace cube_light
} // namespace esphome