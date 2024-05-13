#ifndef HID_KEYBOARD_H
#define HID_KEYBOARD_H

#include <stdbool.h>
#include <stdio.h>

bool init_hid_keyboard(bool *is_usb_enabled);
bool send_something();

#endif // HID_KEYBOARD_H