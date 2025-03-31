import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_OUTPUT_ID, CONF_ID

from .. import CubeLight, cube_light_ns, CUBE_LIGHT_ID

CubeLightOutput = cube_light_ns.class_("CubeLightOutput", cg.Component, light.LightOutput)

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(CubeLightOutput),
        cv.GenerateID(CUBE_LIGHT_ID): cv.use_id(CubeLight),     
    }
)


async def to_code(config):

    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await cg.register_component(var, config)
    await light.register_light(var, config)
    await cg.register_parented(var, config[CUBE_LIGHT_ID])

    #cg.add(var)
    # https://github.com/FastLED/FastLED/blob/master/library.json
    # 3.3.3 has an issue on ESP32 with RMT and fastled_clockless:
    # https://github.com/esphome/issues/issues/1375
    cg.add_library("fastled/FastLED", "3.3.2")



