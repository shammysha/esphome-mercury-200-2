#pragma once

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

    class MercuryComponent : public PollingComponent, public uart::UARTDevice {
      public:
        void setup() override;
        void dump_config() override;
        void update() override;
        void loop() override;
        void set_address(int address) { this->address_ = address; }
        void set_startup_delay(int delay) { this->delay_ = delay; }

#ifdef USE_SENSOR
        SUB_SENSOR(power)
        SUB_SENSOR(current)
        SUB_SENSOR(voltage)
        SUB_SENSOR(tariff1)
        SUB_SENSOR(tariff2)
        SUB_SENSOR(tariff3)
        SUB_SENSOR(total)
#endif

      protected:
        enum class State : uint8_t {
          NOT_READY,
          IDLE,
          SEND_METRICS_CMD,
          WAIT_METRICS_INFO,
          SEND_TARIFFS_CMD,
          WAIT_TARIFFS_INFO
        } state_{State::NOT_READY};

      private:
        int address_;
        int delay_;
        int starttime_;

        unsigned char metrics_[7]; // Байты на получене мгновенных значений
        unsigned char tariffs_[7]; // Байты на получение тарифа

        uint8_t buf_[100];
        uint8_t counter_{0};

        void calculateParams(unsigned char *frame, unsigned char comm);
        void publish();
    };
  }
}
