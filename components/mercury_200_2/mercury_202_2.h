#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

static const char *TAG = "mercury_200_2";

namespace esphome {
  namespace mercury_200_2 {

    long pow(long a, int s);

    template<size_t N = 2> long readLong(unsigned char *inp);
    template<size_t N = 2> double readDouble(unsigned char *inp, int del);

    uint16_t crc16(const uint8_t *data, uint8_t len);

    class MercurySensor : public sensor::Sensor {};

    class MercuryComponent : public PollingComponent, public uart::UARTDevice {
      public:
        void setup() override;
        void dump_config() override;
        void update() override;

        void set_address(int address) { this->address_ = address; }

#ifdef USE_SENSOR
        SUB_SENSOR(power)
        SUB_SENSOR(current)
        SUB_SENSOR(voltage)
        SUB_SENSOR(tariff1)
        SUB_SENSOR(tariff2)
        SUB_SENSOR(tariff3)
        SUB_SENSOR(total)
#endif

      private:
        int address_;

        unsigned char electrical_parameters[7]; // Байты на получене мгновенных значений
        unsigned char tarif[7]; // Байты на получение тарифа

        byte Re_buf[100];
        int counter{0};
        int step{0};

        void calculateParams(unsigned char *frame, unsigned char comm);

        void main_uart_read(byte *command);

        void set_step();
    };
  }}
