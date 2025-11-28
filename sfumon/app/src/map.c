#include "map.h"
#include "collision.h"
#include <string.h>
#include <stdio.h>

// Helper function to initialize room obstacles
static void init_room_obstacles(int obstacles[GRID_HEIGHT][GRID_WIDTH], RoomID room_id) {
    // Clear all obstacles first
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            obstacles[y][x] = 0;
        }
    }
    
    // Add border walls for all rooms
    for (int x = 0; x < GRID_WIDTH; x++) {
        obstacles[0][x] = 1;
        obstacles[GRID_HEIGHT - 1][x] = 1;
    }
    for (int y = 0; y < GRID_HEIGHT; y++) {
        obstacles[y][0] = 1;
        obstacles[y][GRID_WIDTH - 1] = 1;
    }
    
    // Room-specific obstacles
    switch (room_id) {
        case ROOM_BASEMENT:
            // Basement - some pillars
            obstacles[5][10] = 1;
            obstacles[5][11] = 1;
            obstacles[10][10] = 1;
            obstacles[10][11] = 1;
            
            obstacles[5][20] = 1;
            obstacles[5][21] = 1;
            obstacles[10][20] = 1;
            obstacles[10][21] = 1;
            
            // Block 3 sides of stairs (at x=15, y=2), leave bottom open
            obstacles[1][15] = 1;  // Top
            obstacles[2][14] = 1;  // Left
            obstacles[2][16] = 1;  // Right
            break;
            
        case ROOM_MAIN:
            // Main floor - your existing obstacle layout
            obstacles[5][6] = 1;
            obstacles[5][7] = 1;
            obstacles[6][6] = 1;
            obstacles[6][7] = 1;
            
            // Block 3 sides of stairs down (at x=10, y=12), leave top open
            obstacles[12][9] = 1;   // Left
            obstacles[12][11] = 1;  // Right
            obstacles[13][10] = 1;  // Bottom
            
            // Block 3 sides of classroom door (at x=28, y=10), leave left open
            obstacles[9][28] = 1;   // Top
            obstacles[11][28] = 1;  // Bottom
            obstacles[10][29] = 1;  // Right
            break;
            
        case ROOM_CLASSROOM:
            // Classroom - desks in rows
            for (int row = 3; row < 12; row += 3) {
                for (int col = 5; col < 25; col += 4) {
                    obstacles[row][col] = 1;
                    obstacles[row][col + 1] = 1;
                }
            }
            
            // Block 3 sides of exit door (at x=2, y=10), leave right open
            obstacles[9][2] = 1;   // Top
            obstacles[11][2] = 1;  // Bottom
            obstacles[10][1] = 1;  // Left
            break;
    }
}

