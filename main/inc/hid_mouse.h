#ifndef HID_MOUSE_H
#define HID_MOUSE_H

#include <stdbool.h>
#include <stdio.h>

bool init_hid_mouse();
bool mouse_left_click();
bool mouse_right_click();
bool mouse_move_up(const size_t move_value);
bool mouse_move_down(const size_t move_value);
bool mouse_move_left(const size_t move_value);
bool mouse_move_right(const size_t move_value);

#endif // HID_MOUSE_H