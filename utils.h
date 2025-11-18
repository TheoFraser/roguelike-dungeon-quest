#ifndef UTILS_H
#define UTILS_H

#include "types.h"

float distance(int x1, int y1, int x2, int y2);
void set_message(Game *game, const char *msg);
bool is_walkable(Game *game, int x, int y);
Enemy* get_enemy_at(Game *game, int x, int y);
Item* get_item_at(Game *game, int x, int y);
bool find_valid_spawn_position(Game *game, Room *room, int *out_x, int *out_y);

#endif