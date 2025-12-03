#include "player.h"
#include "common.h"
#include "npc.h"
#include "catch.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

void player_init(Player *player, int start_x, int start_y, SDL_Renderer *renderer)
{
    player->grid_x = start_x;
    player->grid_y = start_y;
    player->render_x = start_x * TILE_SIZE;
    player->render_y = start_y * TILE_SIZE;
    player->is_moving = false;
    player->target_grid_x = start_x;
    player->target_grid_y = start_y;
    player->current_direction = DIR_DOWN; // Start facing down

    player->just_teleported = false;

    // Load all directional sprites
    const char *sprite_paths[DIR_COUNT] = {
        "assets/sprites/player/player_down.png",
        "assets/sprites/player/player_up.png",
        "assets/sprites/player/player_left.png",
        "assets/sprites/player/player_right.png"};

    for (int i = 0; i < DIR_COUNT; i++)
    {
        if (!sprite_load(&player->sprites[i], renderer, sprite_paths[i]))
        {
            fprintf(stderr, "Player: Failed to load sprite %d from %s\n", i, sprite_paths[i]);
        }
    }
}

// ensures the player is moving into a valid cell
void player_handle_movement(Player *player, InputDirection dir,
                            int obstacles[][GRID_WIDTH], void *npcs_ptr, int npc_count,
                            unsigned int current_time, unsigned int *last_move_time,
                            void *pets_ptr, int current_room_id)
{
    NPC *npcs = (NPC *)npcs_ptr;
    PetManager *pets = (PetManager *)pets_ptr;

    if (!player->is_moving && current_time - *last_move_time > MOVE_DELAY)
    {
        int new_x = player->grid_x;
        int new_y = player->grid_y;
        Direction new_direction = player->current_direction;

        // Map InputDirection to movement
        switch (dir)
        {
        case INPUT_UP:
            new_y--;
            new_direction = DIR_UP;
            break;
        case INPUT_DOWN:
            new_y++;
            new_direction = DIR_DOWN;
            break;
        case INPUT_LEFT:
            new_x--;
            new_direction = DIR_LEFT;
            break;
        case INPUT_RIGHT:
            new_x++;
            new_direction = DIR_RIGHT;
            break;
        case INPUT_NONE:
        default:
            // No movement
            break;
        }

        // Update direction even if we don't move (so player can turn in place)
        player->current_direction = new_direction;

        // If movement was requested
        if (new_x != player->grid_x || new_y != player->grid_y)
        {
            // Check if professor blocks the tile
            bool professor_blocking = false;
            for (int i = 0; i < npc_count; i++)
            {
                if (!npcs[i].caught && npcs[i].x == new_x && npcs[i].y == new_y)
                {
                    professor_blocking = true;
                    break;
                }
            }

            // Check if pet blocks the tile
            bool pet_blocking = pet_blocks_movement(pets, new_x, new_y, current_room_id);

            // Validate move
            if (new_x >= 0 && new_x < GRID_WIDTH &&
                new_y >= 0 && new_y < GRID_HEIGHT &&
                obstacles[new_y][new_x] == 0 &&
                !professor_blocking &&
                !pet_blocking)
            {
                player->target_grid_x = new_x;
                player->target_grid_y = new_y;
                player->is_moving = true;
                *last_move_time = current_time;
            }
        }
    }
}

// updates the animation of the player walking
void player_update_animation(Player *player)
{
    if (player->is_moving)
    {
        float target_render_x = player->target_grid_x * TILE_SIZE;
        float target_render_y = player->target_grid_y * TILE_SIZE;

        // X-coordinate
        if (player->render_x < target_render_x)
        {
            player->render_x += ANIMATION_SPEED;
            if (player->render_x >= target_render_x)
            {
                player->render_x = target_render_x;
            }
        }
        else if (player->render_x > target_render_x)
        {
            player->render_x -= ANIMATION_SPEED;
            if (player->render_x <= target_render_x)
            {
                player->render_x = target_render_x;
            }
        }

        // Y-coordinate
        if (player->render_y < target_render_y)
        {
            player->render_y += ANIMATION_SPEED;
            if (player->render_y >= target_render_y)
            {
                player->render_y = target_render_y;
            }
        }
        else if (player->render_y > target_render_y)
        {
            player->render_y -= ANIMATION_SPEED;
            if (player->render_y <= target_render_y)
            {
                player->render_y = target_render_y;
            }
        }

        // Check if reached target
        if (player->render_x == target_render_x && player->render_y == target_render_y)
        {
            player->grid_x = player->target_grid_x;
            player->grid_y = player->target_grid_y;
            player->is_moving = false;
        }
    }
}

void player_render(Player *player, SDL_Renderer *renderer)
{
    if (!player || !renderer)
    {
        return;
    }

    sprite_render(&player->sprites[player->current_direction], renderer,
                  (int)player->render_x, (int)player->render_y);
}

void player_cleanup(Player *player)
{
    if (!player)
    {
        return;
    }

    for (int i = 0; i < DIR_COUNT; i++)
    {
        sprite_free(&player->sprites[i]);
    }
}

int player_get_grid_x(const Player *player)
{
    return player->grid_x;
}

int player_get_grid_y(const Player *player)
{
    return player->grid_y;
}