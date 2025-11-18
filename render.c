#include "render.h"
#include "utils.h"
#include "player.h"
#include "fov.h"
#include "status.h"
#include <ncurses.h>

void draw_abilities(Game *game) {
    clear();
    
    mvprintw(0, 0, "=== ABILITIES (Press 'c' to close, ESC to cancel) ===");
    mvprintw(1, 0, "Mana: %d/%d", game->player.mana, game->player.max_mana);
    mvprintw(2, 0, "");
    
    mvprintw(4, 0, "[F] Fireball - 25 mana");
    mvprintw(5, 0, "    AOE damage (30 dmg) to all enemies within 3 tiles");
    mvprintw(6, 0, "");
    
    mvprintw(7, 0, "[H] Heal - 20 mana");
    mvprintw(8, 0, "    Restore 50 HP");
    mvprintw(9, 0, "");
    
    mvprintw(10, 0, "[T] Teleport - 30 mana");
    mvprintw(11, 0, "    Escape to random location in current room");
    mvprintw(12, 0, "");
    
    mvprintw(13, 0, "[Shift + Arrow] Dash - 15 mana");
    mvprintw(14, 0, "    Quick movement 5 tiles in direction");
    mvprintw(15, 0, "");
    
    mvprintw(17, 0, "Tip: Collect mana potions (M) to restore mana!");
    mvprintw(18, 0, "Tip: Mana regenerates slowly each turn (+1 per turn)");
    
    refresh();
}

void draw_inventory(Game *game) {
    clear();
    
    mvprintw(0, 0, "=== INVENTORY (Press 'i' to close, ESC to cancel) ===");
    mvprintw(1, 0, "Items: %d/%d", game->player.inventory_count, MAX_INVENTORY);
    mvprintw(2, 0, "");
    
    if (game->player.inventory_count == 0) {
        mvprintw(4, 0, "Your inventory is empty!");
    } else {
        mvprintw(4, 0, "Press number to USE/EQUIP, 'd' + number to DROP:");
        mvprintw(5, 0, "");
        
        for (int i = 0; i < game->player.inventory_count; i++) {
            InventoryItem *item = &game->player.inventory[i];
            if (!item->exists) continue;
            
            if (item->type == ITEM_HEALTH_POTION) {
                mvprintw(7 + i, 0, "[%d] Health Potion (heals %d HP)", i + 1, item->value);
            } else if (item->type == ITEM_MANA_POTION) {
                mvprintw(7 + i, 0, "[%d] Mana Potion (restores %d mana)", i + 1, item->value);
            } else if (item->type == ITEM_WEAPON) {
                mvprintw(7 + i, 0, "[%d] %s (+%d damage)", i + 1, item->name, item->value);
            } else if (item->type == ITEM_ARMOR) {
                mvprintw(7 + i, 0, "[%d] %s (+%d defense)", i + 1, item->name, item->value);
            }
        }
    }
    
    mvprintw(MAP_HEIGHT - 2, 0, "Currently Equipped:");
    mvprintw(MAP_HEIGHT - 1, 0, "Weapon: %s | Armor: %s", 
             game->player.equipment.weapon_name, game->player.equipment.armor_name);
    
    refresh();
}

void draw_health_bar(int y, int x, int current, int max, int width) {
    float percent = (float)current / max;
    int filled = (int)(percent * width);
    
    mvaddch(y, x, '[');
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            attron(COLOR_PAIR(2) | A_BOLD);  // Red for health
            mvaddch(y, x + 1 + i, '=');
            attroff(COLOR_PAIR(2) | A_BOLD);
        } else {
            mvaddch(y, x + 1 + i, ' ');
        }
    }
    mvaddch(y, x + width + 1, ']');
    
    mvprintw(y, x + width + 3, "%d/%d", current, max);
}

void draw_mana_bar(int y, int x, int current, int max, int width) {
    float percent = (float)current / max;
    int filled = (int)(percent * width);
    
    mvaddch(y, x, '[');
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            attron(COLOR_PAIR(10) | A_BOLD);  // Blue for mana
            mvaddch(y, x + 1 + i, '=');
            attroff(COLOR_PAIR(10) | A_BOLD);
        } else {
            mvaddch(y, x + 1 + i, ' ');
        }
    }
    mvaddch(y, x + width + 1, ']');
    
    mvprintw(y, x + width + 3, "%d/%d", current, max);
}

