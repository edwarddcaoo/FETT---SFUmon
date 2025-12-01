#include "input.h"
#include <stdio.h>

static bool use_joystick = false;
static bool use_button = false;
static bool cached_button_just_pressed = false;

// Detect if we're on target device by checking for SPI
static bool is_target_device(void)
{
    FILE *f = fopen("/dev/spidev0.0", "r");
    if (f)
    {
        fclose(f);
        return true; // SPI device exists = target
    }
    return false;
}

bool input_initialize(void)
{
    // Try to detect target and initialize joystick
    if (is_target_device())
    {
        printf("Target device detected - attempting hardware initialization...\n");

        if (joystick_initialize())
        {
            use_joystick = true;
            printf("✓ Using joystick input\n");
        }
        else
        {
            printf("✗ Joystick initialization failed, falling back to keyboard\n");
            use_joystick = false;
        }

        // Try to initialize button
        button_initialize();
        use_button = true;
        printf("✓ Using hardware button for catch\n");
    }
    else
    {
        printf("Host machine detected - using keyboard input\n");
        use_joystick = false;
        use_button = false;
    }

    return true;
}

void input_poll_once_per_frame(void)
{
    if (use_button)
        cached_button_just_pressed = button_wasJustPressed();
    else
        cached_button_just_pressed = false;
}

bool input_button_just_pressed_cached(void)
{
    return cached_button_just_pressed;
}

InputDirection input_get_direction(void)
{
    if (use_joystick)
    {
        // Read joystick
        joystickDirection joy_dir = joystick_getDirection();

        switch (joy_dir)
        {
        case joy_up:
            return INPUT_UP;
        case joy_down:
            return INPUT_DOWN;
        case joy_left:
            return INPUT_LEFT;
        case joy_right:
            return INPUT_RIGHT;
        case joy_rest:
        default:
            return INPUT_NONE;
        }
    }
    else
    {
        // Read keyboard
        const Uint8 *keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
        {
            return INPUT_UP;
        }
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
        {
            return INPUT_DOWN;
        }
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
        {
            return INPUT_LEFT;
        }
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
        {
            return INPUT_RIGHT;
        }

        return INPUT_NONE;
    }
}

bool input_is_catch_pressed(bool *last_state)
{
    if (use_button)
    {
        // Use hardware button
        return input_button_just_pressed_cached();
    }
    else
    {
        // Use keyboard spacebar with edge detection
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        bool current_state = keys[SDL_SCANCODE_SPACE];

        bool just_pressed = (current_state && !(*last_state));
        *last_state = current_state;

        return just_pressed;
    }
}

bool input_is_interact_pressed(bool *last_state)
{
    if (use_button)
    {
        // Use same hardware button
        return input_button_just_pressed_cached();
    }
    else
    {
        // Use keyboard 'T' key with edge detection
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        bool current_state = keys[SDL_SCANCODE_T];
        bool just_pressed = (current_state && !(*last_state));
        *last_state = current_state;
        return just_pressed;
    }
}

void input_cleanup(void)
{
    if (use_joystick)
    {
        joystick_cleanup();
        printf("Joystick cleaned up\n");
    }

    if (use_button)
    {
        button_cleanup();
    }
}