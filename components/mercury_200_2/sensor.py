
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_TYPE
)
from . import (
    MercuryComponent,
    mercury_200_2_ns,
    CONF_MERCURY_ID
)

AUTO_LOAD = ["mercury_200_2"]

POWER_STR = "POWER"
CURRENT_STR = "CURRENT"
VOLTAGE_STR = "VOLTAGE"
TARIFF1_STR = "TARIFF1"
TARIFF2_STR = "TARIFF2"
TARIFF3_STR = "TARIFF3"
TOTAL_STR = "TOTAL"


MercurySensor = mercury_200_2_ns.class_("MercurySensor", sensor.Sensor)

CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(
        MercurySensor,
    ).extend(
        {
            cv.GenerateID(CONF_MERCURY_ID): cv.use_id(MercuryComponent),
            cv.Required(CONF_TYPE): cv.Any(POWER_STR, CURRENT_STR, VOLTAGE_STR, TARIFF1_STR, TARIFF2_STR, TARIFF3_STR, TOTAL_STR)
        }
    )
)


async def to_code(config):
    component = await cg.get_variable(config[CONF_MERCURY_ID])
    var = await sensor.new_sensor(config)
    
    if config[CONF_TYPE] == POWER_STR:
        cg.add(component.register_power_sensor(var))
    if config[CONF_TYPE] == CURRENT_STR:
        cg.add(component.register_current_sensor(var))
    if config[CONF_TYPE] == VOLTAGE_STR:
        cg.add(component.register_voltage_sensor(var))
    if config[CONF_TYPE] == TARIFF1_STR:
        cg.add(component.register_tariff1_sensor(var))
    if config[CONF_TYPE] == TARIFF2_STR:
        cg.add(component.register_tariff2_sensor(var))
    if config[CONF_TYPE] == TARIFF3_STR:
        cg.add(component.register_tariff3_sensor(var))
    if config[CONF_TYPE] == TOTAL_STR:
        cg.add(component.register_total_sensor(var))

        
        
