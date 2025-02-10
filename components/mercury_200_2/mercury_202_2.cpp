#include "mercury_202_2.h"
#include "esphome/core/log.h"

namespace esphome {
  namespace mercury_200_2 {

    long pow(long a, int s) {
      long out = 1;
      for (int i = 0; i < s; i++)
        out *= a;
      return out;
    }

    template<size_t N = 2> long readLong(unsigned char *inp) {
      long out = 0;

      for (int i = 0; i < N; i++) {
        unsigned char v = inp[i];
        int p = pow(10, ((N - 1) - i) * 2);
        out += (((v >> 4) & 15) * 10 + (v & 15)) * p;
      }

      return out;
    }

    template<size_t N = 2> double readDouble(unsigned char *inp, int del) {
      return (double) readLong<N>(inp) / del;
    }

    uint16_t crc16(const uint8_t *data, uint8_t len) {
      uint16_t crc = 0xFFFF;
      while (len--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++) {
          if ((crc & 0x01) != 0) {
            crc >>= 1;
            crc ^= 0xA001;
          } else {
            crc >>= 1;
          }
        }
      }
      return crc;
    }

    void MercuryComponent::setup() {
      this->calculateParams(this->metrics_, 0x63);
      this->calculateParams(this->tariffs_, 0x27);
    }

    void MercuryComponent::dump_config() {
      ESP_LOGCONFIG(TAG, "Mercury 200.2:");
      ESP_LOGCONFIG(TAG, "  Address: %s", this->address_);
      ESP_LOGCONFIG(TAG, "  Startup delay: %s", this->delay_);
      ESP_LOGCONFIG(TAG, "  Update interval: %s", this->interval_);
    }

    void MercuryComponent::calculateParams(unsigned char *frame, unsigned char comm) {
      frame[0] = 0x00;
      frame[1] = this->address_ >> 16;
      frame[2] = this->address_ >> 8;
      frame[3] = this->address_;
      frame[4] = comm;
      auto crc = crc16(frame, 5);
      frame[5] = crc >> 0;
      frame[6] = crc >> 8;
    }

    void MercuryComponent::loop() {
      if (!this->is_ready()) {
        return;
      }

      unsigned long start = millis();
      unsigned long d = start;
      if (!this->starttime_) {
        this->starttime_ = millis();
      }

      switch (this->state_) {

        case State::NOT_READY: {
          if (start > this->starttime_ + this->delay_) {
            this->state_ = State::IDLE;
          }
        } break;

        case State::IDLE: {
          if (start > this->last_updated_ + this->interval_) {
            this->state_ = State::SEND_METRICS_CMD;
            this->last_updated_ = start;

            ESP_LOGD(TAG, "Starting data collection");
          }
        } break;

        case State::SEND_METRICS_CMD: {
          write_array(this->metrics_, 7);
          flush();

          this->state_ = State::WAIT_METRICS_INFO;
          this->counter_ = 0;

          delay(300);
        } break;

        case State::WAIT_METRICS_INFO: {
          while(available() > 0) {
            while (d < start + 300) {
              d = millis();

              this->buf_[this->counter_] = read();
              this->counter_++;
            }
            return;
          }
          if (this->counter_ >= 23) {
            this->state_ = State::SEND_TARIFFS_CMD;
            this->publish();
          }
        } break;

        case State::SEND_TARIFFS_CMD: {
          write_array(this->tariffs_, 7);
          flush();
          this->state_ = State::WAIT_TARIFFS_INFO;
          this->counter_ = 0;

          delay(300);
        } break;

        case State::WAIT_TARIFFS_INFO: {
          while(available() > 0) {
            while (d < start + 300) {
              d = millis();

              this->buf_[this->counter_] = read();
              this->counter_++;
            }
            return;
          }

          if (this->counter_ >= 23) {
            this->state_ = State::IDLE;
            this->publish();
          }
        } break;

        default:
          break;
      }
    }

    void MercuryComponent::publish() {
      if (this->buf_[0] == 0x00 && this->buf_[4] == 0x63) {

        double V = readDouble(&this->buf_[5], 10); //  Парсинг байтов и перевод в нормальные значения
        double A = readDouble(&this->buf_[7], 100); // Парсинг байтов  и перевод в нормальные значения
        double W = readDouble<3>(&this->buf_[9], 1000); // Парсинг байтов  и перевод в нормальные значения

#ifdef USE_SENSOR
        if (this->power_sensor_) {
          this->power_sensor_->publish_state(W);
        }
        if (this->current_sensor_) {
          this->current_sensor_->publish_state(A);
        }
        if (this->voltage_sensor_) {
          this->voltage_sensor_->publish_state(V);
        }
#endif
      }

      if (this->buf_[0] == 0x00 && this->buf_[4] == 0x27) {
        double T1 = readDouble<4>(&this->buf_[5], 100);
        double T2 = readDouble<4>(&this->buf_[9], 100);
        double T3 = readDouble<4>(&this->buf_[13], 100);
        double sum = T1 + T2 + T3;

#ifdef USE_SENSOR
        if (this->tariff1_sensor_) {
          this->tariff1_sensor_->publish_state(T1);
        }
        if (this->tariff2_sensor_) {
          this->tariff2_sensor_->publish_state(T2);
        }
        if (this->tariff3_sensor_) {
          this->tariff3_sensor_->publish_state(T3);
        }
        if (this->total_sensor_) {
          this->total_sensor_->publish_state(sum);
        }
#endif
      }
    }
  }
}
