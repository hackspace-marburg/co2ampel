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
