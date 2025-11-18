#include "dungeon.h"
#include <stdlib.h>
#include <stdio.h>

void create_room(Game *game, Room room) {
    for (int y = room.y; y < room.y + room.height; y++) {
        for (int x = room.x; x < room.x + room.width; x++) {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                game->map[y][x] = TILE_FLOOR;
            }
        }
    }
}

void create_corridor(Game *game, int x1, int y1, int x2, int y2) {
    // Horizontal corridor
    int start_x = x1 < x2 ? x1 : x2;
    int end_x = x1 < x2 ? x2 : x1;
    for (int x = start_x; x <= end_x; x++) {
        if (x >= 0 && x < MAP_WIDTH && y1 >= 0 && y1 < MAP_HEIGHT) {
            game->map[y1][x] = TILE_FLOOR;
        }
    }
    
    // Vertical corridor
    int start_y = y1 < y2 ? y1 : y2;
    int end_y = y1 < y2 ? y2 : y1;
    for (int y = start_y; y <= end_y; y++) {
        if (x2 >= 0 && x2 < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
            game->map[y][x2] = TILE_FLOOR;
        }
    }
}

RoomType choose_room_type(int room_index, int total_rooms) {
    if (room_index == 0) return ROOM_NORMAL;
    if (room_index == total_rooms - 1) return ROOM_BOSS;
    
    int roll = rand() % 100;
    if (roll < 15) return ROOM_TREASURE;
    else if (roll < 35) return ROOM_MONSTER_DEN;
    else if (roll < 50) return ROOM_SAFE;
    else return ROOM_NORMAL;
}

// GUARANTEED dungeon generation - creates a simple linear dungeon
void create_guaranteed_dungeon(Game *game) {
    game->room_count = 0;
    
    // Create rooms in a line across the map
    int rooms_to_create = MAX_ROOMS < 8 ? MAX_ROOMS : 8;
    int spacing = (MAP_WIDTH - 10) / (rooms_to_create + 1);
    
    for (int i = 0; i < rooms_to_create; i++) {
        int x = 5 + spacing * (i + 1) - 4;
        int y = MAP_HEIGHT / 2 - 3;
        int width = 7;
        int height = 5;
        
        // Keep rooms within bounds
        if (x < 1) x = 1;
        if (y < 1) y = 1;
        if (x + width >= MAP_WIDTH - 1) x = MAP_WIDTH - width - 2;
        if (y + height >= MAP_HEIGHT - 1) y = MAP_HEIGHT - height - 2;
        
        Room new_room = {x, y, width, height, false, ROOM_NORMAL};
        new_room.room_type = choose_room_type(i, rooms_to_create);
        
        if (new_room.room_type == ROOM_BOSS) {
            new_room.is_boss_room = true;
            new_room.width = 8;
            new_room.height = 6;
        }
        
        create_room(game, new_room);
        
        // Connect to previous room
        if (i > 0) {
            Room prev = game->rooms[game->room_count - 1];
            create_corridor(game, 
                prev.x + prev.width / 2, prev.y + prev.height / 2,
                new_room.x + new_room.width / 2, new_room.y + new_room.height / 2);
        }
        
        game->rooms[game->room_count] = new_room;
        game->room_count++;
    }
}

// Count floor tiles to verify dungeon generated
int count_floor_tiles(Game *game) {
    int count = 0;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (game->map[y][x] == TILE_FLOOR || game->map[y][x] == TILE_STAIRS) {
                count++;
            }
        }
    }
    return count;
}

