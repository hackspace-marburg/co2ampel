#include "sensor.hpp"
#include "sink.hpp"

Sensor* sensor;
Sink* sink;

void setup() {
  sensor = new(DummySensor);
  sink = new(SerialSink);
}

void loop() {
  sink->write(sensor->name, sensor->read());

  delay(500);
}
