#include "map.h"
#include "collision.h"
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>

// Debug grid (optional)
void map_render_debug_grid(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);

    // Vertical lines
    for (int x = 0; x <= GRID_WIDTH; x++) {
        int px = x * TILE_SIZE;
        SDL_RenderDrawLine(renderer, px, 0, px, WINDOW_HEIGHT);
    }

    // Horizontal lines
    for (int y = 0; y <= GRID_HEIGHT; y++) {
        int py = y * TILE_SIZE;
        SDL_RenderDrawLine(renderer, 0, py, WINDOW_WIDTH, py);
    }
}

// ----------------------------------------------------
// INITIALIZE OBSTACLES PER ROOM
// ----------------------------------------------------
static void init_room_obstacles(int obstacles[GRID_HEIGHT][GRID_WIDTH], RoomID room_id)
{
    // Clear everything
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            obstacles[y][x] = 0;

    // Add border walls
    for (int x = 0; x < GRID_WIDTH; x++) {
        obstacles[0][x] = 1;
        obstacles[GRID_HEIGHT - 1][x] = 1;
    }
    for (int y = 0; y < GRID_HEIGHT; y++) {
        obstacles[y][0] = 1;
        obstacles[y][GRID_WIDTH - 1] = 1;
    }

    switch (room_id)
    {
        case ROOM_BASEMENT:
            // Pillars
            obstacles[5][10] = 1; obstacles[5][11] = 1;
            obstacles[10][10] = 1; obstacles[10][11] = 1;

            obstacles[5][20] = 1; obstacles[5][21] = 1;
            obstacles[10][20] = 1; obstacles[10][21] = 1;

            // Blocking stairs (3 sides)
            obstacles[1][15] = 1;
            obstacles[2][14] = 1;
            obstacles[2][16] = 1;
            break;

        case ROOM_MAIN:
            // Pillar cluster
            obstacles[5][6] = 1;
            obstacles[5][7] = 1;
            obstacles[6][6] = 1;
            obstacles[6][7] = 1;

            // Stairs down (block sides)
            obstacles[12][9] = 1;
            obstacles[12][11] = 1;
            obstacles[13][10] = 1;

            // Classroom doorway
            obstacles[9][28] = 1;
            obstacles[11][28] = 1;
            obstacles[10][29] = 1;
            break;

        case ROOM_CLASSROOM:
            // Desk rows
            for (int row = 3; row < 12; row += 3) {
                for (int col = 5; col < 25; col += 4) {
                    obstacles[row][col] = 1;
                    obstacles[row][col + 1] = 1;
                }
            }

            // Door blocking (3 sides)
            obstacles[9][2] = 1;
            obstacles[11][2] = 1;
            obstacles[10][1] = 1;
            break;

        default:
            break;
    }
}

