#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

static const char *TAG = "mercury_200_2";

namespace esphome {
  namespace mercury_200_2 {

    class MercuryComponent : public Component, public uart::UARTDevice {
      public:
        void setup() override;
        void dump_config() override;
        void loop() override;
        void set_address(int address) { this->address_ = address; }
        void set_startup_delay(int delay) { this->delay_ = delay; }
        void set_update_interval(int interval) { this->interval_ = interval; }
        void set_request_timeout(int timeout) { this->timeout_ = timeout; }

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
        int interval_;
        int delay_;
        int timeout_{2000};
        int awaiting_;

        unsigned long starttime_;
        unsigned long last_updated_{0};

        unsigned char metrics_[7]; // Байты на получене мгновенных значений
        unsigned char tariffs_[7]; // Байты на получение тарифа

        uint8_t buf_[100];
        uint8_t counter_{0};

        void calculateParams(unsigned char *frame, unsigned char comm);
        void publish();
        void next_state(State state);
        std::string state_str(State state);
    };
  }
}
