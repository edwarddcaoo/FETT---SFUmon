#ifndef MAP_H
#define MAP_H

#include "common.h"
#include "npc.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_DOORS 4
#define MAX_NPCS_PER_ROOM 10

typedef enum {
    ROOM_BASEMENT = 0,
    ROOM_MAIN = 1,
    ROOM_CLASSROOM = 2,
    ROOM_COUNT = 3
} RoomID;

typedef enum {
    DOOR_TYPE_STAIRS_DOWN,  // Stairs going down (to basement)
    DOOR_TYPE_STAIRS_UP,    // Stairs going up (from basement)
    DOOR_TYPE_DOOR          // Regular door
} DoorType;

typedef struct {
    int x;
    int y;
    DoorType type;
    RoomID target_room;  // Which room this leads to
    int spawn_x;         // Where player spawns in target room
    int spawn_y;
} Door;

typedef struct {
    RoomID id;
    char name[32];
    char music_path[128]; 
    int obstacles[GRID_HEIGHT][GRID_WIDTH];
    Door doors[MAX_DOORS];
    int door_count;
    NPC npcs[MAX_NPCS_PER_ROOM];
    int npc_count;
} Room;

typedef struct {
    Room rooms[ROOM_COUNT];
    RoomID current_room_id;
} Map;

// Initialize the entire map with all rooms
void map_init(Map* map, SDL_Renderer* renderer);

// Get the current active room
Room* map_get_current_room(Map* map);

// Check if player is standing on a door and return it
Door* map_check_door_collision(Map* map, int player_x, int player_y);

// Transition to a new room
void map_transition_room(Map* map, RoomID new_room, int* player_x, int* player_y, Door* door);

// Cleanup all map resources
void map_cleanup(Map* map);

#endif // MAP_H