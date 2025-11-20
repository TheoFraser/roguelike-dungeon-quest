#include "random_events.h"
#include "utils.h"
#include "enemy.h"
#include "item.h"
#include "dungeon_features.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void trigger_random_event(Game *game) {
    // 40% chance of event per floor (floor 1 = no events)
    if (game->dungeon_level == 1 || rand() % 100 >= 40) {
        game->current_event = EVENT_NONE;
        game->event_active = false;
        return;
    }
    
    // Choose random event
    int roll = rand() % 100;
    
    if (roll < 15) {
        game->current_event = EVENT_AMBUSH;
    } else if (roll < 30) {
        game->current_event = EVENT_TREASURE;
    } else if (roll < 45) {
        game->current_event = EVENT_BLESSING;
    } else if (roll < 55) {
        game->current_event = EVENT_CURSE;
    } else if (roll < 65) {
        game->current_event = EVENT_MERCHANT;
    } else if (roll < 75) {
        game->current_event = EVENT_SHRINE;
    } else if (roll < 85) {
        game->current_event = EVENT_HEALING;
    } else {
        game->current_event = EVENT_XP_BONUS;
    }
    
    game->event_active = true;
    game->event_duration = -1;  // Permanent unless specified
    
    apply_event_effects(game);
}

void apply_event_effects(Game *game) {
    char msg[150];
    
    switch (game->current_event) {
        case EVENT_AMBUSH:
            set_message(game, "[!] AMBUSH! Extra enemies appear!");
            spawn_ambush_enemies(game);
            break;
            
        case EVENT_TREASURE:
            {
                int bonus_gold = 50 + rand() % 100 + (game->dungeon_level * 20);
                game->player.gold += bonus_gold;
                snprintf(msg, sizeof(msg), "[$] TREASURE CACHE! Found %d gold!", bonus_gold);
                set_message(game, msg);
                
                // Spawn 2-3 bonus items
                for (int i = 0; i < 2 + rand() % 2; i++) {
                    spawn_bonus_item(game);
                }
            }
            break;
            
        case EVENT_BLESSING:
            game->player.base_damage += 5;
            game->player.max_hp += 20;
            game->player.hp += 20;
            set_message(game, "[*] DIVINE BLESSING! +5 damage, +20 max HP!");
            break;
            
        case EVENT_CURSE:
            game->player.base_damage -= 3;
            if (game->player.base_damage < 5) game->player.base_damage = 5;
            set_message(game, "[X] CURSE! -3 damage this floor!");
            game->event_duration = 999;  // Until next floor
            break;
            
        case EVENT_MERCHANT:
            set_message(game, "[M] MYSTERIOUS MERCHANT appears!");
            spawn_event_merchant(game);
            break;
            
        case EVENT_SHRINE:
            set_message(game, "[?] MYSTICAL SHRINE found! Walk to it for buff!");
            spawn_event_shrine(game);
            break;
            
        case EVENT_HEALING:
            game->player.hp = game->player.max_hp;
            game->player.mana = game->player.max_mana;
            set_message(game, "[+] HEALING FOUNTAIN! Fully restored!");
            break;
            
        case EVENT_XP_BONUS:
            set_message(game, "[^] XP BONUS! +50% XP gains this floor!");
            game->event_duration = 999;  // Until next floor
            break;
            
        default:
            break;
    }
}

void spawn_ambush_enemies(Game *game) {
    // Spawn 3-5 extra enemies near player
    int num_enemies = 3 + rand() % 3;
    
    for (int i = 0; i < num_enemies && game->enemy_count < MAX_ENEMIES; i++) {
        // Find position near player (within 5 tiles)
        int attempts = 0;
        while (attempts < 50) {
            int dx = -5 + rand() % 11;  // -5 to +5
            int dy = -5 + rand() % 11;
            int x = game->player.pos.x + dx;
            int y = game->player.pos.y + dy;
            
            if (is_walkable(game, x, y) && get_enemy_at(game, x, y) == NULL) {
                // Create enemy
                Enemy *enemy = &game->enemies[game->enemy_count];
                
                int type_roll = rand() % 100;
                EnemyType type;
                if (type_roll < 50) type = ENEMY_GOBLIN;
                else if (type_roll < 75) type = ENEMY_BAT;
                else type = ENEMY_ORC;
                
                create_enemy(enemy, type, x, y, game->dungeon_level);
                game->enemy_count++;
                break;
            }
            attempts++;
        }
    }
}

void spawn_bonus_item(Game *game) {
    if (game->item_count >= MAX_ITEMS) return;
    
    // Spawn in random room
    if (game->room_count == 0) return;
    
    int room_idx = rand() % game->room_count;
    Room *room = &game->rooms[room_idx];
    
    int x = room->x + 1 + rand() % (room->width - 2);
    int y = room->y + 1 + rand() % (room->height - 2);
    
    Item *item = &game->items[game->item_count];
    
    int roll = rand() % 100;
    if (roll < 30) {
        create_weapon_item(item, x, y, game->dungeon_level);
    } else if (roll < 60) {
        create_armor_item(item, x, y, game->dungeon_level);
    } else if (roll < 80) {
        item->type = ITEM_HEALTH_POTION;
        item->pos.x = x;
        item->pos.y = y;
        item->value = 30 + rand() % 20;
        strcpy(item->name, "Health Potion");
        item->collected = false;
    } else {
        item->type = ITEM_MANA_POTION;
        item->pos.x = x;
        item->pos.y = y;
        item->value = 25 + rand() % 15;
        strcpy(item->name, "Mana Potion");
        item->collected = false;
    }
    
    game->item_count++;
}

void spawn_event_merchant(Game *game) {
    // Find a safe room to spawn merchant
    for (int i = 0; i < game->room_count; i++) {
        Room *room = &game->rooms[i];
        if (room->room_type == ROOM_SAFE || room->room_type == ROOM_NORMAL) {
            if (game->object_count < MAX_INTERACTIVE_OBJECTS) {
                InteractiveObject *merchant = &game->objects[game->object_count];
                merchant->pos.x = room->x + room->width / 2;
                merchant->pos.y = room->y + room->height / 2;
                merchant->type = OBJ_MERCHANT;
                merchant->activated = false;
                merchant->opened = false;
                strcpy(merchant->name, "Mysterious Merchant");
                game->object_count++;
                break;
            }
        }
    }
}

void spawn_event_shrine(Game *game) {
    // Find a room to spawn shrine
    if (game->room_count < 2) return;
    
    int room_idx = 1 + rand() % (game->room_count - 1);
    Room *room = &game->rooms[room_idx];
    
    if (game->object_count < MAX_INTERACTIVE_OBJECTS) {
        InteractiveObject *shrine = &game->objects[game->object_count];
        shrine->pos.x = room->x + room->width / 2;
        shrine->pos.y = room->y + room->height / 2;
        shrine->type = OBJ_SHRINE;
        shrine->activated = false;
        
        // Random shrine buff
        int buff_roll = rand() % 3;
        shrine->shrine_buff = (ShrineType)buff_roll;
        strcpy(shrine->name, "Mystical Shrine");
        
        game->object_count++;
    }
}

void modify_xp_gain(Game *game, int *xp) {
    if (game->event_active && game->current_event == EVENT_XP_BONUS) {
        *xp = (int)(*xp * 1.5);  // 50% bonus
    }
}

void clear_floor_events(Game *game) {
    // Remove temporary event effects when changing floors
    if (game->current_event == EVENT_CURSE) {
        game->player.base_damage += 3;  // Remove curse
    }
    
    game->current_event = EVENT_NONE;
    game->event_active = false;
    game->event_duration = 0;
}