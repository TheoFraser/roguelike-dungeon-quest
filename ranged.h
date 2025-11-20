#ifndef RANGED_H
#define RANGED_H

#include "types.h"

// Initialize a projectile
void init_projectile(Projectile *proj, int x, int y, int dx, int dy, int damage, bool is_player, int range);

// Fire an arrow in the specified direction
void fire_arrow(Game *game, int dx, int dy);

// Update all active projectiles (movement, collisions)
void update_projectiles(Game *game);

// Enter firing mode (aiming interface)
void enter_firing_mode(Game *game);

// Exit firing mode
void exit_firing_mode(Game *game);

// Rotate aim direction clockwise
void rotate_aim_clockwise(Game *game);

// Set aim direction directly
void set_aim_direction(Game *game, int dx, int dy);

// Get direction name for display
char* get_direction_name(int dx, int dy);

// Add arrows to player inventory
void add_arrows(Game *game, int amount);

// Check if bow is equipped
bool is_bow_equipped(Game *game);

#endif