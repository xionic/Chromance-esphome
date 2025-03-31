#include "cube_light_output.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace cube_light
    {

        void CubeLightOutput::setup() {}

        light::LightTraits CubeLightOutput::get_traits()
        {
            auto traits = light::LightTraits();
            traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
            return traits;
        }

        void CubeLightOutput::write_state(light::LightState *state) 
        {
            if(!state->current_values.is_on()){
                parent_->is_on = false;
            } else {
                parent_->is_on = true;
            }
        }

        void CubeLightOutput::dump_config() { ESP_LOGCONFIG(TAG, "cube_light_output.light config"); }

    } // namespace cube_light
} // namespace esphome  