#include "inventory.h"
#include "utils.h"
#include "player.h"
#include <stdio.h>

void add_to_inventory(Game *game, InventoryItem item) {
    if (game->player.inventory_count >= MAX_INVENTORY) {
        set_message(game, "Inventory full! Drop something first (press 'i')");
        return;
    }
    
    game->player.inventory[game->player.inventory_count] = item;
    game->player.inventory[game->player.inventory_count].exists = true;
    game->player.inventory_count++;
}

void use_inventory_item(Game *game, int index) {
    if (index < 0 || index >= game->player.inventory_count) return;
    
    InventoryItem *item = &game->player.inventory[index];
    if (!item->exists) return;
    
    char msg[100];
    
    if (item->type == ITEM_HEALTH_POTION) {
        int old_hp = game->player.hp;
        game->player.hp += item->value;
        if (game->player.hp > game->player.max_hp) {
            game->player.hp = game->player.max_hp;
        }
        int healed = game->player.hp - old_hp;
        sprintf(msg, "Used health potion! Restored %d HP.", healed);
        set_message(game, msg);
        
        item->exists = false;
        for (int i = index; i < game->player.inventory_count - 1; i++) {
            game->player.inventory[i] = game->player.inventory[i + 1];
        }
        game->player.inventory_count--;
    } else if (item->type == ITEM_MANA_POTION) {
        int old_mana = game->player.mana;
        game->player.mana += item->value;
        if (game->player.mana > game->player.max_mana) {
            game->player.mana = game->player.max_mana;
        }
        int restored = game->player.mana - old_mana;
        sprintf(msg, "Used mana potion! Restored %d mana.", restored);
        set_message(game, msg);
        
        item->exists = false;
        for (int i = index; i < game->player.inventory_count - 1; i++) {
            game->player.inventory[i] = game->player.inventory[i + 1];
        }
        game->player.inventory_count--;
    }
}

void equip_from_inventory(Game *game, int index) {
    if (index < 0 || index >= game->player.inventory_count) return;
    
    InventoryItem *item = &game->player.inventory[index];
    if (!item->exists) return;
    
    if (item->type == ITEM_WEAPON) {
        equip_weapon(game, item->weapon_type, item->value, item->name);
        item->exists = false;
        for (int i = index; i < game->player.inventory_count - 1; i++) {
            game->player.inventory[i] = game->player.inventory[i + 1];
        }
        game->player.inventory_count--;
    } else if (item->type == ITEM_ARMOR) {
        equip_armor(game, item->armor_type, item->value, item->name);
        item->exists = false;
        for (int i = index; i < game->player.inventory_count - 1; i++) {
            game->player.inventory[i] = game->player.inventory[i + 1];
        }
        game->player.inventory_count--;
    }
}

void drop_inventory_item(Game *game, int index) {
    if (index < 0 || index >= game->player.inventory_count) return;
    
    InventoryItem *item = &game->player.inventory[index];
    if (!item->exists) return;
    
    char msg[100];
    sprintf(msg, "Dropped %s", item->name);
    set_message(game, msg);
    
    item->exists = false;
    for (int i = index; i < game->player.inventory_count - 1; i++) {
        game->player.inventory[i] = game->player.inventory[i + 1];
    }
    game->player.inventory_count--;
}