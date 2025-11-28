#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
} Sprite;

// Load a sprite from a PNG file
bool sprite_load(Sprite* sprite, SDL_Renderer* renderer, const char* filename);

// Render sprite at position (x, y)
void sprite_render(Sprite* sprite, SDL_Renderer* renderer, int x, int y);

// Render sprite at position with scaling
void sprite_render_scaled(Sprite* sprite, SDL_Renderer* renderer, int x, int y, int w, int h);

// Render a portion of the sprite (for sprite sheets)
void sprite_render_clip(Sprite* sprite, SDL_Renderer* renderer, int x, int y, SDL_Rect* clip);

// Free sprite resources
void sprite_free(Sprite* sprite);

#endif // SPRITE_H