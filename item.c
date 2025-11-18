#include "item.h"
#include <stdlib.h>
#include <string.h>

void create_weapon_item(Item *item, int x, int y, int dungeon_level) {
    item->pos.x = x;
    item->pos.y = y;
    item->type = ITEM_WEAPON;
    item->collected = false;
    
    int tier = (rand() % 100) + (dungeon_level * 10);
    
    if (tier < 30) {
        item->weapon_type = WEAPON_DAGGER;
        item->bonus = 5 + rand() % 5;
        strcpy(item->name, "Dagger");
    } else if (tier < 60) {
        item->weapon_type = WEAPON_SWORD;
        item->bonus = 10 + rand() % 10;
        strcpy(item->name, "Sword");
    } else if (tier < 85) {
        item->weapon_type = WEAPON_AXE;
        item->bonus = 15 + rand() % 15;
        strcpy(item->name, "Axe");
    } else {
        item->weapon_type = WEAPON_GREATSWORD;
        item->bonus = 25 + rand() % 20;
        strcpy(item->name, "Greatsword");
    }
}

void create_armor_item(Item *item, int x, int y, int dungeon_level) {
    item->pos.x = x;
    item->pos.y = y;
    item->type = ITEM_ARMOR;
    item->collected = false;
    
    int tier = (rand() % 100) + (dungeon_level * 10);
    
    if (tier < 40) {
        item->armor_type = ARMOR_LEATHER;
        item->bonus = 2 + rand() % 3;
        strcpy(item->name, "Leather Armor");
    } else if (tier < 70) {
        item->armor_type = ARMOR_CHAINMAIL;
        item->bonus = 5 + rand() % 5;
        strcpy(item->name, "Chainmail");
    } else {
        item->armor_type = ARMOR_PLATE;
        item->bonus = 10 + rand() % 10;
        strcpy(item->name, "Plate Armor");
    }
}

void spawn_items(Game *game) {
    for (int i = 0; i < game->room_count && game->item_count < MAX_ITEMS; i++) {
        Room room = game->rooms[i];
        
        // Skip boss rooms
        if (room.is_boss_room || room.room_type == ROOM_BOSS) continue;
        
        // Determine item spawn based on room type
        int num_items;
        int spawn_chance;
        
        switch (room.room_type) {
            case ROOM_TREASURE:
                num_items = 3 + rand() % 4;  // 3-6 items
                spawn_chance = 100;  // Always spawn
                break;
            case ROOM_SAFE:
                num_items = 1 + rand() % 2;  // 1-2 items
                spawn_chance = 80;
                break;
            case ROOM_MONSTER_DEN:
                num_items = 1;
                spawn_chance = 40;  // Less loot in dangerous rooms
                break;
            default:
                num_items = 1;
                spawn_chance = 60 + (game->dungeon_level * 5);
                break;
        }
        
        if (rand() % 100 < spawn_chance) {
            for (int j = 0; j < num_items && game->item_count < MAX_ITEMS; j++) {
                Item *item = &game->items[game->item_count];
                int x = room.x + 1 + rand() % (room.width - 2);
                int y = room.y + 1 + rand() % (room.height - 2);
                item->collected = false;
                
                int item_roll = rand() % 100;
                
                // Treasure rooms have better loot!
                if (room.room_type == ROOM_TREASURE) {
                    if (item_roll < 20) {
                        item->type = ITEM_HEALTH_POTION;
                        item->pos.x = x;
                        item->pos.y = y;
                        item->value = 20 + rand() % 30;
                        strcpy(item->name, "Health Potion");
                    } else if (item_roll < 35) {
                        item->type = ITEM_MANA_POTION;
                        item->pos.x = x;
                        item->pos.y = y;
                        item->value = 15 + rand() % 25;
                        strcpy(item->name, "Mana Potion");
                    } else if (item_roll < 60) {
                        item->type = ITEM_GOLD;
                        item->pos.x = x;
                        item->pos.y = y;
                        item->value = 30 + rand() % 70 + (game->dungeon_level * 10);  // More gold!
                        strcpy(item->name, "Gold");
                    } else if (item_roll < 80) {
                        create_weapon_item(item, x, y, game->dungeon_level + 1);  // Better items
                    } else {
                        create_armor_item(item, x, y, game->dungeon_level + 1);
                    }
                } else {
                    // Normal item distribution
                    if (item_roll < 25) {
                        item->type = ITEM_HEALTH_POTION;
                        item->pos.x = x;
                        item->pos.y = y;
                        item->value = 20 + rand() % 30;
                        strcpy(item->name, "Health Potion");
                    } else if (item_roll < 40) {
                        item->type = ITEM_MANA_POTION;
                        item->pos.x = x;
                        item->pos.y = y;
                        item->value = 15 + rand() % 25;
                        strcpy(item->name, "Mana Potion");
                    } else if (item_roll < 55) {
                        item->type = ITEM_GOLD;
                        item->pos.x = x;
                        item->pos.y = y;
                        item->value = 10 + rand() % 40 + (game->dungeon_level * 5);
                        strcpy(item->name, "Gold");
                    } else if (item_roll < 77) {
                        create_weapon_item(item, x, y, game->dungeon_level);
                    } else {
                        create_armor_item(item, x, y, game->dungeon_level);
                    }
                }
                game->item_count++;
            }
        }
    }
}