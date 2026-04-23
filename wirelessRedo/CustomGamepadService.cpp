// ATTRIBUTION:
// This file is a slight modification of services/HIDGamepadService.cpp
// in the Mbed_BLE_HID library by "tcoppex" on GitHub:
// Thibault Coppex <tcoppex@proton.me>
//
// See the header for discussion regarding the rationale for including
// our patched version of Coppex's code.
//
// This file is the work of Thibault Coppex, except that it includes a
// few changes made by us to add Z and Rx analog axes to the Usage Page
// in the HID Report Map, as well as a slight modification to report on
// 8 buttons instead of 4 (this change also required us to remove some
// padding from the report; it was previously needed to maintain byte
// alignment since the buttons are stored in a bitfield, but with
// exactly 8 buttons, no padding ought to be used).

#include <mbed.h>
#include "CustomGamepadService.h"

namespace {

// Report Reference
static report_reference_t input_report_ref = { 0, INPUT_REPORT };

static GattAttribute input_report_ref_desc(
  ATT_UUID_HID_REPORT_ID_MAPPING,
  (uint8_t*)&input_report_ref,
  sizeof(input_report_ref),
  sizeof(input_report_ref)
);

static GattAttribute *input_report_ref_descs[] = {
  &input_report_ref_desc,
};

// Input Report
#pragma pack(push, 1)
struct {
  uint8_t x;
  uint8_t y;
  uint8_t z;
  uint8_t rx;
  uint8_t buttons;
} hid_input_report;
#pragma pack(pop)

// Report Map
static uint8_t hid_report_map[] =
{
  USAGE_PAGE(1),      0x01,       // Usage Page (Generic Desktop)
  USAGE(1),           0x05,       // Usage (Game Pad)
  COLLECTION(1),      0x01,       // Collection (Application)
    USAGE(1),           0x01,       // Usage (Pointer)
    COLLECTION(1),      0x00,       // Collection (Physical)
      USAGE_PAGE(1),      0x01,       // Usage Page (Generic Desktop)
      USAGE(1),           0x30,       // Usage (X)
      USAGE(1),           0x31,       // Usage (Y)
      USAGE(1),           0x32,       // Usage (Z)
      USAGE(1),           0x33,       // Usage (Rx)
      REPORT_SIZE(1),     0x08,       // Report Size (8)
      REPORT_COUNT(1),    0x04,       // Report Count (4)
      INPUT(1),           0x02,       // Input (Data, Variable, Absolute)

      // Buttons
      USAGE_PAGE(1),      0x09,       // Usage Page (Buttons)
      USAGE_MINIMUM(1),   0x01,       // Usage Minimum (1)
      USAGE_MAXIMUM(1),   0x08,       // Usage Maximum (8)
      LOGICAL_MINIMUM(1), 0x00,       // Logical Minimum (0)
      LOGICAL_MAXIMUM(1), 0x01,       // Logical Maximum (1)
      REPORT_COUNT(1),    0x08,       // Report Count (8)
      REPORT_SIZE(1),     0x01,       // Report Size (1)
      INPUT(1),           0x02,       // Input (Data, Variable, Absolute)

    END_COLLECTION(0),              // End Collection (Physical)
  END_COLLECTION(0),              // End Collection (Application)
};

} // namespace "" 


CustomGamepadService::CustomGamepadService(BLE &_ble) 
  : HIDService(_ble,
               HID_OTHER,
               
               // report map
               hid_report_map, 
               sizeof(hid_report_map) / sizeof(*hid_report_map),

               // input report
               (uint8_t*)&hid_input_report,
               sizeof(hid_input_report),
               input_report_ref_descs,
               sizeof(input_report_ref_descs) / sizeof(*input_report_ref_descs))
{

}

void CustomGamepadService::motion(float fx, float fy, float fz, float frx) {
  uint8_t x  = static_cast<int>(0x100 + fx  * 0x7f) & 0xff;
  uint8_t y  = static_cast<int>(0x100 + fy  * 0x7f) & 0xff;
  uint8_t z  = static_cast<int>(0x100 + fz  * 0x7f) & 0xff;
  uint8_t rx = static_cast<int>(0x100 + frx * 0x7f) & 0xff;

  hid_input_report.x = x;
  hid_input_report.y = y;
  hid_input_report.z = z;
  hid_input_report.rx = rx;
}

void CustomGamepadService::button(Button buttons) {
  hid_input_report.buttons = uint8_t(buttons); 
}