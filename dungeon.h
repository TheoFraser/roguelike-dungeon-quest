#ifndef DUNGEON_H
#define DUNGEON_H

#include "types.h"

void generate_dungeon(Game *game);
void create_room(Game *game, Room room);
void create_corridor(Game *game, int x1, int y1, int x2, int y2);
void spawn_stairs(Game *game);

#endif