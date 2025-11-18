#include "utils.h"
#include <math.h>
#include <string.h>

float distance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

void set_message(Game *game, const char *msg) {
    strncpy(game->message, msg, 99);
    game->message[99] = '\0';
    
    // Add to message log
    // Shift old messages down
    for (int i = 4; i > 0; i--) {
        strncpy(game->message_log[i], game->message_log[i-1], 99);
        game->message_log[i][99] = '\0';
    }
    
    // Add new message at top
    strncpy(game->message_log[0], msg, 99);
    game->message_log[0][99] = '\0';
    
    // Update count (max 5)
    if (game->message_log_count < 5) {
        game->message_log_count++;
    }
}

bool is_walkable(Game *game, int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;
    char tile = game->map[y][x];
    return tile == TILE_FLOOR || tile == TILE_STAIRS;
}

Enemy* get_enemy_at(Game *game, int x, int y) {
    for (int i = 0; i < game->enemy_count; i++) {
        if (game->enemies[i].alive && 
            game->enemies[i].pos.x == x && 
            game->enemies[i].pos.y == y) {
            return &game->enemies[i];
        }
    }
    return NULL;
}

Item* get_item_at(Game *game, int x, int y) {
    for (int i = 0; i < game->item_count; i++) {
        if (!game->items[i].collected && 
            game->items[i].pos.x == x && 
            game->items[i].pos.y == y) {
            return &game->items[i];
        }
    }
    return NULL;
}

bool find_valid_spawn_position(Game *game, Room *room, int *out_x, int *out_y) {
    // Try center first
    int center_x = room->x + room->width / 2;
    int center_y = room->y + room->height / 2;
    
    if (is_walkable(game, center_x, center_y)) {
        *out_x = center_x;
        *out_y = center_y;
        return true;
    }
    
    // Search for any valid floor tile in the room
    for (int y = room->y + 1; y < room->y + room->height - 1; y++) {
        for (int x = room->x + 1; x < room->x + room->width - 1; x++) {
            if (is_walkable(game, x, y)) {
                *out_x = x;
                *out_y = y;
                return true;
            }
        }
    }
    
    // No valid position found
    return false;
}