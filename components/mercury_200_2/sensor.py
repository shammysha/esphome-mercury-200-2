
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_VOLTAGE,
    CONF_POWER,
    CONF_CURRENT,
    CONF_VOLTAGE,
    CONF_TOTAL
)

from . import (
    MercuryComponent,
    mercury_200_2_ns,
    CONF_MERCURY_ID
)


AUTO_LOAD = ["mercury_200_2"]

CONF_TARIFFS = [f"tariff{x}" for x in range(1,3)]

ICON_POWER = "mdi:flash"
ICON_CURRENT = "mdi:current-ac"
ICON_VOLTAGE = "mdi:sine-wave"
ICON_TARIFF = "mdi:lightning-bolt"

ICON_TRANSMISSION = "mdi:swap-horizontal"
ICON_CONNECTION = "mdi:lan-connect"
ICON_SESSION = "mdi:sync"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MERCURY_ID): cv.use_id(MercuryComponent),
        cv.Optional(CONF_POWER): sensor.sensor_schema(
            device_class=DEVICE_CLASS_POWER,
            icon=ICON_POWER,
        ),
        cv.Optional(CONF_CURRENT): sensor.sensor_schema(
            device_class=DEVICE_CLASS_CURRENT,
            icon=ICON_CURRENT,
        ),
        cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
            device_class=DEVICE_CLASS_VOLTAGE,
            icon=ICON_VOLTAGE,
        ),        
        cv.Optional(CONF_TOTAL): sensor.sensor_schema(
            icon=ICON_TARIFF,
        ),        
    }
)

for i in CONF_TARIFFS:
    CONFIG_SCHEMA = CONFIG_SCHEMA.extend(
        cv.Schema({
            cv.Optional(CONF_TARIFFS[i]): sensor.sensor_schema( 
                icon=ICON_TARIFF 
            )
        })
    )

async def to_code(config):
    meter = await cg.get_variable(config[CONF_MERCURY_ID])

    if conf := config.get(CONF_POWER):
        sensor = await sensor.new_sensor(config[CONF_POWER])
        cg.add(meter.set_power_sensor(sensor))

    if conf := config.get(CONF_CURRENT):
        sensor = await sensor.new_sensor(config[CONF_CURRENT])
        cg.add(meter.set_current_sensor(sensor))
        
    if conf := config.get(CONF_VOLTAGE):
        sensor = await sensor.new_sensor(config[CONF_VOLTAGE])
        cg.add(meter.set_voltage_sensor(sensor))       
        
    if conf := config.get(CONF_TOTAL):
        sensor = await sensor.new_sensor(config[CONF_TOTAL])
        cg.add(meter.set_total_sensor(sensor))  
        
    if conf := config.get(CONF_TARIFFS[0]):
        sensor = await sensor.new_sensor(config[CONF_TARIFFS[0]])
        cg.add(meter.set_tariff1_sensor(sensor))          
    
    if conf := config.get(CONF_TARIFFS[1]):
        sensor = await sensor.new_sensor(config[CONF_TARIFFS[1]])
        cg.add(meter.set_tariff2_sensor(sensor))

    if conf := config.get(CONF_TARIFFS[2]):
        sensor = await sensor.new_sensor(config[CONF_TARIFFS[2]])
        cg.add(meter.set_tariff3_sensor(sensor))                
                
