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

void game_run(void)
{
    SDL_Renderer *renderer = display_get_renderer();

    dialogue_init(renderer);

    if (!input_initialize())
        fprintf(stderr, "Warning: Failed to initialize input\n");
    if (!audio_init())
        fprintf(stderr, "Warning: Failed to initialize audio\n");

    audio_load_sound("assets/sounds/catch.wav", SOUND_CATCH);

    Map game_map;
    map_init(&game_map, renderer);

    Player player;
    player_init(&player, 20, 11, renderer);

    bool music_has_started = false;

    PetManager pets;
    pet_manager_init(&pets, renderer, 3, 3, 2, 2);
    pet_spawn_initial(&pets, renderer, &game_map);

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

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_KEYDOWN)
            {
                SDL_Keycode key = event.key.keysym.sym;

                if (dialogue_is_active())
                {
                    dialogue_handle_key(key);
                    continue;
                }

                if (key == SDLK_ESCAPE)
                    running = false;

                //-------------------------------------------
                // TALK TO NPC (T key)
                //-------------------------------------------
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
                                "Quest Started: Catch 5 Bears"
                            );

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
                                "Quest Started: Catch 3 Raccoons"
                            );

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
                                "Quest Started: Catch 4 Small Deer"
                            );

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
                                "Quest Started: Catch 2 Big Deer"
                            );

                            quest_start(q, 2, "Catch 2 Big Deer and report back to Morteza");
                            break;
                        }
                    }
                }
            }
        }

        Room *current_room = map_get_current_room(&game_map);

        if (music_has_started)
            music_update(current_room,
                         player.grid_x,
                         player.grid_y);

        //----------------------------------------
        // DIALOGUE FREEZE MODE
        //----------------------------------------
        if (dialogue_is_active())
        {
            dialogue_update_typewriter();

            display_clear(0,0,0);
            map_render_background(&game_map, renderer);

            rendering_draw_doors(current_room->doors, current_room->door_count);
            pet_render_all(&pets, renderer,
                           current_room->id,
                           player.grid_x,
                           player.grid_y);
            rendering_draw_npcs(current_room->npcs, current_room->npc_count);
            rendering_draw_player(&player);
            rendering_draw_quest(renderer);

            dialogue_render(renderer);
            display_present();
            SDL_Delay(FRAME_DELAY);
            continue;
        }

        //----------------------------------------
        // CATCHING PETS
        //----------------------------------------
        if (input_is_catch_pressed(&space_was_pressed))
        {
            Pet *p = pet_check_adjacent(&pets,
                                        player.grid_x,
                                        player.grid_y,
                                        current_room->id);

            if (p != NULL)
            {
                audio_play_sound(SOUND_CATCH);
                pet_catch(&pets, p);
                pet_check_respawn(&pets, renderer, &game_map);
            }
        }

        //----------------------------------------
        // MOVEMENT
        //----------------------------------------
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

        //----------------------------------------
        // ROOM TRANSITION
        //----------------------------------------
        if (!player.is_moving)
        {
            Door *door = map_check_door_collision(&game_map,
                                                  player.grid_x,
                                                  player.grid_y);

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
            }
        }

        //----------------------------------------
        // RENDER FRAME
        //----------------------------------------
        display_clear(0,0,0);
        map_render_background(&game_map, renderer);

        rendering_draw_doors(current_room->doors, current_room->door_count);
        pet_render_all(&pets, renderer,
                       current_room->id,
                       player.grid_x,
                       player.grid_y);
        rendering_draw_npcs(current_room->npcs, current_room->npc_count);
        rendering_draw_player(&player);
        rendering_draw_quest(renderer);

        display_present();
        SDL_Delay(FRAME_DELAY);
    }

    //----------------------------------------
    // CLEANUP
    //----------------------------------------
    pet_manager_cleanup(&pets);
    dialogue_cleanup();
    input_cleanup();
    music_cleanup();
    audio_cleanup();
    map_cleanup(&game_map);
    player_cleanup(&player);
}
