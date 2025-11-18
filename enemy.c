#include "enemy.h"
#include "utils.h"
#include "player.h"
#include "status.h"
#include <stdlib.h>
#include <string.h>

void create_enemy(Enemy *enemy, EnemyType type, int x, int y, int level) {
    enemy->pos.x = x;
    enemy->pos.y = y;
    enemy->alive = true;
    enemy->type = type;
    enemy->move_counter = 0;
    enemy->status_effect_count = 0;
    
    float scale = 1.0 + (level - 1) * 0.3;
    
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
        
        // Safe rooms have no enemies!
        if (room.room_type == ROOM_SAFE) {
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
            
            // Monster dens have more dangerous enemies
            if (room.room_type == ROOM_MONSTER_DEN) {
                if (type_roll < 30) {
                    type = ENEMY_GOBLIN;
                } else if (type_roll < 60) {
                    type = ENEMY_BAT;
                } else {
                    type = ENEMY_ORC;
                }
            } else {
                if (type_roll < 50) {
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
            int damage = enemy->damage - get_damage_reduction(&game->player);
            if (damage < 0) damage = 0;
            game->player.hp -= damage;
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
            int damage = enemy->damage - get_damage_reduction(&game->player);
            if (damage < 0) damage = 0;
            game->player.hp -= damage;
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
        int damage = enemy->damage - get_damage_reduction(&game->player);
        if (damage < 0) damage = 0;
        game->player.hp -= damage;
        
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
            int damage = enemy->damage - get_damage_reduction(&game->player);
            if (damage < 0) damage = 0;
            game->player.hp -= damage;
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
        }
    }
}