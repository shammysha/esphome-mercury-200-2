#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

static const char *TAG = "mercury_200_2";

namespace esphome {
  namespace mercury_200_2 {

    class MercurySensor : public sensor::Sensor {};

    class MercuryComponent : public PollingComponent, public uart::UARTDevice {
      public:
        void setup() override;
        void dump_config() override;
        void update() override;

        void set_address(int address) { this->address_ = address; }

        void register_power_sensor(MercurySensor *sensor) { this->power_ = sensor; }
        void register_current_sensor(MercurySensor *sensor) { this->current_ = sensor; }
        void register_voltage_sensor(MercurySensor *sensor) { this->voltage_ = sensor; }
        void register_tariff1_sensor(MercurySensor *sensor) { this->tariff1_ = sensor; }
        void register_tariff2_sensor(MercurySensor *sensor) { this->tariff2_ = sensor; }
        void register_tariff3_sensor(MercurySensor *sensor) { this->tariff3_ = sensor; }
        void register_total_sensor(MercurySensor *sensor) { this->total_ = sensor; }


      private:
        int address_;

        sensor::Sensor *current_{nullptr};
        sensor::Sensor *power_{nullptr};
        sensor::Sensor *voltage_{nullptr};
        sensor::Sensor *tariff1_{nullptr};
        sensor::Sensor *tariff2_{nullptr};
        sensor::Sensor *tariff3_{nullptr};
        sensor::Sensor *total_{nullptr};


        unsigned char electrical_parameters[7]; // Байты на получене мгновенных значений
        unsigned char tarif[7]; // Байты на получение тарифа

        byte Re_buf[100];
        int counter{0};
        int step{0};

        typedef unsigned char uchar;

        long pow(long a, int s);

        template<size_t N = 2> long readLong(uchar *inp);
        template<size_t N = 2> double readDouble(uchar *inp, int del);

        uint16_t crc16(const uint8_t *data, uint8_t len);

        void calculateParams(unsigned char *frame, unsigned char comm);

        void main_uart_read(byte *command);

        void set_step();
    };
  }}
