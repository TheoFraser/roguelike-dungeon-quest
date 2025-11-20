#ifndef SHOP_H
#define SHOP_H

#include "types.h"

// Shop item structure
typedef struct {
    char name[50];
    char description[100];
    int price;
    ShopItemType type;
    int value;  // HP for potions, damage for weapons, etc.
    WeaponType weapon_type;
    ArmorType armor_type;
} ShopItem;

// Initialize player class at game start
void choose_character_class(Player *player, CharacterClass class);

// Apply class bonuses
void apply_class_bonuses(Player *player);

// Shop functions
void generate_shop_items(ShopItem items[5], int dungeon_level);
void display_shop(Game *game, ShopItem items[5]);
bool purchase_item(Game *game, ShopItem *item);

// Class names for display
const char* get_class_name(CharacterClass class);

#endif