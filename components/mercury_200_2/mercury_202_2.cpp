#include "mercury_202_2.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace mercury_202_2 {

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

  void MercuryComponent::dump_config() {
    ESP_LOGCONFIG(TAG, "Mercury 200.2:");
    LOG_UPDATE_INTERVAL(this);
    ESP_LOGCONFIG(TAG, "  Address: %s", this->addreess_);
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

  void MercuryComponent::setup() {
    this->calculateParams(this->electrical_parameters_, 0x63);
    this->calculateParams(this->tarif_, 0x27);
  }

  void MercuryComponent::main_uart_read(byte *command) {
    unsigned long start = millis();
    unsigned long d = start;

    this->write_array(command, 7);
    this->flush();

    while (d >= start && d < start + 3000) {
      d = millis();

      while(this->available()) {
        this->Re_buf_[this->counter_] = this->read();
        this->counter_++;
      }
    }
  }

  void MercuryComponent::update() {
    this->counter_ = 0;

    switch (this->step_) {
      case 0:
        this->main_uart_read(this->tarif_);
        break;

      case 1: } else if (this->step_== 1) {
        this->main_uart_read(this->electrical_parameters_);
        break;
    }

    if (this->Re_buf_[0] == 0x00 && this->Re_buf_[4] == 0x63) {

      double V = this->readDouble(&this->Re_buf_[5], 10); //  Парсинг байтов и перевод в нормальные значения
      double A = this->readDouble(&this->Re_buf_[7], 100); // Парсинг байтов  и перевод в нормальные значения
      double W = this->readDouble<3>(&this->Re_buf_[9], 1000); // Парсинг байтов  и перевод в нормальные значения

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

    if (this->Re_buf_[0] == 0x00 && this->Re_buf_[4] == 0x27) {
      double T1 = this->readDouble<4>(&this->Re_buf_[5], 100);
      double T2 = this->readDouble<4>(&this->Re_buf_[9], 100);
      double T3 = this->readDouble<4>(&this->Re_buf_[13], 100);
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

    this->set_step();
  }

  void MercuryComponent::set_step() {
    this->step_++;
    if (this->step_ >= 2) {
      this->step_ = 0;
    }
  }

}
}
