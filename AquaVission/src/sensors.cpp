#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "sensors.h"

#define PH_PIN         34
#define TURBIDITY_PIN  35
#define TEMP_PIN       4

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

void sensors_init() {
  sensors.begin();
}

float read_ph() {
  int raw = analogRead(PH_PIN);
  return (raw / 4095.0) * 14.0;
}

float read_turbidity() {
  int raw = analogRead(TURBIDITY_PIN);
  return (raw / 4095.0) * 1000.0;
}

float read_temperature() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}
