#include "ability.h"
#include "utils.h"
#include "player.h"
#include "status.h"
#include <stdio.h>
#include <stdlib.h>

void cast_fireball(Game *game) {
    const int MANA_COST = 25;
    const int DAMAGE = 30;
    const float RADIUS = 3.0;
    
    if (game->player.mana < MANA_COST) {
        set_message(game, "Not enough mana for Fireball! (Costs 25 mana)");
        return;
    }
    
    game->player.mana -= MANA_COST;
    int enemies_hit = 0;
    
    for (int i = 0; i < game->enemy_count; i++) {
        Enemy *enemy = &game->enemies[i];
        if (!enemy->alive) continue;
        
        float dist = distance(game->player.pos.x, game->player.pos.y,
                             enemy->pos.x, enemy->pos.y);
        
        if (dist <= RADIUS) {
            enemy->hp -= DAMAGE;
            
            // Apply burn status effect (3 turns, 5 damage per turn)
            apply_status_to_enemy(game, enemy, STATUS_BURN, 3, 5);
            
            if (enemy->hp <= 0) {
                enemy->alive = false;
                game->player.gold += enemy->gold_drop;
                gain_xp(game, enemy->xp_value);
            }
            enemies_hit++;
        }
    }
    
    char msg[100];
    sprintf(msg, "FIREBALL! Hit %d enemies for %d damage each! (+ BURN)", enemies_hit, DAMAGE);
    set_message(game, msg);
}

void cast_heal(Game *game) {
    const int MANA_COST = 20;
    const int HEAL_AMOUNT = 50;
    
    if (game->player.mana < MANA_COST) {
        set_message(game, "Not enough mana for Heal! (Costs 20 mana)");
        return;
    }
    
    game->player.mana -= MANA_COST;
    int old_hp = game->player.hp;
    game->player.hp += HEAL_AMOUNT;
    if (game->player.hp > game->player.max_hp) {
        game->player.hp = game->player.max_hp;
    }
    
    // Remove poison and burn status effects
    remove_status_effect(game->player.status_effects, &game->player.status_effect_count, STATUS_POISON);
    remove_status_effect(game->player.status_effects, &game->player.status_effect_count, STATUS_BURN);
    
    int healed = game->player.hp - old_hp;
    char msg[100];
    sprintf(msg, "HEAL! Restored %d HP! Cured poison/burn!", healed);
    set_message(game, msg);
}

void cast_teleport(Game *game) {
    const int MANA_COST = 30;
    
    if (game->player.mana < MANA_COST) {
        set_message(game, "Not enough mana for Teleport! (Costs 30 mana)");
        return;
    }
    
    Room *current_room = NULL;
    for (int i = 0; i < game->room_count; i++) {
        Room *room = &game->rooms[i];
        if (game->player.pos.x >= room->x && 
            game->player.pos.x < room->x + room->width &&
            game->player.pos.y >= room->y && 
            game->player.pos.y < room->y + room->height) {
            current_room = room;
            break;
        }
    }
    
    if (current_room == NULL) {
        set_message(game, "Can't teleport from corridor!");
        return;
    }
    
    game->player.mana -= MANA_COST;
    
    for (int attempts = 0; attempts < 50; attempts++) {
        int new_x = current_room->x + 1 + rand() % (current_room->width - 2);
        int new_y = current_room->y + 1 + rand() % (current_room->height - 2);
        
        if (is_walkable(game, new_x, new_y) && 
            get_enemy_at(game, new_x, new_y) == NULL) {
            game->player.pos.x = new_x;
            game->player.pos.y = new_y;
            set_message(game, "TELEPORT! Escaped to safe spot!");
            return;
        }
    }
    
    set_message(game, "Teleport failed - no safe spot found!");
}

void cast_dash(Game *game, int dx, int dy) {
    const int MANA_COST = 15;
    const int DASH_DISTANCE = 5;
    
    if (game->player.mana < MANA_COST) {
        set_message(game, "Not enough mana for Dash! (Costs 15 mana)");
        return;
    }
    
    game->player.mana -= MANA_COST;
    
    for (int i = 0; i < DASH_DISTANCE; i++) {
        int new_x = game->player.pos.x + dx;
        int new_y = game->player.pos.y + dy;
        
        if (!is_walkable(game, new_x, new_y) || 
            get_enemy_at(game, new_x, new_y) != NULL) {
            break;
        }
        
        game->player.pos.x = new_x;
        game->player.pos.y = new_y;
    }
    
    set_message(game, "DASH! Quick movement!");
}

void use_ability(Game *game, AbilityType ability) {
    switch (ability) {
        case ABILITY_FIREBALL:
            cast_fireball(game);
            break;
        case ABILITY_HEAL:
            cast_heal(game);
            break;
        case ABILITY_TELEPORT:
            cast_teleport(game);
            break;
        case ABILITY_DASH:
            break;
    }
}