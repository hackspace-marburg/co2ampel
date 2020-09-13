#pragma once

#include <Arduino.h>
#include "sensor.hpp"

class Sink {
  public:
    virtual ~Sink() {}

    virtual void write(const String sensorName, const PPM ppm) = 0;
};

class SerialSink : public Sink {
  public:
    SerialSink(const int baud) {
      Serial.begin(baud);
    }

    ~SerialSink() {
      Serial.end();
    }

    void write(const String sensorName, const PPM ppm) override {
      Serial.println(sensorName + "," + ppm);
    }
};

class LEDSink : public Sink {
  private:
    int pin;
    PPM threshold;

  public:
    LEDSink(const int pin, const PPM threshold) : pin(pin), threshold(threshold) {
      pinMode(pin, OUTPUT);
    }

    void write(const String, const PPM ppm) override {
      digitalWrite(pin, ppm >= threshold);
    }
};
