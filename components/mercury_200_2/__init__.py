import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_AUTH,
    CONF_PASSWORD,
    CONF_ADDRESS,
    CONF_UPDATE_INTERVAL
)

CODEOWNERS = ["@Shammysha"]

AUTO_LOAD = ["sensor"]

DEPENDENCIES = ["uart"]

CONF_MERCURY_ID = "mercury_200_2_id"

DEFAULTS_UPDATE_INTERVAL = "60s"

mercury_200_2_ns = cg.esphome_ns.namespace("mercury_200_2")
MercuryComponent = mercury_200_2_ns.class_("MercuryComponent", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MercuryComponent),
            cv.Required(CONF_ADDRESS): cv.Range(min=100000, max=999999),
            cv.Optional(CONF_UPDATE_INTERVAL, default=DEFAULTS_UPDATE_INTERVAL): cv.update_interval
        }
    )
    .extend(cv.polling_component_schema("60s"))    
    .extend(uart.UART_DEVICE_SCHEMA)           
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)    
        
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))    
    
