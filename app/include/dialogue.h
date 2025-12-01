#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>   // ⭐ REQUIRED for TTF_Font
#include <stdbool.h>

typedef struct
{
    bool active;
    bool finished;

    const char *full_text;
    char shown_text[2048];
    int text_index;
    Uint32 last_char_time;

    SDL_Texture *texture;
    SDL_Renderer *renderer;

    TTF_Font *font;        // dialogue font
    TTF_Font *quest_font;  // ⭐ NEW — quest UI font
} Dialogue;

extern Dialogue g_dialogue;

void dialogue_init(SDL_Renderer *renderer);
void dialogue_start(const char *text);
void dialogue_update_typewriter(void);
void dialogue_handle_key(SDL_Keycode key);
void dialogue_render(SDL_Renderer *renderer);
bool dialogue_is_active(void);
void dialogue_cleanup(void);
void dialogue_set_portrait(const char *path);

// Font getters
TTF_Font* dialogue_get_font(void);
TTF_Font* dialogue_get_quest_font(void);   // ⭐ NEW

#endif
