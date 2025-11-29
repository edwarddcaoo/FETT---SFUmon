#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdbool.h>

bool joystick_init(void);

void joystick_cleanup(void);

int joystick_read_x(void);

int joystick_read_y(void);

bool joystick_read_button(void);

#endif