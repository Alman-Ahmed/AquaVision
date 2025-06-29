#include <Arduino.h>
#include "battery_monitor.h"

#define BATT_PIN 32

void battery_monitor_init() {
  pinMode(BATT_PIN, INPUT);
}

float read_battery_voltage() {
  int raw = analogRead(BATT_PIN);
  float voltage = (raw / 4095.0) * 3.3 * 2.0;
  return voltage;
}

void check_charging_state(float battVoltage) {
  static float lastVoltage = 0;
  if (battVoltage > lastVoltage + 0.01) {
    Serial.println("Charging: YES");
  } else {
    Serial.println("Charging: NO");
  }
  lastVoltage = battVoltage;
}
