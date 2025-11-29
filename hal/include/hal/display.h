#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>

bool display_init(const char* title, int width, int height);

void display_cleanup(void);

SDL_Renderer* display_get_renderer(void);

void display_clear(int r, int g, int b);

void display_present(void);

#endif