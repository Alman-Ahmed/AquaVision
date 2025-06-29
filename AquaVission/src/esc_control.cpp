#include <Arduino.h>
#include <Servo.h>
#include "esc_control.h"

#define ESC1_PIN 18
#define ESC2_PIN 19

Servo esc1, esc2;

void esc_init() {
  esc1.attach(ESC1_PIN);
  esc2.attach(ESC2_PIN);
}

void control_escs(int esc1_us, int esc2_us) {
  esc1.writeMicroseconds(esc1_us);
  esc2.writeMicroseconds(esc2_us);
}
