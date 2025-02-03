#include "mercury_202_2.h"

namespace esphome {
namespace mercury_202_2 {

  long MercuryComponent::pow(long a, int s) {
    long out = 1;
    for (int i = 0; i < s; i++)
      out *= a;
    return out;
  }

  template<size_t N = 2> long MercuryComponent::readLong(uchar *inp) {
    long out = 0;

    for (int i = 0; i < N; i++) {
      uchar v = inp[i];
      int p = pow(10, ((N - 1) - i) * 2);
      out += (((v >> 4) & 15) * 10 + (v & 15)) * p;
    }

    return out;
  }

  template<size_t N = 2> double MercuryComponent::readDouble(uchar *inp, int del) {
    return (double) this->readLong<N>(inp) / del;
  }

  uint16_t MercuryComponent::crc16(const uint8_t *data, uint8_t len) {
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

  void MercuryComponent::setup() override {
    this->calculateParams(this->electrical_parameters, 0x63);
    this->calculateParams(this->tarif, 0x27);
  }

  void MercuryComponent::main_uart_read(byte *command) {
    unsigned long start = millis();
    unsigned long d = start;

    this->write_array(command, 7);
    this->flush();

  //    digitalWrite(0, LOW);

    while (d >= start && d < start + 3000) {
      d = millis();

      while(available()) {
        this->Re_buf[counter] = this->read();
        this->counter++;
      }
    }
  }

  void MercuryComponent::update() override {
    counter = 0;

    if (step == 0) {
      this->main_uart_read(this->tarif);

    } else if (step == 1) {
      this->main_uart_read(this->electrical_parameters);

    }

    if (this->Re_buf[0] == 0x00 && this->Re_buf[4] == 0x63) {

      double V = this->readDouble(&this->Re_buf[5], 10); //  Парсинг байтов и перевод в нормальные значения
      double A = this->readDouble(&this->Re_buf[7], 100); // Парсинг байтов  и перевод в нормальные значения
      double W = this->readDouble<3>(&this->Re_buf[9], 1000); // Парсинг байтов  и перевод в нормальные значения

      if (this->voltage_ != nullptr)
        this->voltage_->publish_state(V); // Отправка в как сенсор
      if (this->current_ != nullptr)
        this->current_->publish_state(A);
      if (this->power_ != nullptr)
        this->power_->publish_state(W);

    }

    if (this->Re_buf[0] == 0x00 && this->Re_buf[4] == 0x27) {
      double T1 = this->readDouble<4>(&this->Re_buf[5], 100);
      double T2 = this->readDouble<4>(&this->Re_buf[9], 100);
      double T3 = this->readDouble<4>(&this->Re_buf[13], 100);
      double sum = T1 + T2 + T3;

      if (this->tariff1_ != nullptr)
        this->tariff1_->publish_state(T1);
      if (this->tariff2_ != nullptr)
        this->tariff2_->publish_state(T2);
      if (this->tariff3_ != nullptr)
        this->tariff3_->publish_state(T3);
      if (this->total_ != nullptr)
        this->total_->publish_state(sum);

    }

    this->set_step();
  }

  void MercuryComponent::set_step() {
    this->step++;
    if (this->step >= 2) {
      this->step = 0;
    }
  }

}
}
