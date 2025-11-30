#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <stdbool.h>

typedef enum
{
    joy_rest,  // 0, rest position when joystick is untouched
    joy_up,    // 1
    joy_down,  // 2
    joy_left,  // 3
    joy_right, // 4
} joystickDirection;

// initialize joystick with SPI set up and calibration
bool joystick_initialize(void);

// clean up joystick data at exit
void joystick_cleanup(void);

// get the direction of the joysticks position
joystickDirection joystick_getDirection(void);

#endif