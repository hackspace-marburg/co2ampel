// SPDX-FileCopyrightText: 2020 Hackspace Marburg
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <vector>
#include "configuration.hpp"
#include "sensor.hpp"
#include "sink.hpp"

Sensor* sensor;
std::vector<Sink*> sinks;

void setup() {
  sensor = new BME680Sensor;

#ifdef SINK_SERIAL
  sinks.push_back(new SerialSink(SERIAL_BAUD));
#endif // SINK_SERIAL
#ifdef SINK_LED
  sinks.push_back(new LEDSink(LED_PIN, LED_THRESHOLD));
#endif // SINK_LED
#ifdef SINK_INFLUX
  sinks.push_back(new InfluxSink(
        INFLUX_DEV_NAME, INFLUX_WIFI_SSID, INFLUX_WIFI_PSK, INFLUX_WIFI_URL));
#endif // SINK_INFLUX
}

void loop() {
  const String sensorName = sensor->name;
  const PPM ppm = sensor->read();

  for (Sink* sink : sinks) {
    sink->write(sensorName, ppm);
  }

  delay(3000);
}
