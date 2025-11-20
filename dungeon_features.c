#include "types.h"
#include "dungeon_features.h"
#include "utils.h"
#include "player.h"
#include "status.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

BiomeType choose_biome(int dungeon_level) {
    // Different biomes appear at different depths
    if (dungeon_level <= 2) {
        return BIOME_DUNGEON;
    } else if (dungeon_level <= 4) {
        return (rand() % 100 < 50) ? BIOME_CRYPT : BIOME_DUNGEON;
    } else if (dungeon_level <= 6) {
        int roll = rand() % 100;
        if (roll < 33) return BIOME_FOREST;
        else if (roll < 66) return BIOME_CRYPT;
        else return BIOME_LAVA_CAVES;
    } else {
        int roll = rand() % 100;
        if (roll < 40) return BIOME_LAVA_CAVES;
        else if (roll < 70) return BIOME_ICE_CAVERN;
        else return BIOME_CRYPT;
    }
}

void apply_biome_to_room(Game *game, Room *room) {
    room->biome = game->current_biome;
}

void spawn_interactive_objects(Game *game) {
    for (int i = 0; i < game->room_count && game->object_count < MAX_INTERACTIVE_OBJECTS; i++) {
        Room room = game->rooms[i];
        
        // Skip first room and boss rooms
        if (i == 0 || room.is_boss_room) continue;
        
        int spawn_chance = 80;  // Increased from 60
        if (room.room_type == ROOM_TREASURE) spawn_chance = 100;
        
        // Only spawn ONE object per room
        if (rand() % 100 < spawn_chance) {
            InteractiveObject *obj = &game->objects[game->object_count];
            
            // Random position in room
            obj->pos.x = room.x + 1 + rand() % (room.width - 2);
            obj->pos.y = room.y + 1 + rand() % (room.height - 2);
            obj->activated = false;
            obj->opened = false;
            obj->linked_door_x = -1;
            obj->linked_door_y = -1;
            
            // Determine object type
            int roll = rand() % 100;
            
            if (room.room_type == ROOM_TREASURE) {
                // Treasure rooms have chests (with chance of mimic)
                if (rand() % 100 < 20) {
                    obj->type = OBJ_MIMIC;
                    strcpy(obj->name, "Suspicious Chest");
                } else {
                    obj->type = OBJ_CHEST;
                    strcpy(obj->name, "Treasure Chest");
                }
            } else if (roll < 20) {
                obj->type = OBJ_CHEST;
                strcpy(obj->name, "Chest");
                // Small chance of mimic
                if (rand() % 100 < 15) {
                    obj->type = OBJ_MIMIC;
                    strcpy(obj->name, "Suspicious Chest");
                }
            } else if (roll < 35) {
                obj->type = OBJ_SHRINE;
                // Random shrine type
                obj->shrine_buff = (ShrineType)(1 + rand() % 4);
                switch (obj->shrine_buff) {
                    case SHRINE_STRENGTH: strcpy(obj->name, "Shrine of Strength"); break;
                    case SHRINE_DEFENSE: strcpy(obj->name, "Shrine of Defense"); break;
                    case SHRINE_VITALITY: strcpy(obj->name, "Shrine of Vitality"); break;
                    case SHRINE_MYSTIC: strcpy(obj->name, "Shrine of Mystic"); break;
                    default: strcpy(obj->name, "Shrine"); break;
                }
            } else if (roll < 80) {  // LEVERS now 45% chance (most common!)
                obj->type = OBJ_LEVER;
                strcpy(obj->name, "Lever");
                
                // Try to create a REAL hidden room that's carved out but sealed off
                Room *current = &room;
                int hidden_x = 0, hidden_y = 0;
                int hidden_w = 5;  // Smaller for better fit
                int hidden_h = 4;  // Smaller for better fit
                bool found_spot = false;
                int door_x = 0, door_y = 0;
                
                // Try ALL 4 directions to find a valid spot
                int directions[] = {0, 1, 2, 3};  // right, left, below, above
                
                // Shuffle directions for variety
                for (int j = 0; j < 4; j++) {
                    int swap = rand() % 4;
                    int temp = directions[j];
                    directions[j] = directions[swap];
                    directions[swap] = temp;
                }
                
                for (int dir_idx = 0; dir_idx < 4 && !found_spot; dir_idx++) {
                    int dir = directions[dir_idx];
                    
                    if (dir == 0) {  // RIGHT
                        if (current->x + current->width + hidden_w + 2 < MAP_WIDTH - 2) {
                            hidden_x = current->x + current->width + 2;
                            hidden_y = current->y + (current->height - hidden_h) / 2;
                            if (hidden_y < 1) hidden_y = 1;
                            door_x = current->x + current->width;
                            door_y = current->y + current->height / 2;
                            found_spot = true;
                        }
                    } else if (dir == 1) {  // LEFT
                        if (current->x - hidden_w - 2 >= 2) {
                            hidden_x = current->x - hidden_w - 2;
                            hidden_y = current->y + (current->height - hidden_h) / 2;
                            if (hidden_y < 1) hidden_y = 1;
                            door_x = current->x - 1;
                            door_y = current->y + current->height / 2;
                            found_spot = true;
                        }
                    } else if (dir == 2) {  // BELOW
                        if (current->y + current->height + hidden_h + 2 < MAP_HEIGHT - 2) {
                            hidden_x = current->x + (current->width - hidden_w) / 2;
                            if (hidden_x < 1) hidden_x = 1;
                            hidden_y = current->y + current->height + 2;
                            door_x = current->x + current->width / 2;
                            door_y = current->y + current->height;
                            found_spot = true;
                        }
                    } else {  // ABOVE
                        if (current->y - hidden_h - 2 >= 2) {
                            hidden_x = current->x + (current->width - hidden_w) / 2;
                            if (hidden_x < 1) hidden_x = 1;
                            hidden_y = current->y - hidden_h - 2;
                            door_x = current->x + current->width / 2;
                            door_y = current->y - 1;
                            found_spot = true;
                        }
                    }
                    
                    if (found_spot) {
                        // VERIFY the area is COMPLETELY walls (no existing floor/corridors)
                        bool area_is_sealed = true;
                        for (int check_y = hidden_y - 1; check_y <= hidden_y + hidden_h; check_y++) {
                            for (int check_x = hidden_x - 1; check_x <= hidden_x + hidden_w; check_x++) {
                                if (check_x >= 0 && check_x < MAP_WIDTH && 
                                    check_y >= 0 && check_y < MAP_HEIGHT) {
                                    if (game->map[check_y][check_x] != TILE_WALL) {
                                        area_is_sealed = false;
                                        break;
                                    }
                                }
                            }
                            if (!area_is_sealed) break;
                        }
                        
                        // If not sealed, try next direction
                        if (!area_is_sealed) {
                            found_spot = false;
                            continue;
                        }
                        
                        // Found a valid spot! Break out of direction loop
                        break;
                    }
                }
                
                // Set lever status based on whether we found a spot
                if (!found_spot) {
                    // Couldn't find valid spot - don't spawn this lever at all!
                    // Instead spawn a different object type
                    int fallback_roll = rand() % 100;
                    if (fallback_roll < 60) {
                        obj->type = OBJ_CHEST;
                        strcpy(obj->name, "Chest");
                    } else {
                        obj->type = OBJ_SHRINE;
                        obj->shrine_buff = (ShrineType)(1 + rand() % 4);
                        switch (obj->shrine_buff) {
                            case SHRINE_STRENGTH: strcpy(obj->name, "Shrine of Strength"); break;
                            case SHRINE_DEFENSE: strcpy(obj->name, "Shrine of Defense"); break;
                            case SHRINE_VITALITY: strcpy(obj->name, "Shrine of Vitality"); break;
                            case SHRINE_MYSTIC: strcpy(obj->name, "Shrine of Mystic"); break;
                            default: strcpy(obj->name, "Shrine"); break;
                        }
                    }
                    // Don't create hidden room code below
                    game->object_count++;
                    continue;  // Skip to next room
                }
                
                if (found_spot) {
                    // ACTUALLY CREATE THE HIDDEN ROOM (but keep it sealed)
                    for (int sy = hidden_y; sy < hidden_y + hidden_h; sy++) {
                        for (int sx = hidden_x; sx < hidden_x + hidden_w; sx++) {
                            if (sx >= 0 && sx < MAP_WIDTH && sy >= 0 && sy < MAP_HEIGHT) {
                                game->map[sy][sx] = TILE_FLOOR;
                            }
                        }
                    }
                    
                    // Surround it with walls (to seal it)
                    for (int sx = hidden_x - 1; sx <= hidden_x + hidden_w; sx++) {
                        if (sx >= 0 && sx < MAP_WIDTH) {
                            if (hidden_y - 1 >= 0) game->map[hidden_y - 1][sx] = TILE_WALL;
                            if (hidden_y + hidden_h < MAP_HEIGHT) game->map[hidden_y + hidden_h][sx] = TILE_WALL;
                        }
                    }
                    for (int sy = hidden_y; sy < hidden_y + hidden_h; sy++) {
                        if (sy >= 0 && sy < MAP_HEIGHT) {
                            if (hidden_x - 1 >= 0) game->map[sy][hidden_x - 1] = TILE_WALL;
                            if (hidden_x + hidden_w < MAP_WIDTH) game->map[sy][hidden_x + hidden_w] = TILE_WALL;
                        }
                    }
                    
                    // Mark the door location as a SECRET DOOR (keeps it sealed)
                    if (door_x >= 0 && door_x < MAP_WIDTH && door_y >= 0 && door_y < MAP_HEIGHT) {
                        game->map[door_y][door_x] = TILE_SECRET_DOOR;
                    }
                    
                    // Store location for lever to open later
                    obj->linked_door_x = door_x;
                    obj->linked_door_y = door_y;
                    
                    // Add treasure to the hidden room NOW
                    int treasure_x = hidden_x + hidden_w / 2;
                    int treasure_y = hidden_y + hidden_h / 2;
                    
                    if (treasure_x < MAP_WIDTH && treasure_y < MAP_HEIGHT && game->item_count < MAX_ITEMS) {
                        Item *treasure = &game->items[game->item_count];
                        treasure->pos.x = treasure_x;
                        treasure->pos.y = treasure_y;
                        treasure->type = ITEM_GOLD;
                        treasure->value = 50 + rand() % 100 + (game->dungeon_level * 20);
                        strcpy(treasure->name, "Hidden Treasure");
                        treasure->collected = false;
                        game->item_count++;
                        
                        // Maybe add a chest
                        if (rand() % 100 < 60 && game->object_count < MAX_INTERACTIVE_OBJECTS) {
                            InteractiveObject *chest = &game->objects[game->object_count];
                            chest->pos.x = treasure_x + 1;
                            chest->pos.y = treasure_y;
                            chest->type = OBJ_CHEST;
                            chest->opened = false;
                            chest->activated = false;
                            strcpy(chest->name, "Hidden Chest");
                            game->object_count++;
                        }
                    }
                }
            } else {
                obj->type = OBJ_PRESSURE_PLATE;
                strcpy(obj->name, "Pressure Plate");
            }
            
            game->object_count++;
        }
    }
    
    // MERCHANT SPAWNING SYSTEM (30% per floor, guaranteed every 3rd)
    bool has_merchant = false;
    
    // Try to spawn merchant with 30% chance
    if (rand() % 100 < 30) {
        // Find a safe room or normal room to spawn merchant in
        for (int i = 1; i < game->room_count - 1 && game->object_count < MAX_INTERACTIVE_OBJECTS; i++) {
            Room room = game->rooms[i];
            
            // Prefer safe rooms, but allow normal rooms too
            if (room.room_type == ROOM_SAFE || room.room_type == ROOM_NORMAL) {
                InteractiveObject *merchant = &game->objects[game->object_count];
                
                // Place in center of room
                merchant->pos.x = room.x + room.width / 2;
                merchant->pos.y = room.y + room.height / 2;
                merchant->type = OBJ_MERCHANT;
                merchant->activated = false;
                merchant->opened = false;
                strcpy(merchant->name, "Merchant");
                
                game->object_count++;
                has_merchant = true;
                break;
            }
        }
    }
    
    // Smart guarantee: if no merchant, track it
    if (has_merchant) {
        game->floors_since_merchant = 0;
    } else {
        game->floors_since_merchant++;
        
        // Force spawn if 2 floors without merchant
        if (game->floors_since_merchant >= 2 && game->object_count < MAX_INTERACTIVE_OBJECTS) {
            // Find ANY room (not first or boss) to force spawn
            for (int i = 1; i < game->room_count - 1; i++) {
                Room room = game->rooms[i];
                if (room.room_type != ROOM_BOSS && !room.is_boss_room) {
                    InteractiveObject *merchant = &game->objects[game->object_count];
                    
                    merchant->pos.x = room.x + room.width / 2;
                    merchant->pos.y = room.y + room.height / 2;
                    merchant->type = OBJ_MERCHANT;
                    merchant->activated = false;
                    merchant->opened = false;
                    strcpy(merchant->name, "Traveling Merchant");
                    
                    game->object_count++;
                    game->floors_since_merchant = 0;
                    break;
                }
            }
        }
    }
}

