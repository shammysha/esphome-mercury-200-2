import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_AUTH,
    CONF_PASSWORD,
    CONF_ADDRESS,
    CONF_UPDATE_INTERVAL,
    CONF_STARTUP_DELAY 
)

CODEOWNERS = ["@Shammysha"]

AUTO_LOAD = ["sensor"]

MULTI_CONF = True

DEPENDENCIES = ["uart"]

CONF_MERCURY_ID = "mercury_200_2_id"
CONF_REQUEST_TIMEOUT = "request_timeout"

DEFAULTS_UPDATE_INTERVAL = "60s"
DEFAULTS_STARTUP_DELAY = "10s"
DEFAULTS_REQUEST_TIMEOUT = "2s"

mercury_200_2_ns = cg.esphome_ns.namespace("mercury_200_2")
MercuryComponent = mercury_200_2_ns.class_("MercuryComponent", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MercuryComponent),
            cv.Required(CONF_ADDRESS): cv.Number(precision=6, scale=0),
            cv.Optional(CONF_UPDATE_INTERVAL, default=DEFAULTS_UPDATE_INTERVAL): cv.update_interval,
            cv.Optional(CONF_STARTUP_DELAY, default=DEFAULTS_STARTUP_DELAY): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_REQUEST_TIMEOUT, default=DEFAULTS_REQUEST_TIMEOUT): cv.positive_time_period_milliseconds
        }
    )
    .extend(cv.COMPONENT_SCHEMA)    
    .extend(uart.UART_DEVICE_SCHEMA)           
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)    
        
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))    
    cg.add(var.set_startup_delay(config[CONF_STARTUP_DELAY]))
    cg.add(var.set_request_timeout(config[CONF_REQUEST_TIMEOUT]))
    
