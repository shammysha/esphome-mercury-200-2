# Получение показаний со счетчика Меркурий 200.2

Пример использования:

```

external_components:
  - source:
      type: git
      url: https://github.com/shammysha/esphome-mercury-200-2
    components: [mercury_200_2]
    refresh: 1s
    
mercury_200_2:
  id: mercury
  uart_id: uart_meter
  # последние 6 цифр серийного номера
  address: ${meter_address}
  
  update_interval: ${meter_update_interval}
  startup_delay: ${meter_startup_delay}

sensor:
  - platform: mercury_200_2
    mercury_200_2_id: mercury

    voltage: 
      name: ${devicename} Voltage

    current: 
      name: ${devicename} Current

    power:
      name: ${devicename} Power

    tariff1:
      name: ${devicename} Tariff1    

    tariff2:
      name: ${devicename} Tariff2   
    
    tariff3:
      name: ${devicename} Tariff3

    total:
      name: ${devicename} Total  
```