#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "hal/joystick.h"
#include "hal/button.h"

typedef enum
{
    INPUT_NONE,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT
} InputDirection;

// Initialize input system (auto-detects joystick or keyboard)
bool input_initialize(void);

// Get current input direction
InputDirection input_get_direction(void);

// Check if catch button is pressed (existing function)
bool input_is_catch_pressed(bool *last_state);

// Check if interact button is pressed
bool input_is_interact_pressed(bool *last_state);

// add prototypes
void input_poll_once_per_frame(void);
bool input_button_just_pressed_cached(void);

// Cleanup input system
void input_cleanup(void);

#endif