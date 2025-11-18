#include "fov.h"
#include <stdlib.h>
#include <math.h>

void clear_fov(Game *game) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            game->visible[y][x] = false;
        }
    }
}

// Bresenham's line algorithm to check line of sight
bool has_line_of_sight(Game *game, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    int x = x0;
    int y = y0;
    
    while (true) {
        // Check if we hit a wall (but not at the end point)
        if (x != x1 || y != y1) {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                if (game->map[y][x] == TILE_WALL) {
                    return false;
                }
            }
        }
        
        // Reached destination
        if (x == x1 && y == y1) {
            return true;
        }
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void calculate_fov(Game *game) {
    clear_fov(game);
    
    int px = game->player.pos.x;
    int py = game->player.pos.y;
    
    // Player's position is always visible
    game->visible[py][px] = true;
    game->explored[py][px] = true;
    
    // Check all tiles within FOV radius
    for (int y = py - FOV_RADIUS; y <= py + FOV_RADIUS; y++) {
        for (int x = px - FOV_RADIUS; x <= px + FOV_RADIUS; x++) {
            // Skip out of bounds
            if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
                continue;
            }
            
            // Calculate distance
            int dx = x - px;
            int dy = y - py;
            float distance = sqrt(dx * dx + dy * dy);
            
            // Skip if outside radius
            if (distance > FOV_RADIUS) {
                continue;
            }
            
            // Check line of sight
            if (has_line_of_sight(game, px, py, x, y)) {
                game->visible[y][x] = true;
                game->explored[y][x] = true;
            }
        }
    }
}

bool is_visible(Game *game, int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }
    return game->visible[y][x];
}

bool is_explored(Game *game, int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }
    return game->explored[y][x];
}