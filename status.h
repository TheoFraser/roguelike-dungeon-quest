#ifndef STATUS_H
#define STATUS_H

// CRITICAL: Include types.h FIRST to get StatusEffect and StatusEffectType definitions
#include "types.h"
#include <stdbool.h>

// Apply a status effect to a target (player or enemy)
void apply_status_to_player(Game *game, StatusEffectType type, int duration, int power);
void apply_status_to_enemy(Game *game, Enemy *enemy, StatusEffectType type, int duration, int power);

// Update all status effects (called each turn)
void update_player_status_effects(Game *game);
void update_enemy_status_effects(Game *game, Enemy *enemy);

// Check if target has a specific status effect
bool has_status_effect(StatusEffect *effects, int count, StatusEffectType type);

// Remove a status effect
void remove_status_effect(StatusEffect *effects, int *count, StatusEffectType type);

// Get status effect name for display
const char* get_status_name(StatusEffectType type);

// Get status effect symbol for display
char get_status_symbol(StatusEffectType type);

// Get status effect color
int get_status_color(StatusEffectType type);

#endif