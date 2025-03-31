#include "feature_switch.h"

#include "esphome/core/log.h"

namespace esphome
{
    namespace cube_light
    {

        void FeatureSwitch::write_state(bool state)
        {
            parent_->set_feature_enabled(feature_, state);
            this->publish_state(state);
        }

    } // namespace cube_light
} // namespace esphome