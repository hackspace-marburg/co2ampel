#include <Arduino.h>
#include "sensor.hpp"

Sensor* sensor;

void setup() {
  Serial.begin(115200);

  sensor = new(DummySensor);
  Serial.println(sensor->name);
}

void loop() {
  Serial.println(sensor->read());
  delay(500);
}
