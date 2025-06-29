#pragma once
#include <OneWire.h>
#include <DallasTemperature.h>

void sensors_init();
float read_ph();
float read_turbidity();
float read_temperature();
