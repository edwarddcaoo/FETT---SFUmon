#include "map.h"
#include "collision.h"
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>

// Helper to load room background
static SDL_Texture *load_room_texture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        fprintf(stderr, "Failed to load image %s: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        fprintf(stderr, "Failed to create texture from %s: %s\n", path, SDL_GetError());
        return NULL;
    }

    printf("Loaded background: %s\n", path);
    return texture;
}

void map_render_debug_grid(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // Red, semi-transparent

    // Draw vertical lines
    for (int x = 0; x <= GRID_WIDTH; x++)
    {
        int pixel_x = x * TILE_SIZE;
        SDL_RenderDrawLine(renderer, pixel_x, 0, pixel_x, WINDOW_HEIGHT);
    }

    // Draw horizontal lines
    for (int y = 0; y <= GRID_HEIGHT; y++)
    {
        int pixel_y = y * TILE_SIZE;
        SDL_RenderDrawLine(renderer, 0, pixel_y, WINDOW_WIDTH, pixel_y);
    }
}

// Helper function to initialize room obstacles (empty for now)
static void init_room_obstacles(int obstacles[GRID_HEIGHT][GRID_WIDTH], RoomID room_id)
{
    (void)room_id; // adding to supress unused paramter warning
    // Clear all obstacles - we'll add collisions later
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            obstacles[y][x] = 0;
        }
    }

    // Room-specific obstacles
    switch (room_id)
    {
    case ROOM_ASB:
        // Tables
        // Table 1
        for (int x = 9; x <= 11; x++)
        {
            for (int y = 6; y <= 7; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        // Table 2
        for (int x = 17; x <= 19; x++)
        {
            for (int y = 3; y <= 4; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        // Table 3
        for (int x = 17; x <= 19; x++)
        {
            for (int y = 11; y <= 12; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        // Sign
        for (int x = 14; x <= 15; x++)
        {
            for (int y = 16; y <= 18; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        // White table and couch area
        for (int x = 13; x <= 16; x++)
        {
            for (int y = 4; y <= 6; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        // Middle couch
        for (int x = 13; x <= 15; x++)
        {
            for (int y = 8; y <= 10; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        // Bottom seating
        obstacles[12][13] = 1;

        for (int x = 12; x <= 15; x++)
        {
            obstacles[13][x] = 1;
        }

        for (int x = 13; x <= 16; x++)
        {
            obstacles[14][x] = 1;
        }
        break;

    case ROOM_CLASSROOM:
        // Wall
        for (int x = 0; x <= 29; x++)
        {
            for (int y = 0; y <= 6; y++)
            {
                obstacles[y][x] = 1;
            }
        }
        for (int x = 0; x <= 6; x++)
        {
            obstacles[7][x] = 1;
        }
        for (int x = 8; x <= 29; x++)
        {
            obstacles[7][x] = 1;
        }

        // U table
        for (int x = 9; x <= 11; x++)
        {
            for (int y = 10; y <= 18; y++)
            {
                obstacles[y][x] = 1;
            }
        }
        for (int x = 12; x <= 15; x++)
        {
            for (int y = 15; y <= 18; y++)
            {
                obstacles[y][x] = 1;
            }
        }
        for (int x = 16; x <= 18; x++)
        {
            for (int y = 10; y <= 18; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        // Chairs
        for (int y = 12; y <= 13; y++)
        {
            obstacles[y][7] = 1;
        }

        for (int y = 16; y <= 18; y++)
        {
            obstacles[y][7] = 1;
        }
        for (int x = 10; x <= 11; x++)
        {
            obstacles[19][x] = 1;
        }

        // Side table
        for (int x = 21; x <= 23; x++)
        {
            for (int y = 14; y <= 18; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        // Random chair
        for (int y = 15; y <= 16; y++)
        {
            obstacles[y][19] = 1;
        }

        obstacles[11][19] = 1;

        for (int x = 21; x <= 22; x++)
        {
            for (int y = 12; y <= 13; y++)
            {
                obstacles[y][x] = 1;
            }
        }

        break;

    case ROOM_PITLAB:
        // Top tables
        for (int x = 7; x <= 9; x++)
        {
            obstacles[1][x] = 1;
        }
        for (int x = 12; x <= 17; x++)
        {
            obstacles[1][x] = 1;
        }
        for (int x = 21; x <= 23; x++)
        {
            obstacles[1][x] = 1;
        }

        // Left column table
        for (int y = 2; y <= 18; y++)
        {
            obstacles[y][7] = 1;
        }

        // Middle column
        for (int x = 14; x <= 15; x++)
        {
            for (int y = 2; y <= 18; y++)
            {
                obstacles[y][x] = 1;
            }
        }
        // Right column
        for (int y = 2; y <= 18; y++)
        {
            obstacles[y][23] = 1;
        }

        // Random chairs
        obstacles[3][8] = 1;
        obstacles[4][8] = 1;
        obstacles[6][8] = 1;
        obstacles[7][8] = 1;
        obstacles[12][8] = 1;
        obstacles[13][8] = 1;

        obstacles[3][13] = 1;
        obstacles[4][13] = 1;
        obstacles[6][13] = 1;
        obstacles[7][13] = 1;
        obstacles[12][13] = 1;
        obstacles[13][13] = 1;

        obstacles[3][17] = 1;
        obstacles[4][17] = 1;
        obstacles[6][17] = 1;
        obstacles[7][17] = 1;
        obstacles[12][17] = 1;
        obstacles[13][17] = 1;

        obstacles[3][21] = 1;
        obstacles[4][21] = 1;
        obstacles[6][21] = 1;
        obstacles[7][21] = 1;
        obstacles[12][21] = 1;
        obstacles[13][21] = 1;

        break;

    default:
        break;
    }
}

void map_init(Map *map, SDL_Renderer *renderer)
{
    // Initialize SDL_image
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags))
    {
        fprintf(stderr, "SDL_image init failed: %s\n", IMG_GetError());
    }

    // Start in ASB
    map->current_room_id = ROOM_ASB;

    // ===== ASB ROOM =====
    Room *asb = &map->rooms[ROOM_ASB];
    asb->id = ROOM_ASB;
    strcpy(asb->name, "ASB");
    strcpy(asb->music_path, "assets/music/main_hall.ogg");
    asb->background_texture = load_room_texture(renderer, "assets/sprites/maps/asb1.png");
    init_room_obstacles(asb->obstacles, ROOM_ASB);

    // Two doors: stairs at top -> pitlab, door at bottom right -> classroom
    asb->door_count = 2;

    // Stairs at top center (approximate position - adjust based on your image)
    asb->doors[0].x = 15; // Top center
    asb->doors[0].y = 2;
    asb->doors[0].type = DOOR_TYPE_STAIRS_UP;
    asb->doors[0].target_room = ROOM_PITLAB;
    asb->doors[0].spawn_x = 29;
    asb->doors[0].spawn_y = 18; // Spawn at bottom of pitlab

    // Door at bottom right
    asb->doors[1].x = 27; // Bottom right
    asb->doors[1].y = 17;
    asb->doors[1].type = DOOR_TYPE_DOOR;
    asb->doors[1].target_room = ROOM_CLASSROOM;
    asb->doors[1].spawn_x = 7;
    asb->doors[1].spawn_y = 7; // Spawn at top left side of classroom (door)

    // ASB NPCs (adjust positions based on your map)
    asb->npc_count = 1;
    asb->npcs[0].x = 10; // Change position as needed
    asb->npcs[0].y = 10;
    asb->npcs[0].caught = false;
    strcpy(asb->npcs[0].name, "TA Soroush");
    sprite_load(&asb->npcs[0].sprite, renderer, "assets/sprites/npc/Soroush.png");

    asb->npcs[1].x = 20;
    asb->npcs[1].y = 10;
    asb->npcs[1].caught = false;
    strcpy(asb->npcs[1].name, "TA Navid");
    sprite_load(&asb->npcs[1].sprite, renderer, "assets/sprites/npc/Navid.png");

    // ===== CLASSROOM ROOM =====
    Room *classroom = &map->rooms[ROOM_CLASSROOM];
    classroom->id = ROOM_CLASSROOM;
    strcpy(classroom->name, "Classroom");
    strcpy(classroom->music_path, "assets/music/classroom.ogg");
    classroom->background_texture = load_room_texture(renderer, "assets/sprites/maps/classroom1.png");
    init_room_obstacles(classroom->obstacles, ROOM_CLASSROOM);

    // Door back to ASB (left side)
    classroom->door_count = 1;
    classroom->doors[0].x = 7;
    classroom->doors[0].y = 7;
    classroom->doors[0].type = DOOR_TYPE_DOOR;
    classroom->doors[0].target_room = ROOM_ASB;
    classroom->doors[0].spawn_x = 26; // Spawn back at right side of ASB
    classroom->doors[0].spawn_y = 17;

    // Classroom NPCs
    classroom->npc_count = 1;
    classroom->npcs[0].x = 15;
    classroom->npcs[0].y = 10;
    classroom->npcs[0].caught = false;
    strcpy(classroom->npcs[0].name, "TA Soroush");
    sprite_load(&classroom->npcs[0].sprite, renderer, "assets/sprites/npc/Navid.png");

    // ===== PITLAB ROOM =====
    Room *pitlab = &map->rooms[ROOM_PITLAB];
    pitlab->id = ROOM_PITLAB;
    strcpy(pitlab->name, "PIT Lab");
    strcpy(pitlab->music_path, "assets/music/basement.ogg");
    pitlab->background_texture = load_room_texture(renderer, "assets/sprites/maps/pitlab1.png");
    init_room_obstacles(pitlab->obstacles, ROOM_PITLAB);

    // Stairs back down to ASB (bottom center)
    pitlab->door_count = 1;
    pitlab->doors[0].x = 29;
    pitlab->doors[0].y = 18;
    pitlab->doors[0].type = DOOR_TYPE_STAIRS_DOWN;
    pitlab->doors[0].target_room = ROOM_ASB;
    pitlab->doors[0].spawn_x = 15;
    pitlab->doors[0].spawn_y = 3; // Spawn at top of ASB

    // PIT Lab NPCs
    pitlab->npc_count = 1;
    pitlab->npcs[0].x = 11; // Change position as needed
    pitlab->npcs[0].y = 5;
    pitlab->npcs[0].caught = false;
    strcpy(pitlab->npcs[0].name, "Professor Matthew");
    sprite_load(&pitlab->npcs[0].sprite, renderer, "assets/sprites/npc/Matthew.png");
    printf("Map initialized with %d rooms\n", ROOM_COUNT);
}

Room *map_get_current_room(Map *map)
{
    return &map->rooms[map->current_room_id];
}

void map_render_background(Map *map, SDL_Renderer *renderer)
{
    Room *current_room = map_get_current_room(map);

    if (current_room->background_texture)
    {
        // Render background stretched to fill the logical window
        SDL_Rect dest = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, current_room->background_texture, NULL, &dest);
    }
    else
    {
        // Fallback: solid color background
        SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND_R, COLOR_BACKGROUND_G, COLOR_BACKGROUND_B, 255);
        SDL_RenderClear(renderer);
    }
}

Door *map_check_door_collision(Map *map, int player_x, int player_y)
{
    Room *current_room = map_get_current_room(map);

    for (int i = 0; i < current_room->door_count; i++)
    {
        Door *door = &current_room->doors[i];
        if (door->x == player_x && door->y == player_y)
        {
            return door;
        }
    }

    return NULL;
}

void map_transition_room(Map *map, RoomID new_room, int *player_x, int *player_y, Door *door)
{
    printf("Transitioning from %s to %s\n",
           map->rooms[map->current_room_id].name,
           map->rooms[new_room].name);

    map->current_room_id = new_room;
    *player_x = door->spawn_x;
    *player_y = door->spawn_y;

    printf("Entered %s at (%d, %d)\n",
           map->rooms[new_room].name, *player_x, *player_y);
}

void map_cleanup(Map *map)
{
    // Cleanup all NPC sprites and textures in all rooms
    for (int room_idx = 0; room_idx < ROOM_COUNT; room_idx++)
    {
        Room *room = &map->rooms[room_idx];

        // Free background texture
        if (room->background_texture)
        {
            SDL_DestroyTexture(room->background_texture);
            room->background_texture = NULL;
        }

        // Free NPC sprites
        for (int i = 0; i < room->npc_count; i++)
        {
            sprite_free(&room->npcs[i].sprite);
        }
    }

    IMG_Quit();
    printf("Map cleanup complete\n");
}
