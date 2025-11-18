#include "player.h"
#include "utils.h"
#include "dungeon.h"
#include "enemy.h"
#include "item.h"
#include "inventory.h"
#include "fov.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_total_damage(Player *player) {
    return player->base_damage + player->equipment.weapon_damage;
}

int get_damage_reduction(Player *player) {
    return player->equipment.armor_defense;
}

void gain_xp(Game *game, int xp) {
    game->player.xp += xp;
    
    if (game->player.xp >= game->player.xp_to_next_level) {
        level_up(game);
    }
}

void level_up(Game *game) {
    char msg[150];
    game->player.level++;
    game->player.xp = 0;
    game->player.xp_to_next_level = 50 + (game->player.level * 30);
    
    int hp_gain = 20 + rand() % 10;
    int dmg_gain = 3 + rand() % 3;
    int mana_gain = 10 + rand() % 10;
    
    game->player.max_hp += hp_gain;
    game->player.hp += hp_gain;
    game->player.max_mana += mana_gain;
    game->player.mana += mana_gain;
    game->player.base_damage += dmg_gain;
    
    sprintf(msg, "LEVEL UP! Lv%d! HP+%d, Mana+%d, Dmg+%d, Fully healed!", 
            game->player.level, hp_gain, mana_gain, dmg_gain);
    set_message(game, msg);
}

void equip_weapon(Game *game, WeaponType weapon, int damage, const char *name) {
    char msg[100];
    game->player.equipment.weapon = weapon;
    game->player.equipment.weapon_damage = damage;
    strcpy(game->player.equipment.weapon_name, name);
    sprintf(msg, "Equipped %s! (+%d damage)", name, damage);
    set_message(game, msg);
}

void equip_armor(Game *game, ArmorType armor, int defense, const char *name) {
    char msg[100];
    game->player.equipment.armor = armor;
    game->player.equipment.armor_defense = defense;
    strcpy(game->player.equipment.armor_name, name);
    sprintf(msg, "Equipped %s! (+%d defense)", name, defense);
    set_message(game, msg);
}

void attack_enemy(Game *game, Enemy *enemy) {
    char msg[100];
    int total_damage = get_total_damage(&game->player);
    enemy->hp -= total_damage;
    
    if (enemy->hp <= 0) {
        enemy->alive = false;
        game->player.gold += enemy->gold_drop;
        gain_xp(game, enemy->xp_value);
        sprintf(msg, "Defeated %s! (+%d gold, +%d XP)", 
                enemy->name, enemy->gold_drop, enemy->xp_value);
        set_message(game, msg);
    } else {
        sprintf(msg, "Hit %s for %d! HP: %d/%d", 
                enemy->name, total_damage, enemy->hp, enemy->max_hp);
        set_message(game, msg);
    }
}

void pickup_item(Game *game, Item *item) {
    char msg[100];
    
    if (item->type == ITEM_WEAPON) {
        InventoryItem inv_item;
        inv_item.type = ITEM_WEAPON;
        inv_item.weapon_type = item->weapon_type;
        inv_item.value = item->bonus;
        strcpy(inv_item.name, item->name);
        inv_item.exists = true;
        add_to_inventory(game, inv_item);
        sprintf(msg, "Picked up %s (+%d dmg)", item->name, item->bonus);
        set_message(game, msg);
        item->collected = true;
    } else if (item->type == ITEM_ARMOR) {
        InventoryItem inv_item;
        inv_item.type = ITEM_ARMOR;
        inv_item.armor_type = item->armor_type;
        inv_item.value = item->bonus;
        strcpy(inv_item.name, item->name);
        inv_item.exists = true;
        add_to_inventory(game, inv_item);
        sprintf(msg, "Picked up %s (+%d def)", item->name, item->bonus);
        set_message(game, msg);
        item->collected = true;
    } else if (item->type == ITEM_HEALTH_POTION) {
        InventoryItem inv_item;
        inv_item.type = ITEM_HEALTH_POTION;
        inv_item.value = item->value;
        strcpy(inv_item.name, "Health Potion");
        inv_item.exists = true;
        add_to_inventory(game, inv_item);
        sprintf(msg, "Picked up Health Potion");
        set_message(game, msg);
        item->collected = true;
    } else if (item->type == ITEM_MANA_POTION) {
        InventoryItem inv_item;
        inv_item.type = ITEM_MANA_POTION;
        inv_item.value = item->value;
        strcpy(inv_item.name, "Mana Potion");
        inv_item.exists = true;
        add_to_inventory(game, inv_item);
        sprintf(msg, "Picked up Mana Potion");
        set_message(game, msg);
        item->collected = true;
    } else if (item->type == ITEM_GOLD) {
        game->player.gold += item->value;
        sprintf(msg, "Collected %d gold!", item->value);
        set_message(game, msg);
        item->collected = true;
    }
}

