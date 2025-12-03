#include "sprite.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

// loads the an image and creates the sprite
bool sprite_load(Sprite* sprite, SDL_Renderer* renderer, const char* filename) {
    if (!sprite || !renderer || !filename) {
        fprintf(stderr, "Sprite: Invalid parameters\n");
        return false;
    }

    // Load image as surface
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        fprintf(stderr, "Sprite: Failed to load '%s': %s\n", filename, IMG_GetError());
        return false;
    }

    // Create texture from surface
    sprite->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!sprite->texture) {
        fprintf(stderr, "Sprite: Failed to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    // Store dimensions
    sprite->width = surface->w;
    sprite->height = surface->h;

    // Free surface (we only need the texture now)
    SDL_FreeSurface(surface);

    printf("Sprite: Loaded '%s' (%dx%d)\n", filename, sprite->width, sprite->height);
    return true;
}

// renders the sprite at a specific position
void sprite_render(Sprite* sprite, SDL_Renderer* renderer, int x, int y) {
    if (!sprite || !sprite->texture || !renderer) {
        return;
    }

    SDL_Rect dest = {
        .x = x,
        .y = y,
        .w = sprite->width,
        .h = sprite->height
    };

    SDL_RenderCopy(renderer, sprite->texture, NULL, &dest);
}

// renders the sprite with custom dimensions at a specific position 
void sprite_render_scaled(Sprite* sprite, SDL_Renderer* renderer, int x, int y, int w, int h) {
    if (!sprite || !sprite->texture || !renderer) {
        return;
    }

    SDL_Rect dest = {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };

    SDL_RenderCopy(renderer, sprite->texture, NULL, &dest);
}

// renders a portion of the sprite for animation frames
void sprite_render_clip(Sprite* sprite, SDL_Renderer* renderer, int x, int y, SDL_Rect* clip) {
    if (!sprite || !sprite->texture || !renderer) {
        return;
    }

    SDL_Rect dest = {
        .x = x,
        .y = y,
        .w = clip ? clip->w : sprite->width,
        .h = clip ? clip->h : sprite->height
    };

    SDL_RenderCopy(renderer, sprite->texture, clip, &dest);
}

// frees sprite resources and resets sprite data
void sprite_free(Sprite* sprite) {
    if (!sprite) {
        return;
    }

    if (sprite->texture) {
        SDL_DestroyTexture(sprite->texture);
        sprite->texture = NULL;
    }

    sprite->width = 0;
    sprite->height = 0;
}