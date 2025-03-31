import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number

CUBE_LIGHT_ID = "cube_light_id"

cube_light_ns = cg.esphome_ns.namespace("cube_light")
CubeLight = cube_light_ns.class_("CubeLight", cg.Component)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CUBE_LIGHT_ID): cv.declare_id(CubeLight),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    cube_light_component = cg.new_Pvariable(config[CUBE_LIGHT_ID])
    await cg.register_component(cube_light_component, config)

    #cg.add(var)
    # https://github.com/FastLED/FastLED/blob/master/library.json
    # 3.3.3 has an issue on ESP32 with RMT and fastled_clockless:
    # https://github.com/esphome/issues/issues/1375
    cg.add_library("fastled/FastLED", "3.3.2")



