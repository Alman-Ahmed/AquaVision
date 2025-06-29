#include <Arduino.h>
#include "serial_comm.h"

void send_sensor_data(float ph, float turbidity, float temp, float batt) {
  Serial2.printf("%.2f,%.2f,%.2f,%.2f\n", ph, turbidity, temp, batt);
}

void print_sensor_data(float ph, float turbidity, float temp, float batt) {
  Serial.printf("pH: %.2f | Turbidity: %.2f NTU | Temp: %.2f C | Battery: %.2f V\n", ph, turbidity, temp, batt);
}
