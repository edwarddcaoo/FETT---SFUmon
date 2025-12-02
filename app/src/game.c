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
#include "quest.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <rendering_ui.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

void show_splash_screen(SDL_Renderer *renderer, const char *image_path, Uint32 duration_ms)
{
    // Load the splash image
    SDL_Surface *surface = IMG_Load(image_path);
    if (!surface)
    {
        fprintf(stderr, "Failed to load splash screen: %s\n", IMG_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        fprintf(stderr, "Failed to create splash texture: %s\n", SDL_GetError());
        return;
    }

    // Show the splash screen
    Uint32 start_time = SDL_GetTicks();
    SDL_Event event;
    bool skip = false;

    while (SDL_GetTicks() - start_time < duration_ms && !skip)
    {
        // Allow skipping with any key
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                event.type == SDL_KEYDOWN ||
                event.type == SDL_MOUSEBUTTONDOWN)
            {
                skip = true;
            }
        }

        // Clear and render splash
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Stretch splash to fill screen
        SDL_Rect dest = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, texture, NULL, &dest);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
}

void game_run(void)
{
    SDL_Renderer *renderer = display_get_renderer();

    show_splash_screen(renderer, "assets/sfumonTitle.png", 5000);

    dialogue_init(renderer);

    if (!input_initialize())
        fprintf(stderr, "Warning: Failed to initialize input\n");
    if (!audio_init())
        fprintf(stderr, "Warning: Failed to initialize audio\n");

    audio_load_sound("assets/sounds/catch.wav", SOUND_CATCH);

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "Warning: Failed to initialize SDL_ttf: %s\n", TTF_GetError());
    }

    // ------------------------------------------
    // MAP + PLAYER INITIALIZATION
    // ------------------------------------------
    Map game_map;
    map_init(&game_map, renderer);

    Player player;
    player_init(&player, 20, 11, renderer);

    bool music_has_started = false;

    PetManager pets;
    pet_manager_init(&pets, renderer, 3, 3, 2, 2);
    pet_spawn_initial(&pets, renderer, &game_map);

    if (!rendering_ui_init())
    {
        fprintf(stderr, "Warning: Failed to initialize UI rendering\n");
    }

    if (!music_init())
        fprintf(stderr, "Warning: Music initialization failed\n");

    Uint32 last_move_time = 0;
    bool space_was_pressed = false;
    bool interact_was_pressed = false;
    bool reset_was_pressed = false;
    bool running = true;
    SDL_Event event;

    // ==========================================
    // MAIN GAME LOOP
    // ==========================================
    while (running)
    {
        Uint32 current_time = SDL_GetTicks();

        if (!music_has_started && current_time > 5000)
        {
            music_start_delayed();
            music_has_started = true;
        }

        // ------------------------------------------
        // EVENT POLLING (SDL events only)
        // ------------------------------------------
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                running = false;

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int mouse_x = event.button.x;
                    int mouse_y = event.button.y;

                    if (rendering_ui_check_reset_click(mouse_x, mouse_y))
                    {
                        rendering_ui_reset_catches(&pets);
                    }
                }
            }
        }
        // END OF EVENT POLLING

        // Poll input state
        input_poll_once_per_frame();

        Room *current_room = map_get_current_room(&game_map);

        if (music_has_started)
            music_update(current_room, player.grid_x, player.grid_y);

        // ------------------------------------------
        // DIALOGUE HANDLING (T key on host, button on target)
        // ------------------------------------------
        if (input_is_interact_pressed(&interact_was_pressed))
        {
            // If dialogue is active, close it
            if (dialogue_is_active())
            {
                dialogue_handle_key(SDLK_t); // Close dialogue
            }
            else
            {
                // Try to talk to nearby NPC
                for (int i = 0; i < current_room->npc_count; i++)
                {
                    NPC *npc = &current_room->npcs[i];

                    bool touching =
                        (player.grid_x == npc->x && player.grid_y == npc->y - 1) ||
                        (player.grid_x == npc->x && player.grid_y == npc->y + 1) ||
                        (player.grid_x == npc->x - 1 && player.grid_y == npc->y) ||
                        (player.grid_x == npc->x + 1 && player.grid_y == npc->y);

                    if (!touching)
                        continue;

                    //-------------------------------
                    // NAVID
                    //-------------------------------
                    if (strcmp(npc->name, "TA Navid") == 0)
                    {
                        QuestID q = QUEST_BEAR_5;

                        if (quest_is_ready_to_turn_in(q))
                        {
                            dialogue_set_portrait("assets/dialogue/navidDialogue.png");
                            dialogue_start("Thank you! Those bears were terrifying.");
                            quest_complete_if_ready(q);
                            break;
                        }

                        if (quest_is_in_progress(q))
                        {
                            char msg[128];
                            snprintf(msg, sizeof(msg),
                                     "Still working on those bears?\n(%d/%d)",
                                     quest_get_progress(q),
                                     quest_get_needed(q));

                            dialogue_set_portrait("assets/dialogue/navidDialogue.png");
                            dialogue_start(msg);
                            break;
                        }

                        dialogue_set_portrait("assets/dialogue/navidDialogue.png");
                        dialogue_start(
                            "The bears keep attacking me!! Please help me get rid of some..\n"
                            "Quest Started: Catch 5 Bears");

                        quest_start(q, 5, "Catch 5 Bears and report back to Navid");
                        break;
                    }

                    //-------------------------------
                    // SOROUSH
                    //-------------------------------
                    if (strcmp(npc->name, "TA Soroush") == 0)
                    {
                        QuestID q = QUEST_RACCOON_3;

                        if (quest_is_ready_to_turn_in(q))
                        {
                            dialogue_set_portrait("assets/dialogue/soroushDialogue.png");
                            dialogue_start("Thanks! I can finally eat my lunches in peace...");
                            quest_complete_if_ready(q);
                            break;
                        }

                        if (quest_is_in_progress(q))
                        {
                            char msg[128];
                            snprintf(msg, sizeof(msg),
                                     "Hey! Haven't caught those rascals yet?\n(%d/%d)",
                                     quest_get_progress(q),
                                     quest_get_needed(q));

                            dialogue_set_portrait("assets/dialogue/soroushDialogue.png");
                            dialogue_start(msg);
                            break;
                        }

                        dialogue_set_portrait("assets/dialogue/soroushDialogue.png");
                        dialogue_start(
                            "My lunch keeps going missing... I think I know who.\n"
                            "Quest Started: Catch 3 Raccoons");

                        quest_start(q, 3, "Catch 3 Raccoons and report back to Soroush");
                        break;
                    }

                    //-------------------------------
                    // MATTHEW
                    //-------------------------------
                    if (strcmp(npc->name, "Professor Matthew") == 0)
                    {
                        QuestID q = QUEST_DEER_4;

                        if (quest_is_ready_to_turn_in(q))
                        {
                            dialogue_set_portrait("assets/dialogue/matthewDialogue.png");
                            dialogue_start("Great work! Those deer were messing up my lecture notes.");
                            quest_complete_if_ready(q);
                            break;
                        }

                        if (quest_is_in_progress(q))
                        {
                            char msg[128];
                            snprintf(msg, sizeof(msg),
                                     "Back already? Still need those deer gone!\n(%d/%d)",
                                     quest_get_progress(q),
                                     quest_get_needed(q));

                            dialogue_set_portrait("assets/dialogue/matthewDialogue.png");
                            dialogue_start(msg);
                            break;
                        }

                        dialogue_set_portrait("assets/dialogue/matthewDialogue.png");
                        dialogue_start(
                            "Hey why are you out of class?? Nevermind.. I need some small deer.\n"
                            "Quest Started: Catch 4 Small Deer");

                        quest_start(q, 4, "Catch 4 Deer and report back to Matthew");
                        break;
                    }

                    //-------------------------------
                    // MORTEZA
                    //-------------------------------
                    if (strcmp(npc->name, "TA Morteza") == 0)
                    {
                        QuestID q = QUEST_BIGDEER_2;

                        if (quest_is_ready_to_turn_in(q))
                        {
                            dialogue_set_portrait("assets/dialogue/mortezaDialogue.png");
                            dialogue_start("Perfect! These giant deer were messing with my research.");
                            quest_complete_if_ready(q);
                            break;
                        }

                        if (quest_is_in_progress(q))
                        {
                            char msg[128];
                            snprintf(msg, sizeof(msg),
                                     "Still hunting those big deer?\n(%d/%d)",
                                     quest_get_progress(q),
                                     quest_get_needed(q));

                            dialogue_set_portrait("assets/dialogue/mortezaDialogue.png");
                            dialogue_start(msg);
                            break;
                        }

                        dialogue_set_portrait("assets/dialogue/mortezaDialogue.png");
                        dialogue_start(
                            "Hey! It's hard to research with all these huge deer running around!\n"
                            "Quest Started: Catch 2 Big Deer");

                        quest_start(q, 2, "Catch 2 Big Deer and report back to Morteza");
                        break;
                    }
                }
            }
        }

        // ------------------------------------------
        // DIALOGUE FREEZE MODE
        // ------------------------------------------
        if (dialogue_is_active())
        {
            dialogue_update_typewriter();

            display_clear(0, 0, 0);
            map_render_background(&game_map, renderer);

            rendering_draw_doors(current_room->doors, current_room->door_count);
            pet_render_all(&pets, renderer,
                           current_room->id,
                           player.grid_x,
                           player.grid_y);
            rendering_draw_npcs(current_room->npcs, current_room->npc_count);
            rendering_draw_player(&player);
            rendering_draw_quest(renderer);

            rendering_ui_draw_hud(&pets);
            dialogue_render(renderer);
            display_present();
            SDL_Delay(FRAME_DELAY);
            continue;
        }

        // ------------------------------------------
        // RESET BUTTON CHECK (hardware button on target)
        // ------------------------------------------
        if (input_is_reset_pressed(&reset_was_pressed))
        {
            printf("Reset button pressed!\n");
            rendering_ui_reset_catches(&pets);
        }

        // ------------------------------------------
        // PET CATCHING (SPACE on host, button on target)
        // ------------------------------------------
        if (input_is_catch_pressed(&space_was_pressed))
        {
            Pet *p = pet_check_adjacent(&pets,
                                        player.grid_x,
                                        player.grid_y,
                                        current_room->id);

            if (p != NULL)
            {
                audio_play_sound(SOUND_CATCH);
                rendering_ui_increment_catch(p->type);
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
        if (!player.is_moving && !player.just_teleported)
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

                // Prevent infinite re-triggering
                player.just_teleported = true;

                current_room = map_get_current_room(&game_map);
                music_change_room(current_room->music_path);
            }
        }

        // ------------------------------------------
        // RESET TELEPORT LOCK when leaving the door
        // ------------------------------------------
        if (player.just_teleported)
        {
            // If not standing on any door anymore, unlock teleport
            if (map_check_door_collision(&game_map,
                                         player_get_grid_x(&player),
                                         player_get_grid_y(&player)) == NULL)
            {
                player.just_teleported = false;
            }
        }

        // ------------------------------------------
        // NORMAL FRAME RENDERING
        // ------------------------------------------
        display_clear(0, 0, 0);

        map_render_background(&game_map, renderer);

        rendering_draw_doors(current_room->doors, current_room->door_count);
        pet_render_all(&pets, renderer,
                       current_room->id,
                       player.grid_x,
                       player.grid_y);
        rendering_draw_npcs(current_room->npcs, current_room->npc_count);
        rendering_draw_player(&player);
        rendering_draw_quest(renderer);

        // Draw UI HUD (inventory and reset button)
        rendering_ui_draw_hud(&pets);

        display_present();
        SDL_Delay(FRAME_DELAY);

    } // END OF WHILE (running)

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