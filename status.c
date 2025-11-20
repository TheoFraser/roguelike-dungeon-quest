#include "types.h"
#include "status.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

void apply_status_to_player(Game *game, StatusEffectType type, int duration, int power) {
    // Check if already has this effect - if so, refresh duration
    for (int i = 0; i < game->player.status_effect_count; i++) {
        if (game->player.status_effects[i].type == type) {
            game->player.status_effects[i].duration = duration;
            game->player.status_effects[i].power = power;
            char msg[100];
            sprintf(msg, "%s effect refreshed!", get_status_name(type));
            set_message(game, msg);
            return;
        }
    }
    
    // Add new effect if there's room
    if (game->player.status_effect_count < MAX_STATUS_EFFECTS) {
        StatusEffect *effect = &game->player.status_effects[game->player.status_effect_count];
        effect->type = type;
        effect->duration = duration;
        effect->power = power;
        game->player.status_effect_count++;
        
        char msg[100];
        sprintf(msg, "You are %s!", get_status_name(type));
        set_message(game, msg);
    }
}

void apply_status_to_enemy(Game *game __attribute__((unused)), Enemy *enemy, StatusEffectType type, int duration, int power) {
    // Check if already has this effect
    for (int i = 0; i < enemy->status_effect_count; i++) {
        if (enemy->status_effects[i].type == type) {
            enemy->status_effects[i].duration = duration;
            enemy->status_effects[i].power = power;
            return;
        }
    }
    
    // Add new effect if there's room
    if (enemy->status_effect_count < MAX_STATUS_EFFECTS) {
        StatusEffect *effect = &enemy->status_effects[enemy->status_effect_count];
        effect->type = type;
        effect->duration = duration;
        effect->power = power;
        enemy->status_effect_count++;
    }
}

void update_player_status_effects(Game *game) {
    char msg[100];
    
    for (int i = 0; i < game->player.status_effect_count; i++) {
        StatusEffect *effect = &game->player.status_effects[i];
        
        // Apply effect
        switch (effect->type) {
            case STATUS_POISON:
                game->player.hp -= effect->power;
                sprintf(msg, "Poison! -%d HP", effect->power);
                set_message(game, msg);
                break;
                
            case STATUS_BURN:
                game->player.hp -= effect->power;
                sprintf(msg, "Burning! -%d HP", effect->power);
                set_message(game, msg);
                break;
                
            case STATUS_REGENERATION:
                game->player.hp += effect->power;
                if (game->player.hp > game->player.max_hp) {
                    game->player.hp = game->player.max_hp;
                }
                sprintf(msg, "Regenerating! +%d HP", effect->power);
                set_message(game, msg);
                break;
                
            case STATUS_STUN:
                // Stun is handled in movement - just show message
                if (effect->duration > 0) {
                    set_message(game, "You are stunned!");
                }
                break;
                
            case STATUS_SLOW:
                // Slow is handled in movement
                break;
                
            default:
                break;
        }
        
        // Decrease duration
        effect->duration--;
        
        // Remove if expired
        if (effect->duration <= 0) {
            sprintf(msg, "%s effect wore off!", get_status_name(effect->type));
            set_message(game, msg);
            
            // Shift remaining effects down
            for (int j = i; j < game->player.status_effect_count - 1; j++) {
                game->player.status_effects[j] = game->player.status_effects[j + 1];
            }
            game->player.status_effect_count--;
            i--; // Check this slot again since we shifted
        }
    }
}

void update_enemy_status_effects(Game *game __attribute__((unused)), Enemy *enemy) {
    for (int i = 0; i < enemy->status_effect_count; i++) {
        StatusEffect *effect = &enemy->status_effects[i];
        
        // Apply effect
        switch (effect->type) {
            case STATUS_POISON:
                enemy->hp -= effect->power;
                if (enemy->hp <= 0) {
                    enemy->alive = false;
                }
                break;
                
            case STATUS_BURN:
                enemy->hp -= effect->power;
                if (enemy->hp <= 0) {
                    enemy->alive = false;
                }
                break;
                
            case STATUS_REGENERATION:
                enemy->hp += effect->power;
                if (enemy->hp > enemy->max_hp) {
                    enemy->hp = enemy->max_hp;
                }
                break;
                
            case STATUS_STUN:
            case STATUS_SLOW:
                // These are handled in movement
                break;
                
            default:
                break;
        }
        
        // Decrease duration
        effect->duration--;
        
        // Remove if expired
        if (effect->duration <= 0) {
            // Shift remaining effects down
            for (int j = i; j < enemy->status_effect_count - 1; j++) {
                enemy->status_effects[j] = enemy->status_effects[j + 1];
            }
            enemy->status_effect_count--;
            i--; // Check this slot again since we shifted
        }
    }
}

bool has_status_effect(StatusEffect *effects, int count, StatusEffectType type) {
    for (int i = 0; i < count; i++) {
        if (effects[i].type == type) {
            return true;
        }
    }
    return false;
}

void remove_status_effect(StatusEffect *effects, int *count, StatusEffectType type) {
    for (int i = 0; i < *count; i++) {
        if (effects[i].type == type) {
            // Shift remaining effects down
            for (int j = i; j < *count - 1; j++) {
                effects[j] = effects[j + 1];
            }
            (*count)--;
            return;
        }
    }
}

const char* get_status_name(StatusEffectType type) {
    switch (type) {
        case STATUS_POISON: return "Poisoned";
        case STATUS_BURN: return "Burning";
        case STATUS_SLOW: return "Slowed";
        case STATUS_STUN: return "Stunned";
        case STATUS_REGENERATION: return "Regenerating";
        default: return "Unknown";
    }
}

char get_status_symbol(StatusEffectType type) {
    switch (type) {
        case STATUS_POISON: return 'P';
        case STATUS_BURN: return 'F';
        case STATUS_SLOW: return 'S';
        case STATUS_STUN: return 'Z';
        case STATUS_REGENERATION: return 'R';
        default: return '?';
    }
}

int get_status_color(StatusEffectType type) {
    switch (type) {
        case STATUS_POISON: return 3;  // Green
        case STATUS_BURN: return 2;    // Red
        case STATUS_SLOW: return 8;    // Blue
        case STATUS_STUN: return 4;    // Yellow
        case STATUS_REGENERATION: return 3; // Green
        default: return 6;             // White
    }
}