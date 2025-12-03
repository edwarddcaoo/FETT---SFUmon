#ifndef RENDERING_H
#define RENDERING_H

#include "common.h"
#include "player.h"
#include "npc.h"
#include "map.h"


// function initializations
void rendering_draw_npcs(NPC *npcs, int count);
void rendering_draw_player(Player *player);
void rendering_draw_doors(Door *doors, int door_count);
void rendering_draw_obstacles(int obstacles[GRID_HEIGHT][GRID_WIDTH]);
void rendering_draw_quest(SDL_Renderer* renderer);

#endif