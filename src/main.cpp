#include <vector>
#include "sensor.hpp"
#include "sink.hpp"

Sensor* sensor;
std::vector<Sink*> sinks;

void setup() {
  sensor = new DummySensor;

  sinks.push_back(new SerialSink(115200));
  sinks.push_back(new LEDSink(2, 1000));
}

void loop() {
  const String sensorName = sensor->name;
  const PPM ppm = sensor->read();

  for (Sink* sink : sinks) {
    sink->write(sensorName, ppm);
  }

  delay(500);
}
