from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_AUTH,
    CONF_PASSWORD,
    CONF_ADDRESS,
    DOMAIN,
    COMPONENT_CLASS
)

CODEOWNERS = ["@Shammysha"]

AUTO_LOAD = ["sensor"]

DEPENDENCIES = ["uart"]

DOMAIN = "mercury_200_2";
COMPONENT_CLASS = "MercuryComponent";

CONF_MERCURY_ID = "mercury_200_2_id"

mercury_200_2_ns = cg.esphome_ns.namespace(DOMAIN)
MercuryComponent = mercury_200_2_ns.class_(
    COMPONENT_CLASS, cg.PollingComponent, uart.UARTDevice
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MercuryComponent),
            cv.Required(CONF_ADDRESS): vol.Range(min=100000, max=999999)            
        }
    )
    .extend(cv.POLLING_COMPONENT_SCHEMA)    
    .extend(uart.UART_DEVICE_SCHEMA)           
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_address(config[CONF_ADDRESS]))
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)    
    