void draw_mini_map(Game *game, int start_y, int start_x) {
    int mini_width = 20;
    int mini_height = 10;
    
    mvprintw(start_y, start_x, "=== Mini-Map ===");
    
    // Calculate scale
    float scale_x = (float)MAP_WIDTH / mini_width;
    float scale_y = (float)MAP_HEIGHT / mini_height;
    
    for (int my = 0; my < mini_height; my++) {
        for (int mx = 0; mx < mini_width; mx++) {
            int map_x = (int)(mx * scale_x);
            int map_y = (int)(my * scale_y);
            
            if (map_x >= MAP_WIDTH) map_x = MAP_WIDTH - 1;
            if (map_y >= MAP_HEIGHT) map_y = MAP_HEIGHT - 1;
            
            char symbol = ' ';
            int color = 0;
            bool use_dim = false;
            
            // Check if player is near this position (within 1 scaled tile)
            bool is_player = false;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int check_x = (int)((mx + dx * 0.5) * scale_x);
                    int check_y = (int)((my + dy * 0.5) * scale_y);
                    if (check_x >= 0 && check_x < MAP_WIDTH && 
                        check_y >= 0 && check_y < MAP_HEIGHT &&
                        check_x == game->player.pos.x && 
                        check_y == game->player.pos.y) {
                        is_player = true;
                        break;
                    }
                }
                if (is_player) break;
            }
            
            if (is_player) {
                symbol = '@';
                color = 1;  // Cyan
            }
            // Check if explored
            else if (game->explored[map_y][map_x]) {
                if (game->map[map_y][map_x] == TILE_FLOOR || 
                    game->map[map_y][map_x] == TILE_STAIRS) {
                    symbol = '.';
                    use_dim = true;
                } else if (game->map[map_y][map_x] == TILE_WALL) {
                    symbol = '#';
                    use_dim = true;
                } else {
                    symbol = ' ';
                }
            } else {
                symbol = ' ';
            }
            
            // Draw the symbol
            if (color > 0) {
                attron(COLOR_PAIR(color) | A_BOLD);
                mvaddch(start_y + 1 + my, start_x + mx, symbol);
                attroff(COLOR_PAIR(color) | A_BOLD);
            } else if (use_dim) {
                attron(A_DIM);
                mvaddch(start_y + 1 + my, start_x + mx, symbol);
                attroff(A_DIM);
            } else {
                mvaddch(start_y + 1 + my, start_x + mx, symbol);
            }
        }
    }
}

void draw_message_log(Game *game, int start_y, int start_x) {
    mvprintw(start_y, start_x, "=== Recent Messages ===");
    
    for (int i = 0; i < game->message_log_count && i < 5; i++) {
        if (i == 0) {
            attron(A_BOLD);
        } else {
            attron(A_DIM);
        }
        mvprintw(start_y + 1 + i, start_x, "%s", game->message_log[i]);
        if (i == 0) {
            attroff(A_BOLD);
        } else {
            attroff(A_DIM);
        }
    }
}

