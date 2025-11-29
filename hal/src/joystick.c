#include "joystick.h"
#include <SDL2/SDL.h>

#ifdef TARGET_BEAGLE

// Real hardware implementation (TODO: implement SPI)
bool joystick_init(void) { return true; }
void joystick_cleanup(void) {}
int joystick_read_x(void) { return 2048; }
int joystick_read_y(void) { return 2048; }
bool joystick_read_button(void) { return false; }
#else
// Simulation using keyboard
bool joystick_init(void) { return true; }
void joystick_cleanup(void) {}
int joystick_read_x(void) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_LEFT]) return 0;
    if (keys[SDL_SCANCODE_RIGHT]) return 4095;
    return 2048;
}
int joystick_read_y(void) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_UP]) return 0;
    if (keys[SDL_SCANCODE_DOWN]) return 4095;
    return 2048;
}
bool joystick_read_button(void) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    return keys[SDL_SCANCODE_SPACE];
}
#endif