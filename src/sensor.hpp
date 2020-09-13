#pragma once

#include <Arduino.h>

class Sensor {
  protected:
    explicit Sensor(const String name) : name(name) {}
    virtual ~Sensor() {}

  public:
    const String name;

    virtual unsigned int read() = 0;
};

class DummySensor : public Sensor {
  private:
    unsigned int ppm = 600;

  public:
    DummySensor() : Sensor(F("dummy")) {};

    virtual unsigned int read() override {
      return ppm >= 1800 ? ppm = 600 : ppm += 50;
    }
};