void move_player(Game *game, int dx, int dy) {
    int new_x = game->player.pos.x + dx;
    int new_y = game->player.pos.y + dy;
    
    if (new_x == game->stairs_pos.x && new_y == game->stairs_pos.y) {
        descend_stairs(game);
        return;
    }
    
    Enemy *enemy = get_enemy_at(game, new_x, new_y);
    if (enemy != NULL) {
        attack_enemy(game, enemy);
        if (enemy->alive) {
            int damage = enemy->damage - get_damage_reduction(&game->player);
            if (damage < 0) damage = 0;
            game->player.hp -= damage;
        }
        return;
    }
    
    if (is_walkable(game, new_x, new_y)) {
        game->player.pos.x = new_x;
        game->player.pos.y = new_y;
        
        Item *item = get_item_at(game, new_x, new_y);
        if (item != NULL) {
            pickup_item(game, item);
        }
    }
}

void descend_stairs(Game *game) {
    char msg[100];
    game->dungeon_level++;
    sprintf(msg, "Descended to floor %d!", game->dungeon_level);
    set_message(game, msg);
    
    // Save player stats (but NOT position)
    int saved_hp = game->player.hp;
    int saved_max_hp = game->player.max_hp;
    int saved_mana = game->player.mana;
    int saved_max_mana = game->player.max_mana;
    int saved_base_damage = game->player.base_damage;
    int saved_gold = game->player.gold;
    int saved_xp = game->player.xp;
    int saved_level = game->player.level;
    int saved_xp_to_next = game->player.xp_to_next_level;
    Equipment saved_equipment = game->player.equipment;
    int saved_inv_count = game->player.inventory_count;
    InventoryItem saved_inventory[MAX_INVENTORY];
    for (int i = 0; i < saved_inv_count; i++) {
        saved_inventory[i] = game->player.inventory[i];
    }
    
    game->room_count = 0;
    game->enemy_count = 0;
    game->item_count = 0;
    
    // FIXED: Generate dungeon (this sets new player position)
    generate_dungeon(game);
    
    // Verify dungeon generated successfully
    if (game->room_count == 0) {
        set_message(game, "ERROR: Failed to generate floor! Retrying...");
        generate_dungeon(game);  // Try one more time
    }
    
    spawn_enemies(game);
    spawn_items(game);
    spawn_stairs(game);
    
    // Restore player stats (position is already set by generate_dungeon)
    game->player.hp = saved_hp;
    game->player.max_hp = saved_max_hp;
    game->player.mana = saved_mana;
    game->player.max_mana = saved_max_mana;
    game->player.base_damage = saved_base_damage;
    game->player.gold = saved_gold;
    game->player.xp = saved_xp;
    game->player.level = saved_level;
    game->player.xp_to_next_level = saved_xp_to_next;
    game->player.equipment = saved_equipment;
    game->player.inventory_count = saved_inv_count;
    for (int i = 0; i < saved_inv_count; i++) {
        game->player.inventory[i] = saved_inventory[i];
    }
    
    // CRITICAL: Recalculate FOV for new floor!
    calculate_fov(game);
}