void map_init(Map* map, SDL_Renderer* renderer) {
    // Start in main room
    map->current_room_id = ROOM_MAIN;
    
    // ===== BASEMENT ROOM =====
    Room* basement = &map->rooms[ROOM_BASEMENT];
    basement->id = ROOM_BASEMENT;
    strcpy(basement->name, "Basement");
    strcpy(basement->music_path, "assets/music/basement.ogg");
    init_room_obstacles(basement->obstacles, ROOM_BASEMENT);
    
    // Stairs up to main level
    basement->door_count = 1;
    basement->doors[0].x = 15;
    basement->doors[0].y = 2;
    basement->doors[0].type = DOOR_TYPE_STAIRS_UP;
    basement->doors[0].target_room = ROOM_MAIN;
    basement->doors[0].spawn_x = 10;
    basement->doors[0].spawn_y = 11;  // Changed from 12 to 11 (one tile above door)
    
    // Basement NPCs
    basement->npc_count = 1;
    basement->npcs[0].x = 8;
    basement->npcs[0].y = 8;
    basement->npcs[0].caught = false;
    strcpy(basement->npcs[0].name, "Basement Janitor");
    sprite_load(&basement->npcs[0].sprite, renderer, "assets/sprites/npc/Soroush.png");
    
    // ===== MAIN LEVEL ROOM =====
    Room* main_room = &map->rooms[ROOM_MAIN];
    main_room->id = ROOM_MAIN;
    strcpy(main_room->name, "Main Hall");
    strcpy(main_room->music_path, "assets/music/main_hall.ogg");
    init_room_obstacles(main_room->obstacles, ROOM_MAIN);
    
    // Two doors: stairs down to basement, door to classroom
    main_room->door_count = 2;
    
    // Stairs down to basement
    main_room->doors[0].x = 10;
    main_room->doors[0].y = 12;
    main_room->doors[0].type = DOOR_TYPE_STAIRS_DOWN;
    main_room->doors[0].target_room = ROOM_BASEMENT;
    main_room->doors[0].spawn_x = 15;
    main_room->doors[0].spawn_y = 3;  // Changed from 3 to 3 (one tile below door at y=2)
    
    // Door to classroom
    main_room->doors[1].x = 28;
    main_room->doors[1].y = 10;
    main_room->doors[1].type = DOOR_TYPE_DOOR;
    main_room->doors[1].target_room = ROOM_CLASSROOM;
    main_room->doors[1].spawn_x = 3;  // Changed from 3 to 3 (one tile to right of door at x=2)
    main_room->doors[1].spawn_y = 10;
    
    // Main room NPCs - your existing NPCs
    main_room->npc_count = 2;
    
    main_room->npcs[0].x = 15;
    main_room->npcs[0].y = 7;
    main_room->npcs[0].caught = false;
    strcpy(main_room->npcs[0].name, "Professor Matthew");
    sprite_load(&main_room->npcs[0].sprite, renderer, "assets/sprites/npc/Matthew.png");
    
    main_room->npcs[1].x = 12;
    main_room->npcs[1].y = 10;
    main_room->npcs[1].caught = false;
    strcpy(main_room->npcs[1].name, "TA Navid");
    sprite_load(&main_room->npcs[1].sprite, renderer, "assets/sprites/npc/Navid.png");
    
    // ===== CLASSROOM ROOM =====
    Room* classroom = &map->rooms[ROOM_CLASSROOM];
    classroom->id = ROOM_CLASSROOM;
    strcpy(classroom->name, "Classroom");
    strcpy(classroom->music_path, "assets/music/classroom.ogg");
    init_room_obstacles(classroom->obstacles, ROOM_CLASSROOM);
    
    // Door back to main hall
    classroom->door_count = 1;
    classroom->doors[0].x = 2;
    classroom->doors[0].y = 10;
    classroom->doors[0].type = DOOR_TYPE_DOOR;
    classroom->doors[0].target_room = ROOM_MAIN;
    classroom->doors[0].spawn_x = 27;  // Changed from 27 to 27 (one tile to left of door at x=28)
    classroom->doors[0].spawn_y = 10;
    
    // Classroom NPCs
    classroom->npc_count = 1;
    classroom->npcs[0].x = 15;
    classroom->npcs[0].y = 8;
    classroom->npcs[0].caught = false;
    strcpy(classroom->npcs[0].name, "TA Soroush");
    sprite_load(&classroom->npcs[0].sprite, renderer, "assets/sprites/npc/Navid.png");
    
    printf("Map initialized with %d rooms\n", ROOM_COUNT);
}

Room* map_get_current_room(Map* map) {
    return &map->rooms[map->current_room_id];
}

Door* map_check_door_collision(Map* map, int player_x, int player_y) {
    Room* current_room = map_get_current_room(map);
    
    for (int i = 0; i < current_room->door_count; i++) {
        Door* door = &current_room->doors[i];
        if (door->x == player_x && door->y == player_y) {
            return door;
        }
    }
    
    return NULL;
}

void map_transition_room(Map* map, RoomID new_room, int* player_x, int* player_y, Door* door) {
    printf("Transitioning from %s to room %d\n", 
           map->rooms[map->current_room_id].name, new_room);
    
    map->current_room_id = new_room;
    *player_x = door->spawn_x;
    *player_y = door->spawn_y;
    
    printf("Entered %s at (%d, %d)\n", 
           map->rooms[new_room].name, *player_x, *player_y);
}

void map_cleanup(Map* map) {
    // Cleanup all NPC sprites in all rooms
    for (int room_idx = 0; room_idx < ROOM_COUNT; room_idx++) {
        Room* room = &map->rooms[room_idx];
        for (int i = 0; i < room->npc_count; i++) {
            sprite_free(&room->npcs[i].sprite);
        }
    }
    printf("Map cleanup complete\n");
}