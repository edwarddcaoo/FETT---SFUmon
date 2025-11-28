#include "npc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void npc_init_all(NPC* npcs, int* count, SDL_Renderer* renderer) {
    *count = 3;
    
    // Professor Matthew
    npcs[0].x = 15;
    npcs[0].y = 7;
    npcs[0].caught = false;
    strcpy(npcs[0].name, "Professor Matthew");
    sprite_load(&npcs[0].sprite, renderer, "assets/sprites/npc/Matthew.png");
    
    // TA Navid
    npcs[1].x = 12;
    npcs[1].y = 10;
    npcs[1].caught = false;
    strcpy(npcs[1].name, "TA Navid");
    sprite_load(&npcs[1].sprite, renderer, "assets/sprites/npc/Navid.png");
    
    // TA Soroush
    npcs[2].x = 3;
    npcs[2].y = 3;
    npcs[2].caught = false;
    strcpy(npcs[2].name, "TA Soroush");
    sprite_load(&npcs[2].sprite, renderer, "assets/sprites/npc/Soroush.png");
}

bool npc_try_catch(NPC* npcs, int count, int player_x, int player_y, int* total_caught) {
    bool caught_someone = false;
    
    for (int i = 0; i < count; i++) {
        if (!npcs[i].caught) {
            int dx = abs(player_x - npcs[i].x);
            int dy = abs(player_y - npcs[i].y);
            
            if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
                npcs[i].caught = true;
                (*total_caught)++;
                printf("*** Caught %s! Total: %d/%d ***\n", npcs[i].name, *total_caught, count);
                caught_someone = true;
            }
        }
    }
    
    return caught_someone;
}

void npc_cleanup_all(NPC* npcs, int count) {
    for (int i = 0; i < count; i++) {
        sprite_free(&npcs[i].sprite);
    }
}