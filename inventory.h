#ifndef INVENTORY_H
#define INVENTORY_H

#include "types.h"

void add_to_inventory(Game *game, InventoryItem item);
void use_inventory_item(Game *game, int index);
void equip_from_inventory(Game *game, int index);
void drop_inventory_item(Game *game, int index);

#endif