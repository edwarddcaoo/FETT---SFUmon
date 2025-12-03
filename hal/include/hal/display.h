#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// initializes the display window and renderer
bool display_init(const char* title, int width, int height);

// cleans up the display resources
void display_cleanup(void);

// gets the renderer for drawing operations
SDL_Renderer* display_get_renderer(void);

// clears the display
void display_clear(int r, int g, int b);

// presents the rendered frames to the screen
void display_present(void);

#endif