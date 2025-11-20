#include "shop.h"
#include "utils.h"
#include "player.h"
#include "inventory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* get_class_name(CharacterClass class) {
    switch (class) {
        case CLASS_WARRIOR: return "Warrior";
        case CLASS_MAGE: return "Mage";
        case CLASS_ROGUE: return "Rogue";
        default: return "Unknown";
    }
}

void choose_character_class(Player *player, CharacterClass class) {
    player->class = class;
    apply_class_bonuses(player);
}

void apply_class_bonuses(Player *player) {
    switch (player->class) {
        case CLASS_WARRIOR:
            // Tank: More HP and damage, less mana
            player->max_hp += 30;
            player->hp += 30;
            player->base_damage += 5;
            player->max_mana -= 10;
            player->mana -= 10;
            if (player->mana < 0) player->mana = 0;
            player->crit_chance = 10;
            player->dodge_chance = 5;
            player->spell_cost_modifier = 1.0;
            break;
            
        case CLASS_MAGE:
            // Glass cannon: More mana, cheaper spells, less HP
            player->max_mana += 30;
            player->mana += 30;
            player->spell_cost_modifier = 0.75;  // 25% cheaper spells
            player->max_hp -= 20;
            player->hp -= 20;
            player->crit_chance = 15;  // High crit for spells
            player->dodge_chance = 5;
            break;
            
        case CLASS_ROGUE:
            // Agile: High crit and dodge, balanced stats
            player->crit_chance = 25;  // 25% crit!
            player->dodge_chance = 20;  // 20% dodge!
            player->base_damage += 2;
            player->max_mana += 10;
            player->mana += 10;
            player->spell_cost_modifier = 1.0;
            break;
    }
}

void generate_shop_items(ShopItem items[5], int dungeon_level) {
    // Item 1: Health Potion (always available)
    strcpy(items[0].name, "Health Potion");
    strcpy(items[0].description, "Restores 50 HP");
    items[0].price = 30;
    items[0].type = SHOP_HEALTH_POTION;
    items[0].value = 50;
    
    // Item 2: Mana Potion (always available)
    strcpy(items[1].name, "Mana Potion");
    strcpy(items[1].description, "Restores 40 mana");
    items[1].price = 25;
    items[1].type = SHOP_MANA_POTION;
    items[1].value = 40;
    
    // Item 3: Random Weapon
    int weapon_tier = rand() % 100 + (dungeon_level * 10);
    if (weapon_tier < 25) {
        items[2].weapon_type = WEAPON_DAGGER;
        items[2].value = 8 + rand() % 5;
        strcpy(items[2].name, "Sharp Dagger");
        items[2].price = 80;
    } else if (weapon_tier < 50) {
        items[2].weapon_type = WEAPON_SWORD;
        items[2].value = 15 + rand() % 8;
        strcpy(items[2].name, "Fine Sword");
        items[2].price = 150;
    } else if (weapon_tier < 70) {
        items[2].weapon_type = WEAPON_AXE;
        items[2].value = 22 + rand() % 10;
        strcpy(items[2].name, "Heavy Axe");
        items[2].price = 220;
    } else if (weapon_tier < 85) {
        items[2].weapon_type = WEAPON_BOW;
        items[2].value = 18 + rand() % 8;
        strcpy(items[2].name, "Hunting Bow");
        items[2].price = 180;
    } else {
        items[2].weapon_type = WEAPON_GREATSWORD;
        items[2].value = 35 + rand() % 15;
        strcpy(items[2].name, "Legendary Greatsword");
        items[2].price = 350;
    }
    sprintf(items[2].description, "+%d damage", items[2].value);
    items[2].type = SHOP_WEAPON;
    
    // Item 4: Random Armor
    int armor_tier = rand() % 100 + (dungeon_level * 10);
    if (armor_tier < 40) {
        items[3].armor_type = ARMOR_LEATHER;
        items[3].value = 3 + rand() % 3;
        strcpy(items[3].name, "Leather Armor");
        items[3].price = 100;
    } else if (armor_tier < 75) {
        items[3].armor_type = ARMOR_CHAINMAIL;
        items[3].value = 7 + rand() % 5;
        strcpy(items[3].name, "Chainmail Armor");
        items[3].price = 200;
    } else {
        items[3].armor_type = ARMOR_PLATE;
        items[3].value = 15 + rand() % 8;
        strcpy(items[3].name, "Plate Armor");
        items[3].price = 350;
    }
    sprintf(items[3].description, "+%d defense", items[3].value);
    items[3].type = SHOP_ARMOR;
    
    // Item 5: Random utility
    int utility_roll = rand() % 3;
    if (utility_roll == 0) {
        strcpy(items[4].name, "Arrow Bundle");
        strcpy(items[4].description, "15 arrows for your bow");
        items[4].price = 20 + (dungeon_level * 5);
        items[4].type = SHOP_HEALTH_POTION;  // Reuse type, check in purchase
        items[4].value = -1;  // Special flag for arrows
    } else if (utility_roll == 1) {
        strcpy(items[4].name, "Vitality Elixir");
        strcpy(items[4].description, "Permanently +20 max HP");
        items[4].price = 100 + (dungeon_level * 20);
        items[4].type = SHOP_ABILITY_SCROLL;
        items[4].value = 20;  // HP bonus
    } else {
        strcpy(items[4].name, "Mystic Elixir");
        strcpy(items[4].description, "Permanently +15 max mana");
        items[4].price = 100 + (dungeon_level * 20);
        items[4].type = SHOP_ABILITY_SCROLL;
        items[4].value = -15;  // Negative = mana bonus
    }
}