void draw_game(Game *game) {
    clear();
    
    // Draw main game area
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // CRITICAL FIX: Draw player FIRST, always visible!
            if (x == game->player.pos.x && y == game->player.pos.y) {
                attron(COLOR_PAIR(1) | A_BOLD);
                mvaddch(y, x, TILE_PLAYER);
                attroff(COLOR_PAIR(1) | A_BOLD);
                continue;
            }
            
            bool visible = is_visible(game, x, y);
            bool explored = is_explored(game, x, y);
            
            // Only draw explored tiles
            if (!explored) {
                mvaddch(y, x, ' ');
                continue;
            }
            
            // Draw stairs (if visible or explored)
            if (x == game->stairs_pos.x && y == game->stairs_pos.y) {
                if (visible) {
                    attron(COLOR_PAIR(7) | A_BOLD);
                } else {
                    attron(COLOR_PAIR(7) | A_DIM);
                }
                mvaddch(y, x, TILE_STAIRS);
                if (visible) {
                    attroff(COLOR_PAIR(7) | A_BOLD);
                } else {
                    attroff(COLOR_PAIR(7) | A_DIM);
                }
                continue;
            }
            
            // Draw items (only if visible)
            if (visible) {
                Item *item = get_item_at(game, x, y);
                if (item != NULL) {
                    if (item->type == ITEM_HEALTH_POTION) {
                        attron(COLOR_PAIR(6) | A_BOLD);
                        mvaddch(y, x, 'H');
                        attroff(COLOR_PAIR(6) | A_BOLD);
                    } else if (item->type == ITEM_MANA_POTION) {
                        attron(COLOR_PAIR(10) | A_BOLD);
                        mvaddch(y, x, 'M');
                        attroff(COLOR_PAIR(10) | A_BOLD);
                    } else if (item->type == ITEM_GOLD) {
                        attron(COLOR_PAIR(4) | A_BOLD);
                        mvaddch(y, x, '$');
                        attroff(COLOR_PAIR(4) | A_BOLD);
                    } else if (item->type == ITEM_WEAPON) {
                        attron(COLOR_PAIR(8) | A_BOLD);
                        mvaddch(y, x, '/');
                        attroff(COLOR_PAIR(8) | A_BOLD);
                    } else if (item->type == ITEM_ARMOR) {
                        attron(COLOR_PAIR(9) | A_BOLD);
                        mvaddch(y, x, '[');
                        attroff(COLOR_PAIR(9) | A_BOLD);
                    }
                    continue;
                }
            }
            
            // Draw enemies (only if visible) with health bars
            if (visible) {
                Enemy *enemy = get_enemy_at(game, x, y);
                if (enemy != NULL) {
                    attron(COLOR_PAIR(enemy->color_pair) | A_BOLD);
                    mvaddch(y, x, enemy->symbol);
                    attroff(COLOR_PAIR(enemy->color_pair) | A_BOLD);
                    
                    // Draw status effects above enemy if has any
                    if (enemy->status_effect_count > 0 && y > 0) {
                        for (int s = 0; s < enemy->status_effect_count && s < 3; s++) {
                            StatusEffect *effect = &enemy->status_effects[s];
                            attron(COLOR_PAIR(get_status_color(effect->type)));
                            mvaddch(y - 1, x - 1 + s, get_status_symbol(effect->type));
                            attroff(COLOR_PAIR(get_status_color(effect->type)));
                        }
                    }
                    // Draw health bar above enemy if hurt (below status effects)
                    else if (enemy->hp < enemy->max_hp && y > 0) {
                        float hp_percent = (float)enemy->hp / enemy->max_hp;
                        int bar_length = 5;
                        int filled = (int)(hp_percent * bar_length);
                        
                        for (int i = 0; i < bar_length; i++) {
                            if (i < filled) {
                                attron(COLOR_PAIR(2));
                                mvaddch(y - 1, x - 2 + i, '=');
                                attroff(COLOR_PAIR(2));
                            } else {
                                mvaddch(y - 1, x - 2 + i, '-');
                            }
                        }
                    }
                    continue;
                }
            }
            
            // Draw map tiles (dimmed if not visible)
            if (visible) {
                mvaddch(y, x, game->map[y][x]);
            } else {
                attron(A_DIM);
                mvaddch(y, x, game->map[y][x]);
                attroff(A_DIM);
            }
        }
    }
    
    // Enhanced status display with bars
    int status_y = MAP_HEIGHT;
    mvprintw(status_y, 0, "HP: ");
    draw_health_bar(status_y, 4, game->player.hp, game->player.max_hp, 15);
    
    mvprintw(status_y, 30, "Mana: ");
    draw_mana_bar(status_y, 36, game->player.mana, game->player.max_mana, 12);
    
    // Display player status effects
    if (game->player.status_effect_count > 0) {
        mvprintw(status_y, 52, "Status: ");
        for (int i = 0; i < game->player.status_effect_count; i++) {
            StatusEffect *effect = &game->player.status_effects[i];
            attron(COLOR_PAIR(get_status_color(effect->type)) | A_BOLD);
            mvprintw(status_y, 60 + i * 3, "%c%d", get_status_symbol(effect->type), effect->duration);
            attroff(COLOR_PAIR(get_status_color(effect->type)) | A_BOLD);
        }
    }
    
    int total_dmg = get_total_damage(&game->player);
    int defense = get_damage_reduction(&game->player);
    
    mvprintw(status_y + 1, 0, "LVL:%d XP:%d/%d | Gold:%d | Dmg:%d | Def:%d | Floor:%d", 
             game->player.level, game->player.xp, game->player.xp_to_next_level,
             game->player.gold, total_dmg, defense, game->dungeon_level);
    
    mvprintw(status_y + 2, 0, "Weapon: %s | Armor: %s | Inv:%d/%d(i) | Abilities(c) | Save(F5) Load(F9)", 
             game->player.equipment.weapon_name, game->player.equipment.armor_name,
             game->player.inventory_count, MAX_INVENTORY);
    
    // Draw message log on the right side
    draw_message_log(game, 0, MAP_WIDTH + 2);
    
    // Draw mini-map
    draw_mini_map(game, 7, MAP_WIDTH + 2);
    
    refresh();
}