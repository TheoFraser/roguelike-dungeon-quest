#ifndef ABILITY_H
#define ABILITY_H

#include "types.h"

void use_ability(Game *game, AbilityType ability);
void cast_fireball(Game *game);
void cast_heal(Game *game);
void cast_teleport(Game *game);
void cast_dash(Game *game, int dx, int dy);

#endif