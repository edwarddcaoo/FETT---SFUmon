#include "rendering.h"
#include "common.h"
#include "hal/display.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

void rendering_draw_obstacles(int obstacles[][GRID_WIDTH]) {
    SDL_Renderer* renderer = display_get_renderer();
    SDL_SetRenderDrawColor(renderer, COLOR_OBSTACLE_R, COLOR_OBSTACLE_G, COLOR_OBSTACLE_B, 255);
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (obstacles[y][x] == 1) {
                SDL_Rect wall_rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                SDL_RenderFillRect(renderer, &wall_rect);
            }
        }
    }
}

void rendering_draw_npcs(NPC* npcs, int count) {
    SDL_Renderer* renderer = display_get_renderer();
    
    for (int i = 0; i < count; i++) {
        if (!npcs[i].caught) {

            // Just render the NPC sprite (no highlight, no interaction)
            sprite_render(&npcs[i].sprite, renderer, 
                         npcs[i].x * TILE_SIZE, 
                         npcs[i].y * TILE_SIZE);
        }
    }
}

void rendering_draw_player(Player* player) {
    SDL_Renderer* renderer = display_get_renderer();
    // Cast away const since player_render doesn't modify player in a meaningful way
    player_render((Player*)player, renderer);
}

void rendering_draw_doors(Door* doors, int door_count) {
    SDL_Renderer* renderer = display_get_renderer();
    
    for (int i = 0; i < door_count; i++) {
        Door* door = &doors[i];
        
        // Choose color based on door type
        if (door->type == DOOR_TYPE_STAIRS_DOWN) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255); // Blue for stairs down
        } else if (door->type == DOOR_TYPE_STAIRS_UP) {
            SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255); // Red for stairs up
        } else {
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Brown for doors
        }
        
        SDL_Rect door_rect = {
            door->x * TILE_SIZE,
            door->y * TILE_SIZE,
            TILE_SIZE,
            TILE_SIZE
        };
        SDL_RenderFillRect(renderer, &door_rect);
    }
}