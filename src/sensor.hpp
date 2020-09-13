#pragma once

#include <Arduino.h>

using PPM = unsigned int;

class Sensor {
  protected:
    explicit Sensor(const String name) : name(name) {}
    virtual ~Sensor() {}

  public:
    const String name;

    virtual PPM read() = 0;
};

class DummySensor : public Sensor {
  private:
    PPM ppm = 550;

  public:
    DummySensor() : Sensor(F("dummy")) {};

    PPM read() override {
      return ppm >= 1800 ? ppm = 600 : ppm += 50;
    }
};
