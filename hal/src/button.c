#include "button.h"
#include <stdio.h>

// Only compile gpiod code for ARM64 target
#ifdef __aarch64__

#include <gpiod.h>
#include <stdlib.h>
#include <unistd.h>

#define CHIP_NAME "/dev/gpiochip2"
#define LINE_BUTTON 13 // GPIO15, same as encoder button

static struct gpiod_chip *chip;
static struct gpiod_line_request *request;
static bool last_state = false;

void button_initialize(void)
{
    // Open the GPIO chip
    chip = gpiod_chip_open(CHIP_NAME);
    if (!chip)
    {
        perror("button: gpiod_chip_open");
        exit(1);
    }

    // Prepare line settings
    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    if (!settings)
    {
        perror("button: gpiod_line_settings_new");
        gpiod_chip_close(chip);
        exit(1);
    }

    // Configure as input with pull-up resistor
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);

    // Create configuration for the button line
    struct gpiod_line_config *config = gpiod_line_config_new();
    if (!config)
    {
        perror("button: gpiod_line_config_new");
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        exit(1);
    }

    unsigned int offset = LINE_BUTTON;
    gpiod_line_config_add_line_settings(config, &offset, 1, settings);

    // Describe who is requesting this line
    struct gpiod_request_config *reqConfig = gpiod_request_config_new();
    if (!reqConfig)
    {
        perror("button: gpiod_request_config_new");
        gpiod_line_config_free(config);
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        exit(1);
    }

    gpiod_request_config_set_consumer(reqConfig, "button");

    // Request the line from the kernel
    request = gpiod_chip_request_lines(chip, reqConfig, config);

    // Free temporary config objects
    gpiod_request_config_free(reqConfig);
    gpiod_line_config_free(config);
    gpiod_line_settings_free(settings);

    if (!request)
    {
        perror("button: gpiod_chip_request_lines");
        gpiod_chip_close(chip);
        exit(1);
    }

    // Initialize last state
    last_state = button_isPressed();

    printf("[Button] Initialized on GPIO15 (line 13)\n");
}

bool button_isPressed(void)
{
    // Button is active low (pressed = 0)
    int value = gpiod_line_request_get_value(request, LINE_BUTTON);
    return (value == 0);
}

bool button_wasJustPressed(void)
{
    bool current_state = button_isPressed();

    // Detect rising edge (transition from not pressed to pressed)
    bool just_pressed = (current_state && !last_state);

    last_state = current_state;
    return just_pressed;
}

void button_cleanup(void)
{
    // Release GPIO resources
    if (request)
    {
        gpiod_line_request_release(request);
        request = NULL;
    }

    if (chip)
    {
        gpiod_chip_close(chip);
        chip = NULL;
    }

    printf("[Button] Cleaned up\n");
}

#else
// Host machine - stub implementations (no gpiod)

void button_initialize(void)
{
    printf("[Button] Stub initialization (host mode - no hardware)\n");
}

bool button_isPressed(void)
{
    return false;
}

bool button_wasJustPressed(void)
{
    return false;
}

void button_cleanup(void)
{
    printf("[Button] Stub cleanup (host mode)\n");
}

#endif // __aarch64__