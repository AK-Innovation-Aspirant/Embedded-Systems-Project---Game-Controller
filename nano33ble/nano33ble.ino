#include "Nano33BleHID.h"
#include "signal_utils.h"

// Use the existing Nano33BleGamepad type alias
Nano33BleGamepad bleGamepad("nano33BLE Gamepad");

// Built-in LED animation delays when disconnected
static const int kLedBeaconDelayMilliseconds = 1250;
static const int kLedErrorDelayMilliseconds  = kLedBeaconDelayMilliseconds / 10;

// Built-in LED intensity when connected
static const int kLedConnectedIntensity = 30;

// Map from 0-1023 to -1.0 to 1.0, with 512 as the center point
static float normalizeAxis(int value)
{
  return (static_cast<float>(value) - 512.0f) / 511.0f;
}

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

void setup()
{
  // General setup
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

  // Initialize BLE and the HID service
  bleGamepad.initialize();

  // Launch the event queue that will manage BLE events and the loop
  // After this call, the main thread will be halted
  MbedBleHID_RunEventThread();
}

void loop()
{
  // When disconnected, animate the built-in LED to indicate the device state
  if (!bleGamepad.connected()) {
    animateLED(pins::LED, bleGamepad.has_error() ? kLedErrorDelayMilliseconds
                                                   : kLedBeaconDelayMilliseconds);
    return;
  }

  // When connected, slightly dim the built-in LED
  analogWrite(pins::LED, kLedConnectedIntensity);

  // Retrieve the HID Gamepad service
  auto *gamepad = bleGamepad.hid();

  // Read buttons
  bool valUp = !digitalRead(pins::UP);
  bool valDown = !digitalRead(pins::DOWN);
  bool valLeft = !digitalRead(pins::LEFT);
  bool valRight = !digitalRead(pins::RIGHT);

  bool valA = !digitalRead(pins::A);
  bool valB = !digitalRead(pins::B);
  bool valX = !digitalRead(pins::X);
  bool valY = !digitalRead(pins::Y);

  // Read joysticks

  int valJoyLeftX = analogRead(pins::JOY_LEFT_X);
  int valJoyLeftY = analogRead(pins::JOY_LEFT_Y);

  int valJoyRightX = analogRead(pins::JOY_RIGHT_X);
  int valJoyRightY = analogRead(pins::JOY_RIGHT_Y);

  // Which button was pressed
  HIDGamepadService::Button buttons = HIDGamepadService::BUTTON_NONE;
  if (valUp) { buttons = static_cast<HIDGamepadService::Button>(buttons | HIDGamepadService::BUTTON_UP); }
  if (valDown) { buttons = static_cast<HIDGamepadService::Button>(buttons | HIDGamepadService::BUTTON_DOWN); }
  if (valLeft) { buttons = static_cast<HIDGamepadService::Button>(buttons | HIDGamepadService::BUTTON_LEFT); }
  if (valRight) { buttons = static_cast<HIDGamepadService::Button>(buttons | HIDGamepadService::BUTTON_RIGHT); }
  if (valA) { buttons = static_cast<HIDGamepadService::Button>(buttons | HIDGamepadService::BUTTON_A); }
  if (valB) { buttons = static_cast<HIDGamepadService::Button>(buttons | HIDGamepadService::BUTTON_B); }
  if (valX) { buttons = static_cast<HIDGamepadService::Button>(buttons | HIDGamepadService::BUTTON_X); }
  if (valY) { buttons = static_cast<HIDGamepadService::Button>(buttons | HIDGamepadService::BUTTON_Y); }

  // Update joysticks
  gamepad->motion(
    normalizeAxis(valJoyLeftX),
    normalizeAxis(valJoyLeftY),
    normalizeAxis(valJoyRightX),
    normalizeAxis(valJoyRightY)
  );
  gamepad->button(buttons);
  gamepad->SendReport();
}
