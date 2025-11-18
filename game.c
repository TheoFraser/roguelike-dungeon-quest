#include "game.h"
#include "utils.h"
#include "player.h"
#include "enemy.h"
#include "item.h"
#include "dungeon.h"
#include "ability.h"
#include "inventory.h"
#include "render.h"
#include "fov.h"
#include "save.h"
#include "status.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

void init_game(Game *game) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            game->map[y][x] = TILE_WALL;
            game->visible[y][x] = false;
            game->explored[y][x] = false;
        }
    }
    
    game->player.hp = 100;
    game->player.max_hp = 100;
    game->player.mana = 50;
    game->player.max_mana = 50;
    game->player.base_damage = 10;
    game->player.gold = 0;
    game->player.xp = 0;
    game->player.level = 1;
    game->player.xp_to_next_level = 50;
    game->player.inventory_count = 0;
    game->player.equipment.weapon = WEAPON_NONE;
    game->player.equipment.armor = ARMOR_NONE;
    game->player.equipment.weapon_damage = 0;
    game->player.equipment.armor_defense = 0;
    strcpy(game->player.equipment.weapon_name, "Fists");
    strcpy(game->player.equipment.armor_name, "None");
    game->player.status_effect_count = 0;
    
    game->room_count = 0;
    game->enemy_count = 0;
    game->item_count = 0;
    game->turn_count = 0;
    game->dungeon_level = 1;
    game->show_inventory = false;
    game->show_abilities = false;
    game->message_log_count = 0;
    
    set_message(game, "Welcome! Find treasures and defeat monsters. Press 'c' for abilities!");
    
    generate_dungeon(game);
    spawn_enemies(game);
    spawn_items(game);
    spawn_stairs(game);
    
    // Calculate initial FOV
    calculate_fov(game);
}

void game_loop(Game *game) {
    bool running = true;
    bool drop_mode = false;
    
    while (running) {
        if (game->show_abilities) {
            draw_abilities(game);
            
            int ch = getch();
            
            if (ch == 'c' || ch == 'C' || ch == 27) {
                game->show_abilities = false;
            } else if (ch == 'f' || ch == 'F') {
                cast_fireball(game);
                game->show_abilities = false;
                move_enemies(game);
                game->turn_count++;
                calculate_fov(game);
            } else if (ch == 'h' || ch == 'H') {
                cast_heal(game);
                game->show_abilities = false;
                move_enemies(game);
                game->turn_count++;
            } else if (ch == 't' || ch == 'T') {
                cast_teleport(game);
                game->show_abilities = false;
                move_enemies(game);
                game->turn_count++;
                calculate_fov(game);
            }
        } else if (game->show_inventory) {
            draw_inventory(game);
            
            int ch = getch();
            
            if (ch == 'i' || ch == 27) {
                game->show_inventory = false;
                drop_mode = false;
            } else if (ch == 'd') {
                drop_mode = true;
            } else if (ch >= '1' && ch <= '9') {
                int index = ch - '1';
                if (drop_mode) {
                    drop_inventory_item(game, index);
                    drop_mode = false;
                } else {
                    if (index < game->player.inventory_count) {
                        InventoryItem *item = &game->player.inventory[index];
                        if (item->type == ITEM_HEALTH_POTION || item->type == ITEM_MANA_POTION) {
                            use_inventory_item(game, index);
                        } else {
                            equip_from_inventory(game, index);
                        }
                    }
                }
                game->show_inventory = false;
            }
        } else {
            draw_game(game);
            
            if (game->player.hp <= 0) {
                mvprintw(MAP_HEIGHT + 3, 0, 
                         "GAME OVER! Level %d, Floor %d, %d gold. Press any key.", 
                         game->player.level, game->dungeon_level, game->player.gold);
                refresh();
                getch();
                break;
            }
            
            int ch = getch();
            bool player_moved = false;
            
            switch (ch) {
                case KEY_UP:
                case 'w':
                case 'W':
                case 'k':
                case 'K':
                    move_player(game, 0, -1);
                    player_moved = true;
                    break;
                case KEY_DOWN:
                case 's':
                case 'S':
                case 'j':
                case 'J':
                    move_player(game, 0, 1);
                    player_moved = true;
                    break;
                case KEY_LEFT:
                case 'a':
                case 'A':
                case 'h':
                    move_player(game, -1, 0);
                    player_moved = true;
                    break;
                case KEY_RIGHT:
                case 'd':
                case 'D':
                case 'l':
                case 'L':
                    move_player(game, 1, 0);
                    player_moved = true;
                    break;
                case KEY_SR:
                    cast_dash(game, 0, -1);
                    player_moved = true;
                    break;
                case KEY_SF:
                    cast_dash(game, 0, 1);
                    player_moved = true;
                    break;
                case KEY_SLEFT:
                    cast_dash(game, -1, 0);
                    player_moved = true;
                    break;
                case KEY_SRIGHT:
                    cast_dash(game, 1, 0);
                    player_moved = true;
                    break;
                case 'i':
                case 'I':
                    game->show_inventory = true;
                    break;
                case 'c':
                case 'C':
                    game->show_abilities = true;
                    break;
                case KEY_F(5):  // F5 for save
                    save_game(game);
                    break;
                case KEY_F(9):  // F9 for load
                    if (load_game(game)) {
                        calculate_fov(game);
                    }
                    break;
                case 'q':
                case 'Q':
                    running = false;
                    break;
            }
            
            if (player_moved) {
                // Check if player is stunned - if so, skip their turn!
                if (has_status_effect(game->player.status_effects, game->player.status_effect_count, STATUS_STUN)) {
                    set_message(game, "You are stunned and cannot move!");
                    // Still update status effects and enemies
                    update_player_status_effects(game);
                    move_enemies(game);
                    game->turn_count++;
                } else {
                    // Normal turn
                    update_player_status_effects(game);
                    move_enemies(game);
                    game->turn_count++;
                    if (game->player.mana < game->player.max_mana) {
                        game->player.mana++;
                    }
                }
                // Recalculate FOV after player moves
                calculate_fov(game);
            }
        }
    }
}