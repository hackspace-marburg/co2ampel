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
    SerialSink() {
      Serial.begin(115200);
    }

    ~SerialSink() {
      Serial.end();
    }

    void write(const String sensorName, const PPM ppm) override {
      Serial.println(sensorName + "," + ppm);
    }
};
