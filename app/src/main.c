#include <stdio.h>
#include "hal/display.h"
#include "common.h"
#include "game.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // Initialize display
    if (!display_init("SFUmon - Smooth Movement", WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        fprintf(stderr, "Failed to initialize display\n");
        return 1;
    }

    printf("=== SFUmon Game ===\n");
    printf("Use ARROW KEYS or WASD to move\n");
    printf("Walk next to professors (they'll highlight) and press SPACE to catch\n");
    printf("Walk onto doors/stairs to change rooms\n");
    printf("Press ESC to quit.\n\n");

    // Run the game
    game_run();

    // Cleanup
    display_cleanup();

    return 0;
}