#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

// Initialize both buttons
void button_initialize(void);

// Catch/Interact button (line 13)
bool button_catch_isPressed(void);
bool button_catch_wasJustPressed(void);

// Reset button (line 14)
bool button_reset_isPressed(void);
bool button_reset_wasJustPressed(void);

bool button_wasJustPressed(void);

// Cleanup
void button_cleanup(void);

#endif