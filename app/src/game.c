#include "game.h"
#include "common.h"
#include "hal/display.h"
#include "hal/audio.h"
#include "player.h"
#include "map.h"
#include "rendering.h"
#include "input.h"
#include "sound_effects.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Helper function to check if player is near Professor Matthew
static bool is_near_professor_matthew(Room* room, int player_x, int player_y) {
    for (int i = 0; i < room->npc_count; i++) {
        if (strcmp(room->npcs[i].name, "Professor Matthew") == 0 && !room->npcs[i].caught) {
            int dx = abs(player_x - room->npcs[i].x);
            int dy = abs(player_y - room->npcs[i].y);
            int distance = dx + dy; // Manhattan distance
            
            // Within 3 tiles
            if (distance <= 3) {
                return true;
            }
        }
    }
    return false;
}

void game_run(void) {
    SDL_Renderer* renderer = display_get_renderer();
    
    // Initialize audio
    if (!audio_init()) {
        fprintf(stderr, "Warning: Failed to initialize audio\n");
    }
    
    // Preload sound effects
    printf("Loading sound effects...\n");
    audio_load_sound("assets/sounds/catch.wav", SOUND_CATCH);
    audio_load_sound("assets/sounds/door.wav", SOUND_DOOR);
    
    // Initialize game systems
    Map game_map;
    map_init(&game_map, renderer);
    
    Player player;
    player_init(&player, 10, 7, renderer);
    
    int total_caught = 0;
    
    // Music state tracking
    char current_music_path[128] = "";
    bool was_near_professor = false;
    bool music_started = false;  // Track if initial music has started
    
    // Preload initial room music (don't play yet)
    printf("Loading music...\n");
    Room* current_room = map_get_current_room(&game_map);
    strcpy(current_music_path, current_room->music_path);
    audio_load_music(current_music_path);
    printf("Music loaded, will start shortly...\n");
    
    // Game state
    Uint32 last_move_time = 0;
    bool space_was_pressed = false;
    bool running = true;
    SDL_Event event;
    
    // Game loop
    while (running) {
        Uint32 current_time = SDL_GetTicks();
        
        // Start music after a short delay (500ms) to let everything initialize
        if (!music_started && current_time > 500) {
            audio_play_music();
            audio_set_music_volume(64);
            music_started = true;
            printf("🎵 Music started!\n");
        }
        
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }
        
        // Get current room
        current_room = map_get_current_room(&game_map);
        
        // Check if near Professor Matthew for encounter music
        bool near_professor = is_near_professor_matthew(current_room, 
                                                        player_get_grid_x(&player),
                                                        player_get_grid_y(&player));
        
        // Handle music transitions - only when state changes
        if (near_professor && !was_near_professor) {
            // Just entered proximity - switch to encounter music
            audio_stop_music();
            audio_load_music("assets/music/matthew.ogg");
            audio_play_music();
            audio_set_music_volume(64);
            was_near_professor = true;
            printf("🎵 Encounter music started!\n");
        } else if (!near_professor && was_near_professor) {
            // Just left proximity - switch back to room music
            audio_stop_music();
            audio_load_music(current_music_path);
            audio_play_music();
            audio_set_music_volume(64);
            was_near_professor = false;
            printf("🎵 Room music resumed\n");
        }
        
        // Handle input
        const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
        
        // Check for catch action
        if (input_is_catch_pressed(&space_was_pressed)) {
            printf("Space pressed! Checking for professors...\n");
            bool caught = npc_try_catch(current_room->npcs, current_room->npc_count, 
                         player_get_grid_x(&player), 
                         player_get_grid_y(&player), &total_caught);
            
            if (caught) {
                audio_play_sound(SOUND_CATCH);
            }
        }
        
        // Update player movement
        player_handle_movement(&player, keyboard_state, current_room->obstacles, 
                              current_room->npcs, current_room->npc_count,
                              current_time, &last_move_time);
        player_update_animation(&player);
        
        // Check for door transitions
        if (!player.is_moving) {
            Door* door = map_check_door_collision(&game_map, 
                                                  player_get_grid_x(&player), 
                                                  player_get_grid_y(&player));
            if (door != NULL) {
                // Play door sound
                audio_play_sound(SOUND_DOOR);
                
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
                strcpy(current_music_path, current_room->music_path);
                audio_stop_music();
                audio_load_music(current_music_path);
                audio_play_music();
                audio_set_music_volume(64);
                was_near_professor = false;  // Reset when changing rooms
            }
        }
        
        // Render everything
        display_clear(COLOR_BACKGROUND_R, COLOR_BACKGROUND_G, COLOR_BACKGROUND_B);
        rendering_draw_obstacles(current_room->obstacles);
        rendering_draw_doors(current_room->doors, current_room->door_count);
        rendering_draw_npcs(current_room->npcs, current_room->npc_count, 
                           player_get_grid_x(&player), 
                           player_get_grid_y(&player));
        rendering_draw_player(&player);
        display_present();
        
        // Frame timing
        SDL_Delay(FRAME_DELAY);
    }
    
    // Cleanup
    audio_cleanup();
    map_cleanup(&game_map);
    player_cleanup(&player);
    
    printf("\n=== Game Over! ===\n");
    printf("You caught %d professors total.\n", total_caught);
}