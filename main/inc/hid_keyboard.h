#ifndef HID_KEYBOARD_H
#define HID_KEYBOARD_H

#include <stdbool.h>
#include <stdio.h>

bool init_hid_keyboard();
bool hid_heyboard_send(const char* input_string, size_t data_len);

#endif // HID_KEYBOARD_H