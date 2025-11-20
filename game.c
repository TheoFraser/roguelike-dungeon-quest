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
#include "dungeon_features.h"
#include "shop.h"
#include "ranged.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
    game->player.equipment.weapon_effect = WEAPON_EFFECT_NONE;
    game->player.equipment.weapon_effect_chance = 0;
    strcpy(game->player.equipment.weapon_name, "Fists");
    strcpy(game->player.equipment.armor_name, "None");
    game->player.status_effect_count = 0;
    
    // Initialize class system (will be set in main.c before init_game)
    game->player.class = CLASS_WARRIOR;  // Default
    game->player.crit_chance = 10;
    game->player.dodge_chance = 5;
    game->player.spell_cost_modifier = 1.0;
    game->player.arrows = 10;  // Start with 10 arrows
    
    game->room_count = 0;
    game->enemy_count = 0;
    game->item_count = 0;
    game->object_count = 0;
    game->hazard_count = 0;
    game->projectile_count = 0;
    game->in_firing_mode = false;
    game->turn_count = 0;
    game->dungeon_level = 1;
    game->current_biome = BIOME_DUNGEON;
    game->show_inventory = false;
    game->show_abilities = false;
    game->message_log_count = 0;
    game->floors_since_merchant = 0;  // Track for guaranteed merchant
    game->current_event = EVENT_NONE;
    game->event_active = false;
    game->event_duration = 0;
    
    set_message(game, "Welcome! Find treasures and defeat monsters. Press 'c' for abilities!");
    
    generate_dungeon(game);
    spawn_enemies(game);
    spawn_items(game);
    spawn_interactive_objects(game);
    spawn_hazards(game);
    spawn_stairs(game);
    
    // Calculate initial FOV
    calculate_fov(game);
}

