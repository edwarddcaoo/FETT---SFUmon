#ifndef NPC_H
#define NPC_H
#include <stdbool.h>
#include "hal/sprite.h"
#include <SDL2/SDL.h>

typedef struct {
    int x;
    int y;
    bool caught;
    char name[20];
    Sprite sprite;  // Add sprite
} NPC;

void npc_init_all(NPC* npcs, int* count, SDL_Renderer* renderer);
bool npc_try_catch(NPC* npcs, int count, int player_x, int player_y, int* total_caught);
void npc_cleanup_all(NPC* npcs, int count);

#endif