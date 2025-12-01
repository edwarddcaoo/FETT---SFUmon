#include "map.h"
#include "collision.h"
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>

// ----------------------------------------------------
// Load background PNG
// ----------------------------------------------------
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
        fprintf(stderr, "Failed to create texture from %s: %s\n", path, SDL_GetError());

    printf("Loaded background: %s\n", path);
    return texture;
}

// ----------------------------------------------------
// Debug overlay grid
// ----------------------------------------------------
void map_render_debug_grid(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);

    for (int x = 0; x <= GRID_WIDTH; x++)
        SDL_RenderDrawLine(renderer, x * TILE_SIZE, 0,
                           x * TILE_SIZE, WINDOW_HEIGHT);

    for (int y = 0; y <= GRID_HEIGHT; y++)
        SDL_RenderDrawLine(renderer, 0, y * TILE_SIZE,
                           WINDOW_WIDTH, y * TILE_SIZE);
}

// ----------------------------------------------------
// Obstacles
// ----------------------------------------------------
static void init_room_obstacles(int obstacles[GRID_HEIGHT][GRID_WIDTH], RoomID room_id)
{
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            obstacles[y][x] = 0;

    switch (room_id)
    {
    // ------------------------------------------------
    // ASB
    // ------------------------------------------------
    case ROOM_ASB:

        // Table 1
        for (int x = 9; x <= 11; x++)
            for (int y = 6; y <= 7; y++)
                obstacles[y][x] = 1;

        // Table 2
        for (int x = 17; x <= 19; x++)
            for (int y = 3; y <= 4; y++)
                obstacles[y][x] = 1;

        // Table 3
        for (int x = 17; x <= 19; x++)
            for (int y = 11; y <= 12; y++)
                obstacles[y][x] = 1;

        // Sign
        for (int x = 14; x <= 15; x++)
            for (int y = 16; y <= 18; y++)
                obstacles[y][x] = 1;

        // White couch
        for (int x = 13; x <= 16; x++)
            for (int y = 4; y <= 6; y++)
                obstacles[y][x] = 1;

        // Middle couch
        for (int x = 13; x <= 15; x++)
            for (int y = 8; y <= 10; y++)
                obstacles[y][x] = 1;

        // Bottom seating
        obstacles[12][13] = 1;
        for (int x = 12; x <= 15; x++)
            obstacles[13][x] = 1;
        for (int x = 13; x <= 16; x++)
            obstacles[14][x] = 1;

        break;

    // ------------------------------------------------
    // CLASSROOM
    // ------------------------------------------------
    case ROOM_CLASSROOM:

        // Wall
        for (int x = 0; x <= 29; x++)
            for (int y = 0; y <= 6; y++)
                obstacles[y][x] = 1;

        for (int x = 0; x <= 5; x++)
            obstacles[7][x] = 1;
        for (int x = 9; x <= 29; x++)
            obstacles[7][x] = 1;

        // U table
        for (int x = 9; x <= 11; x++)
            for (int y = 10; y <= 18; y++)
                obstacles[y][x] = 1;

        for (int x = 12; x <= 15; x++)
            for (int y = 15; y <= 18; y++)
                obstacles[y][x] = 1;

        for (int x = 16; x <= 18; x++)
            for (int y = 10; y <= 18; y++)
                obstacles[y][x] = 1;

        // Chairs
        for (int y = 12; y <= 13; y++)
            obstacles[y][7] = 1;
        for (int y = 16; y <= 18; y++)
            obstacles[y][7] = 1;

        obstacles[19][10] = 1;
        obstacles[19][11] = 1;

        for (int x = 21; x <= 23; x++)
            for (int y = 14; y <= 18; y++)
                obstacles[y][x] = 1;

        obstacles[15][19] = 1;
        obstacles[16][19] = 1;
        obstacles[11][19] = 1;

        for (int x = 21; x <= 22; x++)
            for (int y = 12; y <= 13; y++)
                obstacles[y][x] = 1;

        break;

    // ------------------------------------------------
    // PIT LAB
    // ------------------------------------------------
    case ROOM_PITLAB:

        for (int x = 7; x <= 9; x++)
            obstacles[1][x] = 1;
        for (int x = 12; x <= 17; x++)
            obstacles[1][x] = 1;
        for (int x = 21; x <= 23; x++)
            obstacles[1][x] = 1;

        for (int y = 2; y <= 18; y++)
            obstacles[y][7] = 1;

        for (int x = 14; x <= 15; x++)
            for (int y = 2; y <= 18; y++)
                obstacles[y][x] = 1;

        for (int y = 2; y <= 18; y++)
            obstacles[y][23] = 1;

        int chairs[][2] = {
            {3, 8}, {4, 8}, {6, 8}, {7, 8}, {12, 8}, {13, 8}, {3, 13}, {4, 13}, {6, 13}, {7, 13}, {12, 13}, {13, 13}, {3, 17}, {4, 17}, {6, 17}, {7, 17}, {12, 17}, {13, 17}, {3, 21}, {4, 21}, {6, 21}, {7, 21}, {12, 21}, {13, 21}};
        for (int i = 0; i < 24; i++)
            obstacles[chairs[i][0]][chairs[i][1]] = 1;

        break;

    default:
        break;
    }
}

