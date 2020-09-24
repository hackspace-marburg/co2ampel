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
  sensor = new CCS811Sensor;

#ifdef SINK_SERIAL
  sinks.push_back(new SerialSink(SERIAL_BAUD));
#endif // SINK_SERIAL

#ifdef SINK_LED
  sinks.push_back(new LEDSink(LED_PIN, THRESHOLD_PPM_CRITICAL));
#endif // SINK_LED

#ifdef SINK_RGB_LED
  sinks.push_back(new RGBLEDSink(RGB_PIN_R, RGB_PIN_G, RGB_PIN_B,
        THRESHOLD_PPM_WARN, THRESHOLD_PPM_CRITICAL));
#endif //SINK_RGB_LED

#ifdef SINK_INFLUX
  sinks.push_back(new InfluxSink(
        INFLUX_DEV_NAME, INFLUX_WIFI_SSID, INFLUX_WIFI_PSK, INFLUX_WIFI_URL));
#endif // SINK_INFLUX
}

void loop() {
  const String sensorName = sensor->name;
  const PPM ppm = sensor->read();

  if (ppm != 0) {  // TODO: use propper error handling
    for (Sink* sink : sinks) {
      sink->write(sensorName, ppm);
    }
  }

  delay(CYCLE_DELAY_MS);
}
