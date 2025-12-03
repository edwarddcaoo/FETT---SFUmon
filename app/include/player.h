#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#include "hal/sprite.h"
#include <SDL2/SDL.h>
#include "common.h"
#include "input.h"

typedef enum
{
    DIR_UP = 0,
    DIR_DOWN = 1,
    DIR_LEFT = 2,
    DIR_RIGHT = 3,
    DIR_COUNT = 4
} Direction;

typedef struct
{
    int grid_x;
    int grid_y;
    float render_x;
    float render_y;
    bool is_moving;
    int target_grid_x;
    int target_grid_y;

    // Add sprite data
    Sprite sprites[DIR_COUNT];
    Direction current_direction;

    bool just_teleported;

} Player;


// function initializations
void player_init(Player *player, int start_x, int start_y, SDL_Renderer *renderer);

void player_handle_movement(Player *player, InputDirection dir,
                            int obstacles[][GRID_WIDTH], void *npcs_ptr, int npc_count,
                            unsigned int current_time, unsigned int *last_move_time,
                            void *pets_ptr, int current_room_id);

void player_update_animation(Player *player);

void player_render(Player *player, SDL_Renderer *renderer);

void player_cleanup(Player *player);

int player_get_grid_x(const Player *player);
int player_get_grid_y(const Player *player);

#endif