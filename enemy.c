#include "enemy.h"
#include "utils.h"
#include "player.h"
#include "status.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Helper function for enemy attacks with dodge check
void enemy_attack_player(Game *game, Enemy *enemy) {
    // DODGE CHECK
    bool dodged = (rand() % 100) < game->player.dodge_chance;
    char msg[100];
    
    if (dodged) {
        sprintf(msg, "You dodged %s's attack!", enemy->name);
        set_message(game, msg);
    } else {
        int damage = enemy->damage - get_damage_reduction(&game->player);
        if (damage < 0) damage = 0;
        game->player.hp -= damage;
        
        sprintf(msg, "%s hits you for %d damage!", enemy->name, damage);
        set_message(game, msg);
    }
}

void create_enemy(Enemy *enemy, EnemyType type, int x, int y, int level) {
    enemy->pos.x = x;
    enemy->pos.y = y;
    enemy->alive = true;
    enemy->type = type;
    enemy->move_counter = 0;
    enemy->status_effect_count = 0;
    enemy->is_elite = false;
    strcpy(enemy->elite_title, "");
    
    float scale = 1.0 + (level - 1) * 0.3;
    
    // ELITE ENEMY CHANCE (7% chance, not for bosses)
    if (type != ENEMY_BOSS && rand() % 100 < 7) {
        enemy->is_elite = true;
        
        // Choose random elite title
        const char *titles[] = {"Swift", "Venomous", "Iron", "Savage", "Ancient"};
        strcpy(enemy->elite_title, titles[rand() % 5]);
        
        // Elite bonuses
        scale *= 2.0;  // 2x stats!
    }
    
    switch (type) {
        case ENEMY_GOBLIN:
            strcpy(enemy->name, "Goblin");
            enemy->symbol = 'g';
            enemy->hp = (int)(15 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(5 * scale);
            enemy->gold_drop = 5 + rand() % 10 + (level * 2);
            enemy->xp_value = 10 + (level * 3);
            enemy->color_pair = 2;
            break;
            
        case ENEMY_ORC:
            strcpy(enemy->name, "Orc");
            enemy->symbol = 'O';
            enemy->hp = (int)(40 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(15 * scale);
            enemy->gold_drop = 15 + rand() % 20 + (level * 5);
            enemy->xp_value = 25 + (level * 8);
            enemy->color_pair = 3;
            break;
            
        case ENEMY_BAT:
            strcpy(enemy->name, "Bat");
            enemy->symbol = 'b';
            enemy->hp = (int)(10 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(3 * scale);
            enemy->gold_drop = 3 + rand() % 7 + level;
            enemy->xp_value = 5 + (level * 2);
            enemy->color_pair = 4;
            break;
            
        case ENEMY_BOSS:
            strcpy(enemy->name, "Boss");
            enemy->symbol = 'B';
            enemy->hp = (int)(100 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(20 * scale);
            enemy->gold_drop = 50 + rand() % 50 + (level * 20);
            enemy->xp_value = 100 + (level * 30);
            enemy->color_pair = 5;
            break;
            
        case ENEMY_ARCHER:
            strcpy(enemy->name, "Archer");
            enemy->symbol = 'A';
            enemy->hp = (int)(20 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(12 * scale);
            enemy->gold_drop = 8 + rand() % 12 + (level * 3);
            enemy->xp_value = 15 + (level * 5);
            enemy->color_pair = 6;  // White
            break;
            
        case ENEMY_SUMMONER:
            strcpy(enemy->name, "Summoner");
            enemy->symbol = 'S';
            enemy->hp = (int)(25 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(8 * scale);
            enemy->gold_drop = 12 + rand() % 15 + (level * 4);
            enemy->xp_value = 20 + (level * 6);
            enemy->color_pair = 5;  // Magenta
            break;
            
        case ENEMY_HEALER:
            strcpy(enemy->name, "Healer");
            enemy->symbol = 'H';
            enemy->hp = (int)(18 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(5 * scale);
            enemy->gold_drop = 10 + rand() % 10 + (level * 3);
            enemy->xp_value = 18 + (level * 5);
            enemy->color_pair = 3;  // Green
            break;
            
        case ENEMY_TELEPORTER:
            strcpy(enemy->name, "Blinker");
            enemy->symbol = 'T';
            enemy->hp = (int)(15 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(10 * scale);
            enemy->gold_drop = 8 + rand() % 15 + (level * 3);
            enemy->xp_value = 16 + (level * 5);
            enemy->color_pair = 7;  // Cyan
            break;
            
        case ENEMY_TANK:
            strcpy(enemy->name, "Tank");
            enemy->symbol = 'K';
            enemy->hp = (int)(60 * scale);
            enemy->max_hp = enemy->hp;
            enemy->damage = (int)(18 * scale);
            enemy->gold_drop = 15 + rand() % 20 + (level * 5);
            enemy->xp_value = 30 + (level * 8);
            enemy->color_pair = 2;  // Red
            break;
    }
    
    // Add elite title to name and boost loot
    if (enemy->is_elite) {
        char full_name[50];
        snprintf(full_name, sizeof(full_name), "%s %s", enemy->elite_title, enemy->name);
        strcpy(enemy->name, full_name);
        
        // Elite enemies drop MUCH better loot
        enemy->gold_drop = (int)(enemy->gold_drop * 2.5);
        enemy->xp_value = (int)(enemy->xp_value * 2.0);
    }
}

void spawn_enemies(Game *game) {
    for (int i = 0; i < game->room_count && game->enemy_count < MAX_ENEMIES; i++) {
        Room room = game->rooms[i];
        
        // Skip first room (starting room)
        if (i == 0) continue;
        
        // Handle boss room
        if (room.is_boss_room || room.room_type == ROOM_BOSS) {
            Enemy *enemy = &game->enemies[game->enemy_count];
            create_enemy(enemy, ENEMY_BOSS, 
                        room.x + room.width / 2, 
                        room.y + room.height / 2,
                        game->dungeon_level);
            game->enemy_count++;
            continue;
        }
        
        // Safe rooms and shop rooms have no enemies!
        if (room.room_type == ROOM_SAFE || room.room_type == ROOM_SHOP) {
            continue;
        }
        
        // Determine number of enemies based on room type
        int num_enemies;
        switch (room.room_type) {
            case ROOM_MONSTER_DEN:
                num_enemies = 5 + rand() % 4;  // 5-8 enemies
                break;
            case ROOM_TREASURE:
                num_enemies = 2 + rand() % 3;  // 2-4 enemies (guarding treasure)
                break;
            default:
                num_enemies = 1 + rand() % 4;  // 1-4 enemies
                break;
        }
        
        for (int j = 0; j < num_enemies && game->enemy_count < MAX_ENEMIES; j++) {
            Enemy *enemy = &game->enemies[game->enemy_count];
            int x = room.x + 1 + rand() % (room.width - 2);
            int y = room.y + 1 + rand() % (room.height - 2);
            
            int type_roll = rand() % 100;
            EnemyType type;
            
            // Special enemies start appearing on floor 2+
            bool can_spawn_special = game->dungeon_level >= 2;
            int special_chance = game->dungeon_level * 5;  // 5% per floor
            if (special_chance > 40) special_chance = 40;  // Cap at 40%
            
            // Monster dens have more dangerous enemies + more special types
            if (room.room_type == ROOM_MONSTER_DEN) {
                if (can_spawn_special && type_roll < special_chance) {
                    // Spawn special enemy
                    int special_roll = rand() % 100;
                    if (special_roll < 20) {
                        type = ENEMY_ARCHER;
                    } else if (special_roll < 35) {
                        type = ENEMY_SUMMONER;
                    } else if (special_roll < 50) {
                        type = ENEMY_HEALER;
                    } else if (special_roll < 70) {
                        type = ENEMY_TELEPORTER;
                    } else {
                        type = ENEMY_TANK;
                    }
                } else if (type_roll < 30 + special_chance) {
                    type = ENEMY_GOBLIN;
                } else if (type_roll < 60 + special_chance) {
                    type = ENEMY_BAT;
                } else {
                    type = ENEMY_ORC;
                }
            } else {
                // Normal rooms
                if (can_spawn_special && type_roll < special_chance / 2) {
                    // Lower chance for special enemies in normal rooms
                    int special_roll = rand() % 100;
                    if (special_roll < 25) {
                        type = ENEMY_ARCHER;
                    } else if (special_roll < 45) {
                        type = ENEMY_SUMMONER;
                    } else if (special_roll < 60) {
                        type = ENEMY_HEALER;
                    } else if (special_roll < 80) {
                        type = ENEMY_TELEPORTER;
                    } else {
                        type = ENEMY_TANK;
                    }
                } else if (type_roll < 50) {
                    type = ENEMY_GOBLIN;
                } else if (type_roll < 75) {
                    type = ENEMY_BAT;
                } else {
                    type = ENEMY_ORC;
                }
            }
            
            create_enemy(enemy, type, x, y, game->dungeon_level);
            game->enemy_count++;
        }
    }
}

void move_goblin(Game *game, Enemy *enemy) {
    for (int i = 0; i < 2; i++) {
        int dx = 0, dy = 0;
        if (game->player.pos.x > enemy->pos.x) dx = 1;
        else if (game->player.pos.x < enemy->pos.x) dx = -1;
        
        if (game->player.pos.y > enemy->pos.y) dy = 1;
        else if (game->player.pos.y < enemy->pos.y) dy = -1;
        
        int new_x = enemy->pos.x + dx;
        int new_y = enemy->pos.y + dy;
        
        if (new_x == game->player.pos.x && new_y == game->player.pos.y) {
            enemy_attack_player(game, enemy);
            return;
        }
        
        if (is_walkable(game, new_x, new_y) && get_enemy_at(game, new_x, new_y) == NULL) {
            enemy->pos.x = new_x;
            enemy->pos.y = new_y;
        }
    }
}

void move_orc(Game *game, Enemy *enemy) {
    enemy->move_counter++;
    if (enemy->move_counter % 2 == 0) {
        int dx = 0, dy = 0;
        if (game->player.pos.x > enemy->pos.x) dx = 1;
        else if (game->player.pos.x < enemy->pos.x) dx = -1;
        
        if (game->player.pos.y > enemy->pos.y) dy = 1;
        else if (game->player.pos.y < enemy->pos.y) dy = -1;
        
        int new_x = enemy->pos.x + dx;
        int new_y = enemy->pos.y + dy;
        
        if (new_x == game->player.pos.x && new_y == game->player.pos.y) {
            enemy_attack_player(game, enemy);
            return;
        }
        
        if (is_walkable(game, new_x, new_y) && get_enemy_at(game, new_x, new_y) == NULL) {
            enemy->pos.x = new_x;
            enemy->pos.y = new_y;
        }
    }
}

void move_bat(Game *game, Enemy *enemy) {
    int direction = rand() % 4;
    int dx = 0, dy = 0;
    
    switch (direction) {
        case 0: dy = -1; break;
        case 1: dy = 1; break;
        case 2: dx = -1; break;
        case 3: dx = 1; break;
    }
    
    int new_x = enemy->pos.x + dx;
    int new_y = enemy->pos.y + dy;
    
    if (new_x == game->player.pos.x && new_y == game->player.pos.y) {
        enemy_attack_player(game, enemy);
        
        // Bats have 30% chance to poison
        if (rand() % 100 < 30) {
            apply_status_to_player(game, STATUS_POISON, 5, 3);
        }
        return;
    }
    
    if (is_walkable(game, new_x, new_y) && get_enemy_at(game, new_x, new_y) == NULL) {
        enemy->pos.x = new_x;
        enemy->pos.y = new_y;
    }
}

void move_boss(Game *game, Enemy *enemy) {
    enemy->move_counter++;
    if (enemy->move_counter % 3 == 0) {
        int dx = 0, dy = 0;
        if (game->player.pos.x > enemy->pos.x) dx = 1;
        else if (game->player.pos.x < enemy->pos.x) dx = -1;
        
        if (game->player.pos.y > enemy->pos.y) dy = 1;
        else if (game->player.pos.y < enemy->pos.y) dy = -1;
        
        int new_x = enemy->pos.x + dx;
        int new_y = enemy->pos.y + dy;
        
        if (new_x == game->player.pos.x && new_y == game->player.pos.y) {
            enemy_attack_player(game, enemy);
            return;
        }
        
        if (is_walkable(game, new_x, new_y) && get_enemy_at(game, new_x, new_y) == NULL) {
            enemy->pos.x = new_x;
            enemy->pos.y = new_y;
        }
    }
}

void move_enemies(Game *game) {
    for (int i = 0; i < game->enemy_count; i++) {
        Enemy *enemy = &game->enemies[i];
        if (!enemy->alive) continue;
        
        // Update status effects first
        update_enemy_status_effects(game, enemy);
        
        // Skip movement if stunned
        if (has_status_effect(enemy->status_effects, enemy->status_effect_count, STATUS_STUN)) {
            continue;
        }
        
        // Move slower if slowed (skip every other turn)
        if (has_status_effect(enemy->status_effects, enemy->status_effect_count, STATUS_SLOW)) {
            enemy->move_counter++;
            if (enemy->move_counter % 2 == 0) {
                continue;  // Skip this turn
            }
        }
        
        switch (enemy->type) {
            case ENEMY_GOBLIN: move_goblin(game, enemy); break;
            case ENEMY_ORC: move_orc(game, enemy); break;
            case ENEMY_BAT: move_bat(game, enemy); break;
            case ENEMY_BOSS: move_boss(game, enemy); break;
            case ENEMY_ARCHER: move_archer(game, enemy); break;
            case ENEMY_SUMMONER: move_summoner(game, enemy); break;
            case ENEMY_HEALER: move_healer(game, enemy); break;
            case ENEMY_TELEPORTER: move_teleporter(game, enemy); break;
            case ENEMY_TANK: move_tank(game, enemy); break;
        }
    }
}

// ARCHER: Ranged attacker - keeps distance and shoots
void move_archer(Game *game, Enemy *enemy) {
    float dist = distance(game->player.pos.x, game->player.pos.y, 
                         enemy->pos.x, enemy->pos.y);
    
    // If player is in range (3-6 tiles), shoot!
    if (dist >= 3.0 && dist <= 6.0) {
        enemy->move_counter++;
        if (enemy->move_counter % 2 == 0) {  // Shoot every other turn
            enemy_attack_player(game, enemy);
        }
        return;
    }
    
    // If too close, back away
    if (dist < 3.0) {
        int dx = 0, dy = 0;
        if (game->player.pos.x > enemy->pos.x) dx = -1;
        else if (game->player.pos.x < enemy->pos.x) dx = 1;
        
        if (game->player.pos.y > enemy->pos.y) dy = -1;
        else if (game->player.pos.y < enemy->pos.y) dy = 1;
        
        int new_x = enemy->pos.x + dx;
        int new_y = enemy->pos.y + dy;
        
        if (is_walkable(game, new_x, new_y) && get_enemy_at(game, new_x, new_y) == NULL) {
            enemy->pos.x = new_x;
            enemy->pos.y = new_y;
        }
    }
    // If too far, move closer
    else if (dist > 6.0) {
        move_goblin(game, enemy);  // Use normal movement
    }
}

// SUMMONER: Spawns minions when damaged
void move_summoner(Game *game, Enemy *enemy) {
    // Check if below 50% HP and hasn't summoned yet
    if (enemy->hp < enemy->max_hp / 2 && enemy->move_counter == 0) {
        enemy->move_counter = 1;  // Flag that we've summoned
        
        // Try to spawn 2 goblins nearby
        int spawned = 0;
        for (int attempts = 0; attempts < 10 && spawned < 2 && game->enemy_count < MAX_ENEMIES; attempts++) {
            int dx = (rand() % 3) - 1;  // -1, 0, or 1
            int dy = (rand() % 3) - 1;
            int spawn_x = enemy->pos.x + dx;
            int spawn_y = enemy->pos.y + dy;
            
            if (is_walkable(game, spawn_x, spawn_y) && 
                get_enemy_at(game, spawn_x, spawn_y) == NULL &&
                (spawn_x != game->player.pos.x || spawn_y != game->player.pos.y)) {
                
                Enemy *minion = &game->enemies[game->enemy_count];
                create_enemy(minion, ENEMY_GOBLIN, spawn_x, spawn_y, game->dungeon_level);
                game->enemy_count++;
                spawned++;
            }
        }
        
        if (spawned > 0) {
            set_message(game, "Summoner calls for reinforcements!");
        }
    }
    
    // Move like an orc (slow but steady)
    move_orc(game, enemy);
}

// HEALER: Heals nearby damaged enemies
void move_healer(Game *game, Enemy *enemy) {
    enemy->move_counter++;
    
    // Try to heal every 3 turns
    if (enemy->move_counter % 3 == 0) {
        // Find a damaged enemy within 4 tiles
        Enemy *target = NULL;
        float best_need = 0;
        
        for (int i = 0; i < game->enemy_count; i++) {
            Enemy *other = &game->enemies[i];
            if (!other->alive || other == enemy) continue;
            if (other->hp >= other->max_hp) continue;
            
            float dist = distance(enemy->pos.x, enemy->pos.y, other->pos.x, other->pos.y);
            if (dist <= 4.0) {
                float need = (float)(other->max_hp - other->hp) / other->max_hp;
                if (need > best_need) {
                    best_need = need;
                    target = other;
                }
            }
        }
        
        if (target != NULL) {
            int heal_amount = enemy->damage * 2;  // Heals for 2x its damage
            target->hp += heal_amount;
            if (target->hp > target->max_hp) target->hp = target->max_hp;
            
            char msg[100];
            sprintf(msg, "Healer restores %d HP to %s!", heal_amount, target->name);
            set_message(game, msg);
            return;
        }
    }
    
    // Move slowly toward player
    if (enemy->move_counter % 2 == 0) {
        move_goblin(game, enemy);
    }
}

// TELEPORTER: Blinks around randomly, hard to pin down
void move_teleporter(Game *game, Enemy *enemy) {
    enemy->move_counter++;
    
    // Teleport every 4 turns
    if (enemy->move_counter % 4 == 0) {
        // Try to teleport within 5 tiles of player
        for (int attempt = 0; attempt < 20; attempt++) {
            int new_x = game->player.pos.x + (rand() % 11) - 5;  // -5 to +5
            int new_y = game->player.pos.y + (rand() % 11) - 5;
            
            if (is_walkable(game, new_x, new_y) && 
                get_enemy_at(game, new_x, new_y) == NULL &&
                (new_x != game->player.pos.x || new_y != game->player.pos.y)) {
                
                enemy->pos.x = new_x;
                enemy->pos.y = new_y;
                set_message(game, "Blinker teleports!");
                return;
            }
        }
    }
    
    // Attack if adjacent
    int dx = 0, dy = 0;
    if (game->player.pos.x > enemy->pos.x) dx = 1;
    else if (game->player.pos.x < enemy->pos.x) dx = -1;
    
    if (game->player.pos.y > enemy->pos.y) dy = 1;
    else if (game->player.pos.y < enemy->pos.y) dy = -1;
    
    int new_x = enemy->pos.x + dx;
    int new_y = enemy->pos.y + dy;
    
    if (new_x == game->player.pos.x && new_y == game->player.pos.y) {
        enemy_attack_player(game, enemy);
    }
}

// TANK: Slow but incredibly tough and hits hard
void move_tank(Game *game, Enemy *enemy) {
    enemy->move_counter++;
    
    // Moves very slowly (every 3 turns)
    if (enemy->move_counter % 3 != 0) {
        return;
    }
    
    // Move toward player
    int dx = 0, dy = 0;
    if (game->player.pos.x > enemy->pos.x) dx = 1;
    else if (game->player.pos.x < enemy->pos.x) dx = -1;
    
    if (game->player.pos.y > enemy->pos.y) dy = 1;
    else if (game->player.pos.y < enemy->pos.y) dy = -1;
    
    int new_x = enemy->pos.x + dx;
    int new_y = enemy->pos.y + dy;
    
    if (new_x == game->player.pos.x && new_y == game->player.pos.y) {
        enemy_attack_player(game, enemy);
        return;
    }
    
    if (is_walkable(game, new_x, new_y) && get_enemy_at(game, new_x, new_y) == NULL) {
        enemy->pos.x = new_x;
        enemy->pos.y = new_y;
    }
}