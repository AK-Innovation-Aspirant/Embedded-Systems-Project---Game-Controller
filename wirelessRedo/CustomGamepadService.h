// ATTRIBUTION:
// This file is a trivial modification of services/HIDGamepadService.h
// in the Mbed_BLE_HID library by "tcoppex" on GitHub:
// Thibault Coppex <tcoppex@proton.me>
//
// We have included it separately from the library because two changes
// were required for our project:
// * We need two analog axes instead of just one
// * We require supporting 8 buttons instead of 4
//
// The class written by Coppex did not offer customization of these
// attributes, and forking the library to change a few lines of code
// seemed excessive, so we have simply pasted our patched version here.

#ifndef H_CUSTOM_GAMEPAD_SERVICE
#define H_CUSTOM_GAMEPAD_SERVICE

#include <ble/BLE.h>
#include <services/HIDService.h>

/* -------------------------------------------------------------------------- */

/**
 * BLE HID Game Pad Service
 *
 * When this class is instantiated, it adds a Game Pad HID service in 
 * the GattServer.
 *
 * The GamePad consists of 8 buttons, a joystick for X and Y motion, and
 * another joystick for X and Y motion which is implemented as a joystick
 * for axes called Z and Rx due to a legacy preference for these axis names
 * in the USB HID specification.
 *
 * Multiple instances of this HID service are not supported.
 */
class CustomGamepadService : public HIDService {
 public:
  // Our change: expand this enum to support 8 buttons
  enum Button {
    BUTTON_NONE    = 0,
    BUTTON_UP      = 1 << 0,
    BUTTON_DOWN    = 1 << 1,
    BUTTON_LEFT    = 1 << 2,
    BUTTON_RIGHT   = 1 << 3,
    BUTTON_A       = 1 << 4,
    BUTTON_B       = 1 << 5,
    BUTTON_X       = 1 << 6,
    BUTTON_Y       = 1 << 7,
  };

  CustomGamepadService(BLE &_ble);

  ble::adv_data_appearance_t appearance() const override {
    return ble::adv_data_appearance_t::GAMEPAD;
  }

  // Our change: add two additional axes to this method's parameters
  void motion(float fx, float fy, float fz, float frx);
  void button(Button buttons);
};

/* -------------------------------------------------------------------------- */

#endif