// ----------------------------------------------------
// MAP INIT
// ----------------------------------------------------
void map_init(Map *map, SDL_Renderer *renderer)
{
    IMG_Init(IMG_INIT_PNG);

    map->current_room_id = ROOM_ASB;

    // ==============================
    // ASB
    // ==============================
    Room *asb = &map->rooms[ROOM_ASB];
    asb->id = ROOM_ASB;
    strcpy(asb->name, "ASB");
    strcpy(asb->music_path, "assets/music/main_hall.ogg");
    asb->background_texture = load_room_texture(renderer, "assets/sprites/maps/asb1.png");
    init_room_obstacles(asb->obstacles, ROOM_ASB);

    asb->door_count = 2;
    asb->doors[0] = (Door){15, 2, DOOR_TYPE_STAIRS_UP, ROOM_PITLAB, 29, 18};
    asb->doors[1] = (Door){27, 17, DOOR_TYPE_DOOR, ROOM_CLASSROOM, 7, 7};

    asb->npc_count = 2;

    // Soroush
    asb->npcs[0].x = 10;
    asb->npcs[0].y = 10;
    asb->npcs[0].caught = false;
    strcpy(asb->npcs[0].name, "TA Soroush");
    sprite_load(&asb->npcs[0].sprite, renderer, "assets/sprites/npc/Soroush.png");
    strcpy(asb->npcs[0].portrait_path, "assets/dialogue/soroushDialogue.png");

    // ==============================
    // CLASSROOM
    // ==============================
    Room *classroom = &map->rooms[ROOM_CLASSROOM];
    classroom->id = ROOM_CLASSROOM;
    strcpy(classroom->name, "Classroom");
    strcpy(classroom->music_path, "assets/music/classroom.ogg");
    classroom->background_texture = load_room_texture(renderer, "assets/sprites/maps/classroom1.png");
    init_room_obstacles(classroom->obstacles, ROOM_CLASSROOM);

    classroom->door_count = 1;
    classroom->doors[0] = (Door){7, 7, DOOR_TYPE_DOOR, ROOM_ASB, 27, 17};

    classroom->npc_count = 1;

    // Navid
    classroom->npcs[0].x = 15;
    classroom->npcs[0].y = 10;
    classroom->npcs[0].caught = false;
    strcpy(classroom->npcs[0].name, "TA Navid");
    sprite_load(&classroom->npcs[0].sprite, renderer, "assets/sprites/npc/Navid.png");
    strcpy(classroom->npcs[0].portrait_path, "assets/dialogue/navidDialogue.png");

    // ==============================
    // PIT LAB
    // ==============================
    Room *pitlab = &map->rooms[ROOM_PITLAB];
    pitlab->id = ROOM_PITLAB;
    strcpy(pitlab->name, "PIT Lab");
    strcpy(pitlab->music_path, "assets/music/basement.ogg");
    pitlab->background_texture = load_room_texture(renderer, "assets/sprites/maps/pitlab1.png");
    init_room_obstacles(pitlab->obstacles, ROOM_PITLAB);

    pitlab->door_count = 1;
    pitlab->doors[0] = (Door){29, 18, DOOR_TYPE_STAIRS_DOWN, ROOM_ASB, 15, 3};

    pitlab->npc_count = 1;

    pitlab->npcs[0].x = 11;
    pitlab->npcs[0].y = 5;
    pitlab->npcs[0].caught = false;
    strcpy(pitlab->npcs[0].name, "Professor Matthew");
    sprite_load(&pitlab->npcs[0].sprite, renderer, "assets/sprites/npc/Matthew.png");
    strcpy(pitlab->npcs[0].portrait_path, "assets/dialogue/matthewDialogue.png");

    printf("Map initialized with %d rooms\n", ROOM_COUNT);
}

// ----------------------------------------------------
Room *map_get_current_room(Map *map)
{
    return &map->rooms[map->current_room_id];
}

// ----------------------------------------------------
Door *map_check_door_collision(Map *map, int px, int py)
{
    Room *room = map_get_current_room(map);

    for (int i = 0; i < room->door_count; i++)
        if (room->doors[i].x == px && room->doors[i].y == py)
            return &room->doors[i];

    return NULL;
}

// ----------------------------------------------------
void map_transition_room(Map *map, RoomID new_room,
                         int *player_x, int *player_y, Door *door)
{
    printf("Transitioning to %s\n", map->rooms[new_room].name);

    map->current_room_id = new_room;
    *player_x = door->spawn_x;
    *player_y = door->spawn_y;
}

// ----------------------------------------------------
void map_render_background(Map *map, SDL_Renderer *renderer)
{
    Room *room = map_get_current_room(map);

    if (room->background_texture)
    {
        SDL_Rect dest = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, room->background_texture, NULL, &dest);
    }
}

// ----------------------------------------------------
void map_cleanup(Map *map)
{
    for (int r = 0; r < ROOM_COUNT; r++)
    {
        Room *room = &map->rooms[r];

        if (room->background_texture)
        {
            SDL_DestroyTexture(room->background_texture);
            room->background_texture = NULL;
        }

        for (int i = 0; i < room->npc_count; i++)
            sprite_free(&room->npcs[i].sprite);
    }

    IMG_Quit();
}