void spawn_hazards(Game *game) {
    BiomeType biome = game->current_biome;
    
    for (int i = 0; i < game->room_count && game->hazard_count < MAX_HAZARDS; i++) {
        Room room = game->rooms[i];
        
        // Skip safe rooms and first room
        if (i == 0 || room.room_type == ROOM_SAFE) continue;
        
        int num_hazards = 0;
        TileType hazard_type = TILE_FLOOR;
        
        // Choose hazards based on biome
        switch (biome) {
            case BIOME_LAVA_CAVES:
                num_hazards = 2 + rand() % 4;
                hazard_type = TILE_LAVA;
                break;
            case BIOME_CRYPT:
                if (rand() % 100 < 40) {
                    num_hazards = 1 + rand() % 3;
                    hazard_type = TILE_POISON_GAS;
                }
                break;
            case BIOME_DUNGEON:
                if (rand() % 100 < 30) {
                    num_hazards = 1 + rand() % 2;
                    hazard_type = TILE_SPIKE_TRAP;
                }
                break;
            case BIOME_FOREST:
                // Forests have fewer hazards
                if (rand() % 100 < 20) {
                    num_hazards = 1;
                    hazard_type = TILE_POISON_GAS;
                }
                break;
            case BIOME_ICE_CAVERN:
                // Ice caverns don't have many hazards (slippery floor could be future feature)
                break;
        }
        
        for (int j = 0; j < num_hazards && game->hazard_count < MAX_HAZARDS; j++) {
            EnvironmentalHazard *hazard = &game->hazards[game->hazard_count];
            
            hazard->pos.x = room.x + 1 + rand() % (room.width - 2);
            hazard->pos.y = room.y + 1 + rand() % (room.height - 2);
            hazard->type = hazard_type;
            hazard->active = true;
            
            // Set damage based on type
            switch (hazard_type) {
                case TILE_LAVA:
                    hazard->damage = 15 + rand() % 10;
                    break;
                case TILE_SPIKE_TRAP:
                    hazard->damage = 10 + rand() % 5;
                    break;
                case TILE_POISON_GAS:
                    hazard->damage = 5;  // Applies poison instead
                    break;
                default:
                    hazard->damage = 5;
                    break;
            }
            
            // Place on map
            game->map[hazard->pos.y][hazard->pos.x] = hazard_type;
            game->hazard_count++;
        }
    }
}

