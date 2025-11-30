#ifndef BUTTON_H
#define BUTTON_H
#include <stdbool.h>

// Initialize button GPIO
void button_initialize(void);

// Check if button is currently pressed
bool button_isPressed(void);

// Check if button was just pressed (edge detection)
bool button_wasJustPressed(void);

// Cleanup GPIO resources
void button_cleanup(void);

#endif