#include "collision.h"
#include "common.h"

void collision_init_obstacles(int obstacles[][GRID_WIDTH]) {
    // Initialize all to 0 (walkable)
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            obstacles[y][x] = 0;
        }
    }
    
    // Create border around map
    for (int x = 0; x < GRID_WIDTH; x++) {
        obstacles[0][x] = 1;
        obstacles[GRID_HEIGHT-1][x] = 1;
    }
    for (int y = 0; y < GRID_HEIGHT; y++) {
        obstacles[y][0] = 1;
        obstacles[y][GRID_WIDTH-1] = 1;
    }
    
    // Add building obstacle
    for (int x = 5; x <= 8; x++) {
        for (int y = 3; y <= 6; y++) {
            obstacles[y][x] = 1;
        }
    }
}