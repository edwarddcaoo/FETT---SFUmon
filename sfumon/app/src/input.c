#include "input.h"
#include <SDL2/SDL.h>

bool input_is_catch_pressed(bool* was_pressed) {
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
    bool space_is_pressed = keyboard_state[SDL_SCANCODE_SPACE];
    
    // Edge-triggered: only true on new press
    bool result = space_is_pressed && !(*was_pressed);
    *was_pressed = space_is_pressed;
    
    return result;
}