void game_loop(Game *game) {
    bool running = true;
    bool drop_mode = false;
    bool in_shop = false;
    ShopItem shop_items[5];
    InteractiveObject *current_merchant = NULL;
    
    while (running) {
        // Check for nearby merchant (but not if we're already in shop!)
        if (!in_shop) {
            current_merchant = NULL;
        }
        if (!game->show_abilities && !game->show_inventory && !in_shop) {
            for (int i = 0; i < game->object_count; i++) {
                InteractiveObject *obj = &game->objects[i];
                if (obj->type == OBJ_MERCHANT) {
                    float dist = distance(game->player.pos.x, game->player.pos.y,
                                         obj->pos.x, obj->pos.y);
                    if (dist <= 1.5) {  // Adjacent or on same tile
                        current_merchant = obj;
                        break;
                    }
                }
            }
        }
        
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
        } else if (game->in_firing_mode) {
            // FIRING MODE - Aiming interface
            draw_game(game);
            
            // Show aiming reticle
            int aim_x = game->player.pos.x + game->aim_dx * 3;
            int aim_y = game->player.pos.y + game->aim_dy * 3;
            if (aim_x >= 0 && aim_x < MAP_WIDTH && aim_y >= 0 && aim_y < MAP_HEIGHT) {
                attron(COLOR_PAIR(3) | A_BOLD);
                mvaddch(aim_y, aim_x, 'X');
                attroff(COLOR_PAIR(3) | A_BOLD);
            }
            
            mvprintw(MAP_HEIGHT + 3, 0, "AIM: %s | Arrows: %d | Arrow keys=aim, ENTER=fire, ESC=cancel",
                    get_direction_name(game->aim_dx, game->aim_dy), game->player.arrows);
            refresh();
            
            int ch = getch();
            if (ch == 27) {  // ESC
                exit_firing_mode(game);
            } else if (ch == 10 || ch == KEY_ENTER) {  // ENTER
                fire_arrow(game, game->aim_dx, game->aim_dy);
                game->in_firing_mode = false;
                move_enemies(game);
                update_projectiles(game);
                game->turn_count++;
                calculate_fov(game);
            } else if (ch == KEY_UP || ch == 'w' || ch == 'W') {
                set_aim_direction(game, 0, -1);
            } else if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
                set_aim_direction(game, 0, 1);
            } else if (ch == KEY_LEFT || ch == 'a' || ch == 'A') {
                set_aim_direction(game, -1, 0);
            } else if (ch == KEY_RIGHT || ch == 'd' || ch == 'D') {
                set_aim_direction(game, 1, 0);
            }
        } else if (in_shop && current_merchant != NULL) {
            // SHOP UI
            clear();
            mvprintw(0, 0, "=== MERCHANT'S SHOP - Gold: %d ===", game->player.gold);
            mvprintw(1, 0, "\"Welcome, friend! Take a look at my wares!\"");
            mvprintw(2, 0, "Press number to buy, ESC or M to close");
            
            for (int i = 0; i < 5; i++) {
                int color = (i < 2) ? 3 : (i < 4) ? 4 : 5;
                attron(COLOR_PAIR(color));
                
                mvprintw(4 + i * 2, 2, "[%d] %s - %dg", 
                         i + 1, shop_items[i].name, shop_items[i].price);
                mvprintw(5 + i * 2, 6, "%s", shop_items[i].description);
                
                attroff(COLOR_PAIR(color));
            }
            
            refresh();
            
            int ch = getch();
            if (ch == 27 || ch == 'm' || ch == 'M') {
                in_shop = false;
            } else if (ch >= '1' && ch <= '5') {
                purchase_item(game, &shop_items[ch - '1']);
            }
        } else {
            draw_game(game);
            
            // Show merchant prompt if nearby
            if (current_merchant != NULL) {
                mvprintw(MAP_HEIGHT + 4, 0, "[E] Talk to Merchant | [M] Open Shop");
            }
            
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
                case 'f':
                case 'F':
                    enter_firing_mode(game);
                    break;
                case 'm':
                case 'M':
                    // Open shop - search for nearby merchant
                    {
                        InteractiveObject *nearby_merchant = NULL;
                        for (int i = 0; i < game->object_count; i++) {
                            InteractiveObject *obj = &game->objects[i];
                            if (obj->type == OBJ_MERCHANT) {
                                float dist = distance(game->player.pos.x, game->player.pos.y,
                                                     obj->pos.x, obj->pos.y);
                                if (dist <= 2.0) {  // Within 2 tiles
                                    nearby_merchant = obj;
                                    break;
                                }
                            }
                        }
                        
                        if (nearby_merchant != NULL) {
                            // Generate shop items if first time
                            if (!nearby_merchant->opened) {
                                generate_shop_items(shop_items, game->dungeon_level);
                                nearby_merchant->opened = true;
                            }
                            current_merchant = nearby_merchant;
                            in_shop = true;
                        } else {
                            set_message(game, "No merchant nearby. Stand next to the 'M' symbol.");
                        }
                    }
                    break;
                case 'e':
                case 'E':
                    // Interact with adjacent objects
                    {
                        bool interacted = false;
                        // Check player's position first
                        InteractiveObject *obj = get_object_at(game, game->player.pos.x, game->player.pos.y);
                        if (obj != NULL) {
                            interact_with_object(game, game->player.pos.x, game->player.pos.y);
                            interacted = true;
                        } else {
                            // Check all 4 adjacent tiles
                            int check_positions[4][2] = {{0,-1}, {0,1}, {-1,0}, {1,0}};
                            for (int i = 0; i < 4; i++) {
                                int check_x = game->player.pos.x + check_positions[i][0];
                                int check_y = game->player.pos.y + check_positions[i][1];
                                obj = get_object_at(game, check_x, check_y);
                                if (obj != NULL) {
                                    interact_with_object(game, check_x, check_y);
                                    interacted = true;
                                    break;
                                }
                            }
                        }
                        if (!interacted) {
                            set_message(game, "Nothing to interact with nearby.");
                        }
                    }
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
                    process_hazard_damage(game);  // Check for environmental hazards
                    move_enemies(game);
                    update_projectiles(game);  // Update arrows/projectiles
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