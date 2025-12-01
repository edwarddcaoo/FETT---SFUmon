#include "dialogue.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <stdio.h>

#define FONT_PATH "assets/font/rainyhearts.ttf"
#define FONT_SIZE 100
#define TEXT_X 90
#define TEXT_Y 870
#define MAX_LINE_WIDTH 1750

Dialogue g_dialogue = {0};

static void render_text(SDL_Renderer *renderer, const char *text)
{
    if (!g_dialogue.font)
        return;

    SDL_Color red = {46, 18, 18, 255};

    SDL_Surface *surf = TTF_RenderUTF8_Blended_Wrapped(
        g_dialogue.font, text, red, MAX_LINE_WIDTH
    );
    if (!surf) return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_Rect dst = {TEXT_X, TEXT_Y, surf->w, surf->h};
    SDL_FreeSurface(surf);

    SDL_RenderCopy(renderer, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
}

void dialogue_init(SDL_Renderer *renderer)
{
    g_dialogue.renderer = renderer;  // ⭐ STORE THE RENDERER

    if (TTF_Init() < 0)
        printf("TTF Init error: %s\n", TTF_GetError());

    g_dialogue.font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!g_dialogue.font)
        printf("Font load error: %s\n", TTF_GetError());

    // Load default dialogue box image
    g_dialogue.texture = IMG_LoadTexture(renderer, "assets/dialogue/navidDialogue.png");
    if (!g_dialogue.texture)
        printf("Failed to load initial PNG: %s\n", IMG_GetError());
}

void dialogue_start(const char *text)
{
    g_dialogue.active = true;
    g_dialogue.finished = false;
    g_dialogue.full_text = text;

    g_dialogue.text_index = 0;
    g_dialogue.shown_text[0] = '\0';
    g_dialogue.last_char_time = SDL_GetTicks();
}

void dialogue_update_typewriter(void)
{
    if (!g_dialogue.active || g_dialogue.finished)
        return;

    Uint32 now = SDL_GetTicks();
    if (now - g_dialogue.last_char_time > 35)
    {
        g_dialogue.last_char_time = now;

        g_dialogue.shown_text[g_dialogue.text_index] =
            g_dialogue.full_text[g_dialogue.text_index];

        g_dialogue.text_index++;

        if (g_dialogue.full_text[g_dialogue.text_index] == '\0')
            g_dialogue.finished = true;

        g_dialogue.shown_text[g_dialogue.text_index] = '\0';
    }
}

void dialogue_handle_key(SDL_Keycode key)
{
    if (!g_dialogue.active)
        return;

    if (key == SDLK_t)
    {
        if (!g_dialogue.finished)
        {
            strcpy(g_dialogue.shown_text, g_dialogue.full_text);
            g_dialogue.finished = true;
        }
        else
        {
            g_dialogue.active = false;
        }
    }
}

void dialogue_render(SDL_Renderer *renderer)
{
    if (!g_dialogue.active)
        return;

    SDL_RenderCopy(renderer, g_dialogue.texture, NULL, NULL);
    render_text(renderer, g_dialogue.shown_text);
}

bool dialogue_is_active(void)
{
    return g_dialogue.active;
}

void dialogue_cleanup(void)
{
    if (g_dialogue.texture)
        SDL_DestroyTexture(g_dialogue.texture);

    if (g_dialogue.font)
        TTF_CloseFont(g_dialogue.font);

    TTF_Quit();
}

void dialogue_set_portrait(const char *path)
{
    if (g_dialogue.texture)
        SDL_DestroyTexture(g_dialogue.texture);

    g_dialogue.texture = IMG_LoadTexture(g_dialogue.renderer, path);

    if (!g_dialogue.texture)
        printf("Portrait load error %s: %s\n", path, IMG_GetError());
}
