#ifndef FOV_H
#define FOV_H

#include "types.h"

void calculate_fov(Game *game);
void clear_fov(Game *game);
bool is_visible(Game *game, int x, int y);
bool is_explored(Game *game, int x, int y);
bool has_line_of_sight(Game *game, int x0, int y0, int x1, int y1);

#endif