// SPDX-FileCopyrightText: 2020 Hackspace Marburg
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <Adafruit_BME680.h>
#include <Arduino.h>
#include <Wire.h>
#include <ccs811.h>

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

class BME680Sensor : public Sensor {
  private:
    Adafruit_BME680 bme;

  public:
    BME680Sensor() : Sensor(F("bme680-voc")) {
      bme.begin();
      bme.setGasHeater(320, 150);
    }

    PPM read() override {
      if (!bme.performReading()) {
        return 0;
      }

      // This is _NOT_ a CO2 value, but a VOC value. The BME680 sensor is not
      // capable of measuring CO2. However, the proprietary BSEC library can
      // calculate it based on some correlation. Other studies are also
      // mentioning correlations, but I was not able to find some formula. Sigh.
      //
      // https://github.com/adafruit/Adafruit_BME680/issues/37
      // http://www.dbu.de/media/12030902585415b2.pdf
      return bme.gas_resistance / 1000;
    }
};

class CCS811Sensor : public Sensor {
  private:
    CCS811 ccs811;

  public:
    CCS811Sensor() : Sensor(F("ccs811")) {
      Wire.begin();

      for (int i = 0; !ccs811.begin() && i < 100; i++) {
        delay(100);
      }
      for (int i = 0; !ccs811.start(CCS811_MODE_1SEC) && i < 100; i++) {
        delay(100);
      }
    }

    PPM read() override {
      uint16_t eco2, errstat;
      ccs811.read(&eco2, nullptr, &errstat, nullptr);

      if (errstat != CCS811_ERRSTAT_OK) {
        return 0;
      }

      return eco2;
    }
};
