// SPDX-FileCopyrightText: 2020 Hackspace Marburg
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "sensor.hpp"

class Sink {
  public:
    virtual ~Sink() {}

    virtual void write(const String sensorName, const PPM ppm) = 0;
};

class SerialSink : public Sink {
  public:
    explicit SerialSink(const int baud) {
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
    const int pin;
    const PPM threshold;

  public:
    LEDSink(const int pin, const PPM threshold) : pin(pin), threshold(threshold) {
      pinMode(pin, OUTPUT);
    }

    void write(const String, const PPM ppm) override {
      digitalWrite(pin, ppm >= threshold);
    }
};

class RGBLEDSink : public Sink {
  private:
    const int pin_r, pin_g, pin_b;
    const PPM threshold_warn, threshold_critical;

    inline void rgb(const bool r, const bool g, const bool b) {
      digitalWrite(pin_r, r);
      digitalWrite(pin_g, g);
      digitalWrite(pin_b, b);
    }

  public:
    RGBLEDSink(const int pin_r, const int pin_g, const int pin_b,
        const PPM threshold_warn, const PPM threshold_critical)
      : pin_r(pin_r), pin_g(pin_g), pin_b(pin_b),
      threshold_warn(threshold_warn), threshold_critical(threshold_critical)
    {
      pinMode(pin_r, OUTPUT);
      pinMode(pin_g, OUTPUT);
      pinMode(pin_b, OUTPUT);
    }

    void write(const String, const PPM ppm) override {
      if (ppm < threshold_warn) {
        rgb(false, true, false);
      } else if (ppm < threshold_critical) {
        rgb(true, true, false);
      } else {
        rgb(true, false, false);
      }
    }
};

class InfluxSink : public Sink {
  private:
    const String deviceName;
    const String influxUrl;

  public:
    InfluxSink(const String deviceName, const char* wifiSsid, const char* wifiPsk, const String influxUrl)
      : deviceName(deviceName), influxUrl(influxUrl)
    {
      WiFi.begin(wifiSsid, wifiPsk);

      for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++) {
        delay(100);
      }
    }

    void write(const String sensorName, const PPM ppm) override {
      if (WiFi.status() != WL_CONNECTED) {
        return;
      }

      String payload = "co2,sensor=";
      payload += sensorName;
      payload += ",device=";
      payload += deviceName;
      payload += " ppm=";
      payload += ppm;

      HTTPClient httpClient;
      httpClient.begin(influxUrl.c_str());
      httpClient.POST(payload);
      httpClient.end();
    }
};