// ----------------------------------------------------
// MAP INITIALIZATION
// ----------------------------------------------------
void map_init(Map *map, SDL_Renderer *renderer)
{
    map->current_room_id = ROOM_MAIN;

    // ==========================
    // BASEMENT ROOM
    // ==========================
    Room *basement = &map->rooms[ROOM_BASEMENT];
    basement->id = ROOM_BASEMENT;
    strcpy(basement->name, "Basement");
    strcpy(basement->music_path, "assets/music/basement.ogg");
    init_room_obstacles(basement->obstacles, ROOM_BASEMENT);

    basement->door_count = 1;
    basement->doors[0].x = 15;
    basement->doors[0].y = 2;
    basement->doors[0].type = DOOR_TYPE_STAIRS_UP;
    basement->doors[0].target_room = ROOM_MAIN;
    basement->doors[0].spawn_x = 10;
    basement->doors[0].spawn_y = 11;

    basement->npc_count = 1;
    basement->npcs[0].x = 8;
    basement->npcs[0].y = 8;
    strcpy(basement->npcs[0].name, "Basement Janitor");
    basement->npcs[0].caught = false;
    sprite_load(&basement->npcs[0].sprite, renderer,
                "assets/sprites/npc/Janitor.png");

    // ==========================
    // MAIN HALL ROOM
    // ==========================
    Room *main_room = &map->rooms[ROOM_MAIN];
    main_room->id = ROOM_MAIN;
    strcpy(main_room->name, "Main Hall");
    strcpy(main_room->music_path, "assets/music/main_hall.ogg");
    init_room_obstacles(main_room->obstacles, ROOM_MAIN);

    main_room->door_count = 2;

    // Stairs down → Basement
    main_room->doors[0].x = 10;
    main_room->doors[0].y = 12;
    main_room->doors[0].type = DOOR_TYPE_STAIRS_DOWN;
    main_room->doors[0].target_room = ROOM_BASEMENT;
    main_room->doors[0].spawn_x = 15;
    main_room->doors[0].spawn_y = 3;

    // Door → Classroom
    main_room->doors[1].x = 28;
    main_room->doors[1].y = 10;
    main_room->doors[1].type = DOOR_TYPE_DOOR;
    main_room->doors[1].target_room = ROOM_CLASSROOM;
    main_room->doors[1].spawn_x = 3;
    main_room->doors[1].spawn_y = 10;

    main_room->npc_count = 2;

    // Professor Matthew
    main_room->npcs[0].x = 15;
    main_room->npcs[0].y = 7;
    strcpy(main_room->npcs[0].name, "Professor Matthew");
    main_room->npcs[0].caught = false;
    sprite_load(&main_room->npcs[0].sprite, renderer,
                "assets/sprites/npc/Matthew.png");

    // TA Navid
    main_room->npcs[1].x = 12;
    main_room->npcs[1].y = 10;
    strcpy(main_room->npcs[1].name, "TA Navid");
    main_room->npcs[1].caught = false;
    sprite_load(&main_room->npcs[1].sprite, renderer,
                "assets/sprites/npc/Navid.png");

    // ==========================
    // CLASSROOM ROOM
    // ==========================
    Room *classroom = &map->rooms[ROOM_CLASSROOM];
    classroom->id = ROOM_CLASSROOM;
    strcpy(classroom->name, "Classroom");
    strcpy(classroom->music_path, "assets/music/classroom.ogg");
    init_room_obstacles(classroom->obstacles, ROOM_CLASSROOM);

    classroom->door_count = 1;

    // Door back to Main Hall
    classroom->doors[0].x = 2;
    classroom->doors[0].y = 10;
    classroom->doors[0].type = DOOR_TYPE_DOOR;
    classroom->doors[0].target_room = ROOM_MAIN;
    classroom->doors[0].spawn_x = 27;
    classroom->doors[0].spawn_y = 10;

    classroom->npc_count = 1;
    classroom->npcs[0].x = 15;
    classroom->npcs[0].y = 10;
    strcpy(classroom->npcs[0].name, "TA Soroush");
    classroom->npcs[0].caught = false;
    sprite_load(&classroom->npcs[0].sprite, renderer,
                "assets/sprites/npc/Soroush.png");

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

    for (int i = 0; i < room->door_count; i++) {
        Door *d = &room->doors[i];
        if (d->x == px && d->y == py)
            return d;
    }
    return NULL;
}

// ----------------------------------------------------

void map_transition_room(Map *map, RoomID new_room,
                         int *player_x, int *player_y,
                         Door *door)
{
    printf("Transitioning from %s → %s\n",
           map->rooms[map->current_room_id].name,
           map->rooms[new_room].name);

    map->current_room_id = new_room;

    *player_x = door->spawn_x;
    *player_y = door->spawn_y;

    printf("Spawned at (%d, %d)\n", *player_x, *player_y);
}

// ----------------------------------------------------

void map_cleanup(Map *map)
{
    for (int r = 0; r < ROOM_COUNT; r++) {
        Room *room = &map->rooms[r];

        for (int i = 0; i < room->npc_count; i++)
            sprite_free(&room->npcs[i].sprite);
    }

    IMG_Quit();
    printf("Map cleanup complete\n");
}
