#pragma once

void battery_monitor_init();
float read_battery_voltage();
void check_charging_state(float battVoltage);
