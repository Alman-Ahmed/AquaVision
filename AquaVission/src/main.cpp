#include <Arduino.h>
#include "sensors.h"
#include "esc_control.h"
#include "battery_monitor.h"
#include "serial_comm.h"
#include "web_server.h"

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
  sensors_init();
  esc_init();
  battery_monitor_init();
  start_web_server(); // Start WiFi and web server
}

void loop() {
  float ph = read_ph();
  float turbidity = read_turbidity();
  float temp = read_temperature();
  float batt = read_battery_voltage();

  print_sensor_data(ph, turbidity, temp, batt);
  send_sensor_data(ph, turbidity, temp, batt);

  receive_and_process_command(); // NEW: handle incoming commands

  // Use latest throttle values from commands
  control_escs(get_esc1_throttle(), get_esc2_throttle());

  check_charging_state(batt);

  delay(2000);
}
