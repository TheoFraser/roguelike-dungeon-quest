#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"

void create_enemy(Enemy *enemy, EnemyType type, int x, int y, int level);
void spawn_enemies(Game *game);
void move_enemies(Game *game);
void move_goblin(Game *game, Enemy *enemy);
void move_orc(Game *game, Enemy *enemy);
void move_bat(Game *game, Enemy *enemy);
void move_boss(Game *game, Enemy *enemy);

#endif