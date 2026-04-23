// This is the main sketch of our project.
//
// It depends on the Mbed_BLE_HID library by
// "tcoppex" on GitHub:
// Thibault Coppex <tcoppex@proton.me>
//
// The library usage in this implementation was
// adapted from the examples/ written by Thibault
// Coppex.
//
// The sensor reading facilities in this sketch
// were written by us to suit our project's needs.

#include <Mbed_BLE_HID.h>
#include "CustomGamepadService.h"

typedef CustomGamepadService::Button Btn;

namespace pins {
  // Onboard LED
  static const int LED = D13;

  // Directional Pad
  static const int UP = A4;
  static const int DOWN = A5;
  static const int LEFT = A6;
  static const int RIGHT = A7;

  // Main Buttons
  static const int A = D4;
  static const int B = D5;
  static const int X = D6;
  static const int Y = D7;

  // Left Joystick
  static const int JOY_LEFT_X = A0;
  static const int JOY_LEFT_Y = A1;

  // Right Joystick
  static const int JOY_RIGHT_X = A2;
  static const int JOY_RIGHT_Y = A3;
}

BasicMbedBleHID<CustomGamepadService> gamepad(
  "Nano 33", "Boilermakers", "0.1"
);

void panic() {
  while (1) {
    digitalWrite(pins::LED, HIGH);
    delay(100);
    digitalWrite(pins::LED, LOW);
    delay(100);
  }
}

float discreteToContinuous(int analogVal) {
  return 2.0f * (analogVal / 1023.0f) - 1.0f;
}

void setup() {
  pinMode(pins::LED, OUTPUT);

  pinMode(pins::UP, INPUT_PULLUP);
  pinMode(pins::DOWN, INPUT_PULLUP);
  pinMode(pins::LEFT, INPUT_PULLUP);
  pinMode(pins::RIGHT, INPUT_PULLUP);

  pinMode(pins::A, INPUT_PULLUP);
  pinMode(pins::B, INPUT_PULLUP);
  pinMode(pins::X, INPUT_PULLUP);
  pinMode(pins::Y, INPUT_PULLUP);

  pinMode(pins::JOY_LEFT_X, INPUT);
  pinMode(pins::JOY_LEFT_Y, INPUT);

  pinMode(pins::JOY_RIGHT_X, INPUT);
  pinMode(pins::JOY_RIGHT_Y, INPUT);

  gamepad.initialize();

  MbedBleHID_RunEventThread();
}

void loop() {
  if (!gamepad.connected()) {
    if (gamepad.has_error()) {
      panic();
    } else {
      digitalWrite(pins::LED, HIGH);
      return;
    }
  }

  digitalWrite(pins::LED, LOW);

  bool valUp    = !digitalRead(pins::UP);
  bool valDown  = !digitalRead(pins::DOWN);
  bool valLeft  = !digitalRead(pins::LEFT);
  bool valRight = !digitalRead(pins::RIGHT);

  bool valA = !digitalRead(pins::A);
  bool valB = !digitalRead(pins::B);
  bool valX = !digitalRead(pins::X);
  bool valY = !digitalRead(pins::Y);

  int valJoyLeftX = 1023 - analogRead(pins::JOY_LEFT_X);
  int valJoyLeftY = 1023 - analogRead(pins::JOY_LEFT_Y);

  int valJoyRightX = analogRead(pins::JOY_RIGHT_X);
  int valJoyRightY = analogRead(pins::JOY_RIGHT_Y);

  CustomGamepadService *gs = gamepad.hid();

  int buttonStates =
    valUp    * Btn::BUTTON_UP    |
    valDown  * Btn::BUTTON_DOWN  |
    valLeft  * Btn::BUTTON_LEFT  |
    valRight * Btn::BUTTON_RIGHT |
    valA     * Btn::BUTTON_A     |
    valB     * Btn::BUTTON_B     |
    valX     * Btn::BUTTON_X     |
    valY     * Btn::BUTTON_Y;

  gs->button(static_cast<Btn>(buttonStates));

  float fx = discreteToContinuous(valJoyLeftX);
  float fy = discreteToContinuous(valJoyLeftY);

  float fz  = discreteToContinuous(valJoyRightX);
  float frx = discreteToContinuous(valJoyRightY);

  gs->motion(fx, fy, fz, frx);

  gs->SendReport();
}
