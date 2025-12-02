#include "button.h"
#include <stdio.h>

// Only compile gpiod code for ARM64 target
#ifdef __aarch64__

#include <gpiod.h>
#include <stdlib.h>
#include <unistd.h>

#define CHIP_NAME "/dev/gpiochip2"
#define LINE_CATCH_BUTTON 13 // GPIO for catch/interact
#define LINE_RESET_BUTTON 14 // GPIO for reset

static struct gpiod_chip *chip;
static struct gpiod_line_request *catch_request;
static struct gpiod_line_request *reset_request;
static bool last_catch_state = false;
static bool last_reset_state = false;

void button_initialize(void)
{
    // Open the GPIO chip
    chip = gpiod_chip_open(CHIP_NAME);
    if (!chip)
    {
        perror("button: gpiod_chip_open");
        exit(1);
    }

    // ===== CATCH BUTTON (LINE 13) =====
    struct gpiod_line_settings *catch_settings = gpiod_line_settings_new();
    if (!catch_settings)
    {
        perror("button: gpiod_line_settings_new (catch)");
        gpiod_chip_close(chip);
        exit(1);
    }
    gpiod_line_settings_set_direction(catch_settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(catch_settings, GPIOD_LINE_BIAS_PULL_UP);

    struct gpiod_line_config *catch_config = gpiod_line_config_new();
    if (!catch_config)
    {
        perror("button: gpiod_line_config_new (catch)");
        gpiod_line_settings_free(catch_settings);
        gpiod_chip_close(chip);
        exit(1);
    }
    unsigned int catch_offset = LINE_CATCH_BUTTON;
    gpiod_line_config_add_line_settings(catch_config, &catch_offset, 1, catch_settings);

    struct gpiod_request_config *catch_reqConfig = gpiod_request_config_new();
    if (!catch_reqConfig)
    {
        perror("button: gpiod_request_config_new (catch)");
        gpiod_line_config_free(catch_config);
        gpiod_line_settings_free(catch_settings);
        gpiod_chip_close(chip);
        exit(1);
    }
    gpiod_request_config_set_consumer(catch_reqConfig, "catch_button");

    catch_request = gpiod_chip_request_lines(chip, catch_reqConfig, catch_config);

    gpiod_request_config_free(catch_reqConfig);
    gpiod_line_config_free(catch_config);
    gpiod_line_settings_free(catch_settings);

    if (!catch_request)
    {
        perror("button: gpiod_chip_request_lines (catch)");
        gpiod_chip_close(chip);
        exit(1);
    }

    // ===== RESET BUTTON (LINE 14) =====
    struct gpiod_line_settings *reset_settings = gpiod_line_settings_new();
    if (!reset_settings)
    {
        perror("button: gpiod_line_settings_new (reset)");
        gpiod_line_request_release(catch_request);
        gpiod_chip_close(chip);
        exit(1);
    }
    gpiod_line_settings_set_direction(reset_settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(reset_settings, GPIOD_LINE_BIAS_PULL_UP);

    struct gpiod_line_config *reset_config = gpiod_line_config_new();
    if (!reset_config)
    {
        perror("button: gpiod_line_config_new (reset)");
        gpiod_line_settings_free(reset_settings);
        gpiod_line_request_release(catch_request);
        gpiod_chip_close(chip);
        exit(1);
    }
    unsigned int reset_offset = LINE_RESET_BUTTON;
    gpiod_line_config_add_line_settings(reset_config, &reset_offset, 1, reset_settings);

    struct gpiod_request_config *reset_reqConfig = gpiod_request_config_new();
    if (!reset_reqConfig)
    {
        perror("button: gpiod_request_config_new (reset)");
        gpiod_line_config_free(reset_config);
        gpiod_line_settings_free(reset_settings);
        gpiod_line_request_release(catch_request);
        gpiod_chip_close(chip);
        exit(1);
    }
    gpiod_request_config_set_consumer(reset_reqConfig, "reset_button");

    reset_request = gpiod_chip_request_lines(chip, reset_reqConfig, reset_config);

    gpiod_request_config_free(reset_reqConfig);
    gpiod_line_config_free(reset_config);
    gpiod_line_settings_free(reset_settings);

    if (!reset_request)
    {
        perror("button: gpiod_chip_request_lines (reset)");
        gpiod_line_request_release(catch_request);
        gpiod_chip_close(chip);
        exit(1);
    }

    // Initialize last states
    last_catch_state = button_catch_isPressed();
    last_reset_state = button_reset_isPressed();

    printf("[Button] Initialized catch button on line 13\n");
    printf("[Button] Initialized reset button on line 14\n");
}

// ===== CATCH BUTTON FUNCTIONS =====
bool button_catch_isPressed(void)
{
    int value = gpiod_line_request_get_value(catch_request, LINE_CATCH_BUTTON);
    return (value == 0); // Active low
}

bool button_catch_wasJustPressed(void)
{
    bool current_state = button_catch_isPressed();
    bool just_pressed = (current_state && !last_catch_state);
    last_catch_state = current_state;
    return just_pressed;
}

// ===== RESET BUTTON FUNCTIONS =====
bool button_reset_isPressed(void)
{
    int value = gpiod_line_request_get_value(reset_request, LINE_RESET_BUTTON);
    return (value == 0); // Active low
}

bool button_reset_wasJustPressed(void)
{
    bool current_state = button_reset_isPressed();
    bool just_pressed = (current_state && !last_reset_state);
    last_reset_state = current_state;
    return just_pressed;
}

bool button_wasJustPressed(void)
{
    return button_catch_wasJustPressed();
}

void button_cleanup(void)
{
    if (catch_request)
    {
        gpiod_line_request_release(catch_request);
        catch_request = NULL;
    }
    if (reset_request)
    {
        gpiod_line_request_release(reset_request);
        reset_request = NULL;
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

bool button_catch_isPressed(void)
{
    return false;
}

bool button_catch_wasJustPressed(void)
{
    return false;
}

bool button_reset_isPressed(void)
{
    return false;
}

bool button_reset_wasJustPressed(void)
{
    return false;
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