void generate_dungeon(Game *game) {
    // Initialize everything
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            game->map[y][x] = TILE_WALL;
            game->visible[y][x] = false;
            game->explored[y][x] = false;
        }
    }
    
    game->room_count = 0;
    
    // Try random generation multiple times
    int generation_attempt = 0;
    const int MAX_ATTEMPTS = 100;
    
    while (generation_attempt < MAX_ATTEMPTS && game->room_count < 3) {
        generation_attempt++;
        game->room_count = 0;
        
        // Clear map for fresh attempt
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                game->map[y][x] = TILE_WALL;
            }
        }
        
        // Try to place rooms with relaxed constraints
        int room_attempts = 0;
        int rooms_placed = 0;
        
        while (rooms_placed < MAX_ROOMS && room_attempts < MAX_ROOMS * 10) {
            room_attempts++;
            
            int width = 5 + rand() % 7;
            int height = 4 + rand() % 6;
            int x = 2 + rand() % (MAP_WIDTH - width - 4);
            int y = 2 + rand() % (MAP_HEIGHT - height - 4);
            
            Room new_room = {x, y, width, height, false, ROOM_NORMAL};
            new_room.room_type = choose_room_type(rooms_placed, MAX_ROOMS);
            
            if (new_room.room_type == ROOM_BOSS) {
                new_room.is_boss_room = true;
            }
            
            // Check overlap (allow touching)
            bool overlap = false;
            for (int j = 0; j < game->room_count; j++) {
                Room existing = game->rooms[j];
                if (x < existing.x + existing.width &&
                    x + width > existing.x &&
                    y < existing.y + existing.height &&
                    y + height > existing.y) {
                    overlap = true;
                    break;
                }
            }
            
            if (!overlap) {
                create_room(game, new_room);
                
                if (game->room_count > 0) {
                    Room prev = game->rooms[game->room_count - 1];
                    create_corridor(game, 
                        prev.x + prev.width / 2, prev.y + prev.height / 2,
                        new_room.x + new_room.width / 2, new_room.y + new_room.height / 2);
                }
                
                game->rooms[game->room_count] = new_room;
                game->room_count++;
                rooms_placed++;
            }
        }
    }
    
    // CRITICAL FAILSAFE: If generation failed, force create guaranteed dungeon
    int floor_count = count_floor_tiles(game);
    if (game->room_count < 3 || floor_count < 50) {
        // Clear map completely
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                game->map[y][x] = TILE_WALL;
            }
        }
        
        game->room_count = 0;
        
        // Create guaranteed linear dungeon
        create_guaranteed_dungeon(game);
    }
    
    // VERIFY: Ensure we have rooms
    if (game->room_count == 0) {
        // Ultimate fallback - create single large room
        Room emergency_room = {MAP_WIDTH / 2 - 10, MAP_HEIGHT / 2 - 5, 20, 10, false, ROOM_NORMAL};
        create_room(game, emergency_room);
        game->rooms[0] = emergency_room;
        game->room_count = 1;
    }
    
    // Spawn player in first room
    if (game->room_count > 0) {
        Room *first_room = &game->rooms[0];
        int center_x = first_room->x + first_room->width / 2;
        int center_y = first_room->y + first_room->height / 2;
        
        game->player.pos.x = center_x;
        game->player.pos.y = center_y;
        
        // Force position to be floor
        if (center_x >= 0 && center_x < MAP_WIDTH && 
            center_y >= 0 && center_y < MAP_HEIGHT) {
            game->map[center_y][center_x] = TILE_FLOOR;
        }
    }
}

void spawn_stairs(Game *game) {
    if (game->room_count < 2) {
        // If only 1 room, put stairs in center
        if (game->room_count == 1) {
            Room room = game->rooms[0];
            game->stairs_pos.x = room.x + room.width / 2 + 2;
            game->stairs_pos.y = room.y + room.height / 2;
            if (game->stairs_pos.x >= 0 && game->stairs_pos.x < MAP_WIDTH && 
                game->stairs_pos.y >= 0 && game->stairs_pos.y < MAP_HEIGHT) {
                game->map[game->stairs_pos.y][game->stairs_pos.x] = TILE_STAIRS;
            }
        }
        return;
    }
    
    // Place stairs in a middle room (not first or last)
    int room_idx = game->room_count / 2;
    if (room_idx == 0 && game->room_count > 1) room_idx = 1;
    if (room_idx >= game->room_count) room_idx = game->room_count - 2;
    
    Room room = game->rooms[room_idx];
    
    int stairs_x = room.x + room.width / 2;
    int stairs_y = room.y + room.height / 2;
    
    if (stairs_x >= 0 && stairs_x < MAP_WIDTH && 
        stairs_y >= 0 && stairs_y < MAP_HEIGHT) {
        game->stairs_pos.x = stairs_x;
        game->stairs_pos.y = stairs_y;
        game->map[stairs_y][stairs_x] = TILE_STAIRS;
    }
}
