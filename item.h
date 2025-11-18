#ifndef ITEM_H
#define ITEM_H

#include "types.h"

void create_weapon_item(Item *item, int x, int y, int dungeon_level);
void create_armor_item(Item *item, int x, int y, int dungeon_level);
void spawn_items(Game *game);

#endif