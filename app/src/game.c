#include "game.h"
#include "common.h"
#include "hal/display.h"
#include "hal/audio.h"
#include "player.h"
#include "map.h"
#include "rendering.h"
#include "input.h"
#include "sound_effects.h"
#include "music.h"
#include "catch.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

void game_run(void)
{
    SDL_Renderer *renderer = display_get_renderer();

    if (!input_initialize())
    {
        fprintf(stderr, "Warning: Failed to initialize input\n");
    }

    // Initialize audio
    if (!audio_init())
    {
        fprintf(stderr, "Warning: Failed to initialize audio\n");
    }

    // Load sound effects
    printf("Loading sound effects...\n");
    audio_load_sound("assets/sounds/catch.wav", SOUND_CATCH);
    
    // Initialize game systems
    Map game_map;
    map_init(&game_map, renderer);

    Player player;
    player_init(&player, 10, 7, renderer);

    bool music_has_started = false;
    
    // Initialize pet system with counts for each pet type
    // (bear_count, raccoon_count, deer_count, bigdeer_count)
    PetManager pets;
    pet_manager_init(&pets, renderer, 3, 3, 2, 2);
    
    // Spawn initial pets
    pet_spawn_initial(&pets, renderer);

    // Initialize music 
    if (!music_init())
    {
        fprintf(stderr, "Warning: Music initialization failed\n");
    }

    // Game state
    Uint32 last_move_time = 0;
    bool space_was_pressed = false;
    bool running = true;
    SDL_Event event;

    // Game loop
    while (running)
    {
        Uint32 current_time = SDL_GetTicks();

        // Start music after 12 second delay
        if (!music_has_started && current_time > 12000)
        {
            music_start_delayed();
            music_has_started = true;
        }

        // Handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                running = false;
            }
        }

        // Get current room
        Room *current_room = map_get_current_room(&game_map);

        // Update music based on player position (only after music has started)
        if (music_has_started)
        {
            music_update(current_room, player_get_grid_x(&player), player_get_grid_y(&player));
        }

        // Check for catch action
        if (input_is_catch_pressed(&space_was_pressed))
        {
            printf("Space pressed! Checking for pets...\n");
            
            // Check for adjacent pets
            Pet* pet = pet_check_adjacent(&pets, 
                                          player_get_grid_x(&player),
                                          player_get_grid_y(&player),
                                          current_room->id);
            if (pet != NULL) {
                audio_play_sound(SOUND_CATCH);
                pet_catch(&pets, pet);
                
                // Check if we need to respawn any pets
                pet_check_respawn(&pets, renderer);
            }
        }

        // Get movement input and update player
        InputDirection dir = input_get_direction();
        player_handle_movement(&player, dir, current_room->obstacles,
                               current_room->npcs, current_room->npc_count,
                               current_time, &last_move_time,
                               &pets, current_room->id);
        player_update_animation(&player);

        // Check for door transitions
        if (!player.is_moving)
        {
            Door *door = map_check_door_collision(&game_map,
                                                  player_get_grid_x(&player),
                                                  player_get_grid_y(&player));
            if (door != NULL) {
                int new_x = player.grid_x;
                int new_y = player.grid_y;
                map_transition_room(&game_map, door->target_room, &new_x, &new_y, door);

                // Reset player position in new room
                player.grid_x = new_x;
                player.grid_y = new_y;
                player.target_grid_x = new_x;
                player.target_grid_y = new_y;
                player.render_x = new_x * TILE_SIZE;
                player.render_y = new_y * TILE_SIZE;
                player.is_moving = false;

                // Change room music
                current_room = map_get_current_room(&game_map);
                music_change_room(current_room->music_path);
            }
        }

        // Render everything
        display_clear(COLOR_BACKGROUND_R, COLOR_BACKGROUND_G, COLOR_BACKGROUND_B);
        rendering_draw_obstacles(current_room->obstacles);
        rendering_draw_doors(current_room->doors, current_room->door_count);
        pet_render_all(&pets, renderer, current_room->id, 
                      player_get_grid_x(&player), player_get_grid_y(&player));
        rendering_draw_npcs(current_room->npcs, current_room->npc_count);
        rendering_draw_player(&player);
        display_present();

        // Frame timing
        SDL_Delay(FRAME_DELAY);
    }

    // Cleanup
    pet_manager_cleanup(&pets);
    input_cleanup();
    music_cleanup();
    audio_cleanup();
    map_cleanup(&game_map);
    player_cleanup(&player);

    printf("\n=== Game Over! ===\n");
}