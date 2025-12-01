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
#include "dialogue.h"
#include "catch.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <rendering_ui.h>
#include <SDL2/SDL_ttf.h>

void game_run(void)
{
    SDL_Renderer *renderer = display_get_renderer();

    // Dialogue system first
    dialogue_init(renderer);

    if (!input_initialize())
        fprintf(stderr, "Warning: Failed to initialize input\n");
    if (!audio_init())
        fprintf(stderr, "Warning: Failed to initialize audio\n");

    printf("Loading sound effects...\n");
    audio_load_sound("assets/sounds/catch.wav", SOUND_CATCH);

    if (TTF_Init() == -1) {
        fprintf(stderr, "Warning: Failed to initialize SDL_ttf: %s\n", TTF_GetError());
    }   

    // ------------------------------------------
    // MAP + PLAYER INITIALIZATION
    // ------------------------------------------
    Map game_map;
    map_init(&game_map, renderer);

    Player player;
    player_init(&player, 10, 7, renderer);

    bool music_has_started = false;

    // ------------------------------------------
    // PET SYSTEM INITIALIZATION
    // ------------------------------------------
    PetManager pets;
    pet_manager_init(&pets, renderer, 3, 3, 2, 2);
    pet_spawn_initial(&pets, renderer, &game_map);

    if (!rendering_ui_init()) {
        fprintf(stderr, "Warning: Failed to initialize UI rendering\n");
    }

    if (!music_init())
        fprintf(stderr, "Warning: Music initialization failed\n");

    Uint32 last_move_time = 0;
    bool space_was_pressed = false;
    bool running = true;
    SDL_Event event;

    while (running)
    {
        Uint32 current_time = SDL_GetTicks();

        if (!music_has_started && current_time > 12000)
        {
            music_start_delayed();
            music_has_started = true;
        }

        // ------------------------------------------
        // EVENT LOOP
        // ------------------------------------------
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouse_x = event.button.x;
                    int mouse_y = event.button.y;
        
                    if (rendering_ui_check_reset_click(mouse_x, mouse_y)) {
                        rendering_ui_reset_catches(&pets);
                    }
                }
            }
            if (event.type == SDL_KEYDOWN)
            {
                SDL_Keycode key = event.key.keysym.sym;

                // Dialogue consumes input first
                if (dialogue_is_active())
                {
                    dialogue_handle_key(key);
                    continue;
                }

                if (key == SDLK_ESCAPE)
                    running = false;

                // TALK TO NPC - T key
                if (key == SDLK_t)
                {
                    Room *room = map_get_current_room(&game_map);

                    for (int i = 0; i < room->npc_count; i++)
                    {
                        NPC *npc = &room->npcs[i];

                        bool touching =
                            (player.grid_x == npc->x && player.grid_y == npc->y - 1) ||
                            (player.grid_x == npc->x && player.grid_y == npc->y + 1) ||
                            (player.grid_x == npc->x - 1 && player.grid_y == npc->y) ||
                            (player.grid_x == npc->x + 1 && player.grid_y == npc->y);

                        if (!touching)
                            continue;

                        // Dialogue routes based on NPC name
                        if (strcmp(npc->name, "TA Navid") == 0)
                        {
                            dialogue_set_portrait("assets/dialogue/navidDialogue.png");
                            dialogue_start(
                                "The bears keep attacking me!! Please help me get rid of some..\nQuest Started: Catch 5 Bears"
                            );
                            break;
                        }

                        if (strcmp(npc->name, "TA Soroush") == 0)
                        {
                            dialogue_set_portrait("assets/dialogue/soroushDialogue.png");
                            dialogue_start(
                                "My lunch keeps going missing... I feel like I know the culprits. Could you help?\nQuest Started: Catch 3 Raccoons"
                            );
                            break;
                        }

                        if (strcmp(npc->name, "Professor Matthew") == 0)
                        {
                            dialogue_set_portrait("assets/dialogue/matthewDialogue.png");
                            dialogue_start(
                                "Hey why are you out of class?? Nevermind.. could you help me get some deer?\nQuest Started: Catch 4 Deers"
                            );
                            break;
                        }
                    }
                }
            }
        }

        Room *current_room = map_get_current_room(&game_map);

        if (music_has_started)
            music_update(current_room,
                         player_get_grid_x(&player),
                         player_get_grid_y(&player));

        // ------------------------------------------
        // DIALOGUE FREEZE MODE
        // ------------------------------------------
        if (dialogue_is_active())
        {
            dialogue_update_typewriter();

            // Draw full frame with no movement
            display_clear(0,0,0);

            map_render_background(&game_map, renderer);

            rendering_draw_doors(current_room->doors, current_room->door_count);

            pet_render_all(&pets, renderer,
                           current_room->id,
                           player_get_grid_x(&player),
                           player_get_grid_y(&player));

            rendering_draw_npcs(current_room->npcs, current_room->npc_count);
            rendering_draw_player(&player);

            rendering_ui_draw_hud(&pets);
            dialogue_render(renderer);

            display_present();
            SDL_Delay(FRAME_DELAY);
            continue;
        }

        // ------------------------------------------
        // PET CATCHING (SPACE)
        // ------------------------------------------
        if (input_is_catch_pressed(&space_was_pressed))
        {
            Pet *p = pet_check_adjacent(&pets,
                                        player_get_grid_x(&player),
                                        player_get_grid_y(&player),
                                        current_room->id);

            if (p != NULL)
            {
                audio_play_sound(SOUND_CATCH);
                rendering_ui_increment_catch(p->type);  // <-- ADD THIS LINE
                pet_catch(&pets, p);
                pet_check_respawn(&pets, renderer, &game_map);
            }
        }

        // ------------------------------------------
        // PLAYER MOVEMENT
        // ------------------------------------------
        InputDirection dir = input_get_direction();

        player_handle_movement(&player, dir,
                               current_room->obstacles,
                               current_room->npcs,
                               current_room->npc_count,
                               current_time,
                               &last_move_time,
                               &pets,
                               current_room->id);

        player_update_animation(&player);

        // ------------------------------------------
        // ROOM TRANSITION
        // ------------------------------------------
        if (!player.is_moving)
        {
            Door *door = map_check_door_collision(&game_map,
                                                  player_get_grid_x(&player),
                                                  player_get_grid_y(&player));

            if (door)
            {
                int new_x = player.grid_x;
                int new_y = player.grid_y;

                map_transition_room(&game_map, door->target_room,
                                    &new_x, &new_y, door);

                player.grid_x = new_x;
                player.grid_y = new_y;
                player.target_grid_x = new_x;
                player.target_grid_y = new_y;
                player.render_x = new_x * TILE_SIZE;
                player.render_y = new_y * TILE_SIZE;
                player.is_moving = false;

                current_room = map_get_current_room(&game_map);
                music_change_room(current_room->music_path);
            }
        }

        // ------------------------------------------
        // NORMAL FRAME RENDERING
        // ------------------------------------------
        display_clear(0,0,0);

        // Draw background FIRST
        map_render_background(&game_map, renderer);

        // Draw all gameplay layers
        //rendering_draw_obstacles(current_room->obstacles);
        rendering_draw_doors(current_room->doors, current_room->door_count);

        pet_render_all(&pets, renderer,
                       current_room->id,
                       player_get_grid_x(&player),
                       player_get_grid_y(&player));

        rendering_draw_npcs(current_room->npcs, current_room->npc_count);
        rendering_draw_player(&player);

        // Draw UI HUD (inventory and reset button)
        rendering_ui_draw_hud(&pets);

        display_present();
        SDL_Delay(FRAME_DELAY);
    }

    // ------------------------------------------
    // CLEANUP
    // ------------------------------------------
    pet_manager_cleanup(&pets);
    rendering_ui_cleanup();
    TTF_Quit();
    dialogue_cleanup();
    input_cleanup();
    music_cleanup();
    audio_cleanup();
    map_cleanup(&game_map);
    player_cleanup(&player);

    printf("\n=== Game Over! ===\n");
}