bool purchase_item(Game *game, ShopItem *item) {
    if (game->player.gold < item->price) {
        set_message(game, "Not enough gold!");
        return false;
    }
    
    char msg[100];
    game->player.gold -= item->price;
    
    switch (item->type) {
        case SHOP_HEALTH_POTION:
            // Check if it's actually arrows (value == -1)
            if (item->value == -1) {
                game->player.arrows += 15;
                sprintf(msg, "Bought 15 arrows for %d gold! Total: %d", item->price, game->player.arrows);
                set_message(game, msg);
                break;
            }
            
            if (game->player.inventory_count >= MAX_INVENTORY) {
                set_message(game, "Inventory full!");
                game->player.gold += item->price;  // Refund
                return false;
            }
            {
                InventoryItem inv_item;
                inv_item.type = ITEM_HEALTH_POTION;
                inv_item.value = item->value;
                strcpy(inv_item.name, "Health Potion");
                inv_item.exists = true;
                add_to_inventory(game, inv_item);
                sprintf(msg, "Bought Health Potion for %d gold!", item->price);
                set_message(game, msg);
            }
            break;
            
        case SHOP_MANA_POTION:
            if (game->player.inventory_count >= MAX_INVENTORY) {
                set_message(game, "Inventory full!");
                game->player.gold += item->price;  // Refund
                return false;
            }
            {
                InventoryItem inv_item;
                inv_item.type = ITEM_MANA_POTION;
                inv_item.value = item->value;
                strcpy(inv_item.name, "Mana Potion");
                inv_item.exists = true;
                add_to_inventory(game, inv_item);
                sprintf(msg, "Bought Mana Potion for %d gold!", item->price);
                set_message(game, msg);
            }
            break;
            
        case SHOP_WEAPON:
            equip_weapon(game, item->weapon_type, item->value, item->name, 
                        WEAPON_EFFECT_NONE, 0);  // Shop weapons have no special effects
            sprintf(msg, "Bought %s for %d gold!", item->name, item->price);
            set_message(game, msg);
            break;
            
        case SHOP_ARMOR:
            equip_armor(game, item->armor_type, item->value, item->name);
            sprintf(msg, "Bought %s for %d gold!", item->name, item->price);
            set_message(game, msg);
            break;
            
        case SHOP_ABILITY_SCROLL:
            if (item->value > 0) {
                // HP bonus
                game->player.max_hp += item->value;
                game->player.hp += item->value;
                sprintf(msg, "Bought %s! Max HP +%d", item->name, item->value);
            } else {
                // Mana bonus (value is negative)
                int mana_bonus = -item->value;
                game->player.max_mana += mana_bonus;
                game->player.mana += mana_bonus;
                sprintf(msg, "Bought %s! Max Mana +%d", item->name, mana_bonus);
            }
            set_message(game, msg);
            break;
    }
    
    return true;
}

void display_shop(Game *game, ShopItem items[5]) {
    // This will be called from game loop to render shop UI
    // For now, we'll integrate it into the game loop directly
    (void)game;
    (void)items;
}