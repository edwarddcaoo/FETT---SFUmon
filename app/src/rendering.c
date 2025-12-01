#include "rendering.h"
#include "common.h"
#include "quest.h"
#include "dialogue.h"   // gives access to dialogue_get_font()
#include <SDL2/SDL_ttf.h>
#include "hal/display.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

// void rendering_draw_obstacles(int obstacles[][GRID_WIDTH]) {
//     SDL_Renderer* renderer = display_get_renderer();
//     SDL_SetRenderDrawColor(renderer, COLOR_OBSTACLE_R, COLOR_OBSTACLE_G, COLOR_OBSTACLE_B, 255);

//     for (int y = 0; y < GRID_HEIGHT; y++) {
//         for (int x = 0; x < GRID_WIDTH; x++) {
//             if (obstacles[y][x] == 1) {
//                 SDL_Rect wall_rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
//                 SDL_RenderFillRect(renderer, &wall_rect);
//             }
//         }
//     }
// }

void rendering_draw_npcs(NPC *npcs, int count)
{
    SDL_Renderer *renderer = display_get_renderer();

    for (int i = 0; i < count; i++)
    {
        if (!npcs[i].caught)
        {

            // Just render the NPC sprite (no highlight, no interaction)
            sprite_render(&npcs[i].sprite, renderer,
                          npcs[i].x * TILE_SIZE,
                          npcs[i].y * TILE_SIZE);
        }
    }
}

void rendering_draw_player(Player *player)
{
    SDL_Renderer *renderer = display_get_renderer();
    // Cast away const since player_render doesn't modify player in a meaningful way
    player_render((Player *)player, renderer);
}

void rendering_draw_doors(Door *doors, int door_count)
{
    SDL_Renderer *renderer = display_get_renderer();

    for (int i = 0; i < door_count; i++)
    {
        Door *door = &doors[i];

        // Choose color based on door type
        if (door->type == DOOR_TYPE_STAIRS_DOWN)
        {
            SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255); // Blue for stairs down
        }
        else if (door->type == DOOR_TYPE_STAIRS_UP)
        {
            SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255); // Red for stairs up
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Brown for doors
        }

        SDL_Rect door_rect = {
            door->x * TILE_SIZE,
            door->y * TILE_SIZE,
            TILE_SIZE,
            TILE_SIZE};
        SDL_RenderFillRect(renderer, &door_rect);
    }
}

void rendering_draw_quest(SDL_Renderer* renderer)
{
    if (!quest_any_active())
        return;

    int start_x = 20;
    int start_y = 1100;   // bottom-left area for 800x480
    int line_height = 40;

    TTF_Font* font = dialogue_get_quest_font();
    if (!font)
        return;

    SDL_Color black = {0, 0, 0, 255};   // Main text only

    QuestID list[QUEST_COUNT];
    int count = quest_get_active_list(list, QUEST_COUNT);

    for (int i = 0; i < count; i++)
    {
        QuestID q = list[i];

        char buffer[256];
        snprintf(buffer, sizeof(buffer),
                 "%s (%d/%d)",
                 quest_get_desc(q),
                 quest_get_progress(q),
                 quest_get_needed(q));

        int y = start_y + (i * line_height);

        // ---- MAIN TEXT (BLACK, NO SHADOW) ----
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, buffer, black);
        if (!surf) continue;

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

        SDL_Rect dst = {start_x, y, surf->w, surf->h};
        SDL_FreeSurface(surf);

        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
}
