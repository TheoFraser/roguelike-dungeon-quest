#include "ranged.h"
#include "utils.h"
#include "player.h"
#include "status.h"
#include <stdio.h>
#include <stdlib.h>

void init_projectile(Projectile *proj, int x, int y, int dx, int dy, int damage, bool is_player, int range) {
    proj->pos.x = x;
    proj->pos.y = y;
    proj->dx = dx;
    proj->dy = dy;
    proj->damage = damage;
    proj->active = true;
    proj->symbol = '-';
    if (dx != 0 && dy == 0) proj->symbol = '-';  // Horizontal
    if (dx == 0 && dy != 0) proj->symbol = '|';  // Vertical
    if (dx != 0 && dy != 0) proj->symbol = '*';  // Diagonal
    proj->color_pair = is_player ? 3 : 2;  // Green for player, red for enemy
    proj->range_remaining = range;
    proj->is_player_projectile = is_player;
}

void fire_arrow(Game *game, int dx, int dy) {
    // Check if player has bow equipped
    if (game->player.equipment.weapon != WEAPON_BOW) {
        set_message(game, "You need a bow equipped to shoot arrows!");
        return;
    }
    
    // Check if player has arrows
    if (game->player.arrows <= 0) {
        set_message(game, "Out of arrows! Find or buy more!");
        return;
    }
    
    // Consume arrow
    game->player.arrows--;
    
    // Create projectile
    if (game->projectile_count < 20) {
        int start_x = game->player.pos.x + dx;
        int start_y = game->player.pos.y + dy;
        
        int damage = get_total_damage(&game->player);
        
        // Check for crit
        bool is_crit = (rand() % 100) < game->player.crit_chance;
        if (is_crit) {
            damage *= 2;
            set_message(game, "CRITICAL SHOT!");
        }
        
        Projectile *proj = &game->projectiles[game->projectile_count];
        init_projectile(proj, start_x, start_y, dx, dy, damage, true, 8);
        game->projectile_count++;
        
        char msg[100];
        snprintf(msg, sizeof(msg), "You fire an arrow! (%d arrows left)", game->player.arrows);
        set_message(game, msg);
    }
}

void update_projectiles(Game *game) {
    for (int i = 0; i < game->projectile_count; i++) {
        Projectile *proj = &game->projectiles[i];
        if (!proj->active) continue;
        
        // Move projectile
        int new_x = proj->pos.x + proj->dx;
        int new_y = proj->pos.y + proj->dy;
        
        // Check bounds
        if (new_x < 0 || new_x >= MAP_WIDTH || new_y < 0 || new_y >= MAP_HEIGHT) {
            proj->active = false;
            continue;
        }
        
        // Check wall collision
        if (!is_walkable(game, new_x, new_y)) {
            proj->active = false;
            continue;
        }
        
        // Check enemy collision (if player projectile)
        if (proj->is_player_projectile) {
            Enemy *enemy = get_enemy_at(game, new_x, new_y);
            if (enemy != NULL) {
                // Hit enemy!
                enemy->hp -= proj->damage;
                
                char msg[100];
                if (enemy->hp <= 0) {
                    enemy->alive = false;
                    game->player.gold += enemy->gold_drop;
                    gain_xp(game, enemy->xp_value);
                    snprintf(msg, sizeof(msg), "Arrow kills %s! (+%d gold, +%d XP)", 
                            enemy->name, enemy->gold_drop, enemy->xp_value);
                } else {
                    snprintf(msg, sizeof(msg), "Arrow hits %s for %d! HP: %d/%d", 
                            enemy->name, proj->damage, enemy->hp, enemy->max_hp);
                }
                set_message(game, msg);
                
                proj->active = false;
                continue;
            }
        }
        
        // Move projectile
        proj->pos.x = new_x;
        proj->pos.y = new_y;
        proj->range_remaining--;
        
        // Deactivate if out of range
        if (proj->range_remaining <= 0) {
            proj->active = false;
        }
    }
    
    // Compact projectile array (remove inactive)
    int write_idx = 0;
    for (int read_idx = 0; read_idx < game->projectile_count; read_idx++) {
        if (game->projectiles[read_idx].active) {
            if (write_idx != read_idx) {
                game->projectiles[write_idx] = game->projectiles[read_idx];
            }
            write_idx++;
        }
    }
    game->projectile_count = write_idx;
}

void enter_firing_mode(Game *game) {
    if (game->player.equipment.weapon != WEAPON_BOW) {
        set_message(game, "You need a bow equipped to enter firing mode!");
        return;
    }
    
    if (game->player.arrows <= 0) {
        set_message(game, "Out of arrows! Find or buy more!");
        return;
    }
    
    game->in_firing_mode = true;
    game->aim_dx = 0;
    game->aim_dy = -1;  // Default aim up
    set_message(game, "FIRING MODE: Use arrow keys to aim, ENTER to fire, ESC to cancel");
}

void exit_firing_mode(Game *game) {
    game->in_firing_mode = false;
    set_message(game, "Firing mode cancelled.");
}

void rotate_aim_clockwise(Game *game) {
    // Rotate aim direction clockwise
    if (game->aim_dx == 0 && game->aim_dy == -1) {  // Up
        game->aim_dx = 1; game->aim_dy = -1;  // Up-right
    } else if (game->aim_dx == 1 && game->aim_dy == -1) {  // Up-right
        game->aim_dx = 1; game->aim_dy = 0;  // Right
    } else if (game->aim_dx == 1 && game->aim_dy == 0) {  // Right
        game->aim_dx = 1; game->aim_dy = 1;  // Down-right
    } else if (game->aim_dx == 1 && game->aim_dy == 1) {  // Down-right
        game->aim_dx = 0; game->aim_dy = 1;  // Down
    } else if (game->aim_dx == 0 && game->aim_dy == 1) {  // Down
        game->aim_dx = -1; game->aim_dy = 1;  // Down-left
    } else if (game->aim_dx == -1 && game->aim_dy == 1) {  // Down-left
        game->aim_dx = -1; game->aim_dy = 0;  // Left
    } else if (game->aim_dx == -1 && game->aim_dy == 0) {  // Left
        game->aim_dx = -1; game->aim_dy = -1;  // Up-left
    } else if (game->aim_dx == -1 && game->aim_dy == -1) {  // Up-left
        game->aim_dx = 0; game->aim_dy = -1;  // Up
    }
}

void set_aim_direction(Game *game, int dx, int dy) {
    game->aim_dx = dx;
    game->aim_dy = dy;
}

char* get_direction_name(int dx, int dy) {
    if (dx == 0 && dy == -1) return "UP";
    if (dx == 1 && dy == -1) return "UP-RIGHT";
    if (dx == 1 && dy == 0) return "RIGHT";
    if (dx == 1 && dy == 1) return "DOWN-RIGHT";
    if (dx == 0 && dy == 1) return "DOWN";
    if (dx == -1 && dy == 1) return "DOWN-LEFT";
    if (dx == -1 && dy == 0) return "LEFT";
    if (dx == -1 && dy == -1) return "UP-LEFT";
    return "UNKNOWN";
}

void add_arrows(Game *game, int amount) {
    game->player.arrows += amount;
    char msg[100];
    snprintf(msg, sizeof(msg), "Found %d arrows! Total: %d", amount, game->player.arrows);
    set_message(game, msg);
}

bool is_bow_equipped(Game *game) {
    return game->player.equipment.weapon == WEAPON_BOW;
}