InteractiveObject* get_object_at(Game *game, int x, int y) {
    for (int i = 0; i < game->object_count; i++) {
        if (game->objects[i].pos.x == x && game->objects[i].pos.y == y) {
            return &game->objects[i];
        }
    }
    return NULL;
}

EnvironmentalHazard* get_hazard_at(Game *game, int x, int y) {
    for (int i = 0; i < game->hazard_count; i++) {
        if (game->hazards[i].pos.x == x && game->hazards[i].pos.y == y && 
            game->hazards[i].active) {
            return &game->hazards[i];
        }
    }
    return NULL;
}

void interact_with_object(Game *game, int x, int y) {
    InteractiveObject *obj = get_object_at(game, x, y);
    if (!obj) {
        // Check if there's an item here instead
        Item *item = get_item_at(game, x, y);
        if (item != NULL) {
            set_message(game, "That's an item, just walk over it to pick it up!");
        } else {
            set_message(game, "Nothing to interact with here.");
        }
        return;
    }
    
    char msg[100];
    
    switch (obj->type) {
        case OBJ_CHEST:
            if (obj->opened) {
                set_message(game, "The chest is empty.");
            } else {
                obj->opened = true;
                // Spawn loot
                int gold = 20 + rand() % 50 + (game->dungeon_level * 10);
                game->player.gold += gold;
                sprintf(msg, "Opened chest! Found %d gold!", gold);
                set_message(game, msg);
                
                // Chance for extra item
                if (rand() % 100 < 50) {
                    set_message(game, "Also found a health potion!");
                }
            }
            break;
            
        case OBJ_MIMIC:
            if (!obj->activated) {
                obj->activated = true;
                set_message(game, "IT'S A MIMIC! The chest attacks you!");
                
                // Mimic deals damage
                int damage = 20 + rand() % 15;
                game->player.hp -= damage;
                sprintf(msg, "The mimic bites for %d damage!", damage);
                set_message(game, msg);
                
                // TODO: Could spawn a mimic enemy here
            }
            break;
            
        case OBJ_LEVER:
            if (obj->linked_door_x >= 0 && obj->linked_door_y >= 0) {
                if (!obj->activated) {
                    obj->activated = true;
                    
                    // Simply open the secret door!
                    int door_x = obj->linked_door_x;
                    int door_y = obj->linked_door_y;
                    
                    if (door_x >= 0 && door_x < MAP_WIDTH && 
                        door_y >= 0 && door_y < MAP_HEIGHT) {
                        game->map[door_y][door_x] = TILE_FLOOR;
                    }
                    
                    set_message(game, "You pull the lever! A secret door opens revealing a HIDDEN ROOM!");
                } else {
                    set_message(game, "The lever has already been pulled.");
                }
            } else {
                set_message(game, "The lever doesn't seem to do anything...");
            }
            break;
            
        case OBJ_SHRINE:
            if (!obj->activated) {
                obj->activated = true;
                
                switch (obj->shrine_buff) {
                    case SHRINE_STRENGTH:
                        game->player.base_damage += 5;
                        set_message(game, "Shrine of Strength! +5 damage permanently!");
                        break;
                    case SHRINE_DEFENSE:
                        game->player.equipment.armor_defense += 3;
                        set_message(game, "Shrine of Defense! +3 defense permanently!");
                        break;
                    case SHRINE_VITALITY:
                        game->player.max_hp += 20;
                        game->player.hp += 20;
                        set_message(game, "Shrine of Vitality! +20 max HP!");
                        break;
                    case SHRINE_MYSTIC:
                        game->player.max_mana += 15;
                        game->player.mana += 15;
                        set_message(game, "Shrine of Mystic! +15 max mana!");
                        break;
                    default:
                        break;
                }
            } else {
                set_message(game, "The shrine's power has been exhausted.");
            }
            break;
            
        case OBJ_PRESSURE_PLATE:
            set_message(game, "You step on a pressure plate... *click*");
            // Could trigger traps, spawn enemies, open doors, etc.
            break;
            
        case OBJ_MERCHANT:
            set_message(game, "Greetings, traveler! Press 'M' to browse my wares.");
            // Actual shop UI will be handled in game loop
            break;
            
        default:
            break;
    }
}

void process_hazard_damage(Game *game) {
    EnvironmentalHazard *hazard = get_hazard_at(game, game->player.pos.x, game->player.pos.y);
    
    if (!hazard) return;
    
    char msg[100];
    
    switch (hazard->type) {
        case TILE_LAVA:
            game->player.hp -= hazard->damage;
            sprintf(msg, "LAVA! You take %d fire damage!", hazard->damage);
            set_message(game, msg);
            apply_status_to_player(game, STATUS_BURN, 3, 5);
            break;
            
        case TILE_SPIKE_TRAP:
            game->player.hp -= hazard->damage;
            sprintf(msg, "SPIKE TRAP! You take %d damage!", hazard->damage);
            set_message(game, msg);
            hazard->active = false;  // Traps are one-time
            game->map[hazard->pos.y][hazard->pos.x] = TILE_FLOOR;
            break;
            
        case TILE_POISON_GAS:
            sprintf(msg, "POISON GAS! You're poisoned!");
            set_message(game, msg);
            apply_status_to_player(game, STATUS_POISON, 5, 3);
            break;
            
        default:
            break;
    }
}