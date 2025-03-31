import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_SWITCH,
)   
from .. import CubeLight, cube_light_ns, CUBE_LIGHT_ID

FeatureSwitch = cube_light_ns.class_("FeatureSwitch", switch.Switch)

CONF_PATTERN_RANDOM = "pattern_random"
CONF_PATTERN_CUBE = "pattern_cube"
CONF_PATTERN_STARBURST = "pattern_starburst"

CONFIG_SCHEMA = {
    cv.GenerateID(CUBE_LIGHT_ID): cv.use_id(CubeLight),
    cv.Required(CONF_PATTERN_RANDOM): switch.switch_schema(
        FeatureSwitch,
        device_class=DEVICE_CLASS_SWITCH,
    ),
    cv.Required(CONF_PATTERN_CUBE): switch.switch_schema(
        FeatureSwitch,
        device_class=DEVICE_CLASS_SWITCH,
    ),
    cv.Required(CONF_PATTERN_STARBURST): switch.switch_schema(
        FeatureSwitch,
        device_class=DEVICE_CLASS_SWITCH,
    ),
}


async def to_code(config):
    #cube_light_component = await cg.get_variable(config[CUBE_LIGHT_ID])
  
    switch_random = await switch.new_switch(config.get(CONF_PATTERN_RANDOM))
    await cg.register_parented(switch_random, config[CUBE_LIGHT_ID])
    cg.add(switch_random.set_feature(0))

    switch_cube = await switch.new_switch(config.get(CONF_PATTERN_CUBE))
    await cg.register_parented(switch_cube, config[CUBE_LIGHT_ID])
    cg.add(switch_cube.set_feature(1))

    switch_starburst = await switch.new_switch(config.get(CONF_PATTERN_STARBURST))
    await cg.register_parented(switch_starburst, config[CUBE_LIGHT_ID])
    cg.add(switch_starburst.set_feature(2))