#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

void move_player(Game *game, int dx, int dy);
int get_total_damage(Player *player);
int get_damage_reduction(Player *player);
void gain_xp(Game *game, int xp);
void level_up(Game *game);
void attack_enemy(Game *game, Enemy *enemy);
void pickup_item(Game *game, Item *item);
void equip_weapon(Game *game, WeaponType weapon, int damage, const char *name, WeaponEffect effect, int effect_chance);
void equip_armor(Game *game, ArmorType armor, int defense, const char *name);
void descend_stairs(Game *game);

#endif