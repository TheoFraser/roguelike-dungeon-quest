#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "game.h"
#include "save.h"
#include "fov.h"
#include "shop.h"

void draw_class_selection_screen() {
    clear();
    
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(2, 25, "================================");
    mvprintw(3, 25, "    CHOOSE YOUR CLASS          ");
    mvprintw(4, 25, "================================");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    mvprintw(7, 20, "[1] WARRIOR - The Tank");
    attron(COLOR_PAIR(2));
    mvprintw(8, 22, "+30 HP, +5 damage, -10 mana");
    mvprintw(9, 22, "10%% crit, 5%% dodge");
    mvprintw(10, 22, "Great for beginners!");
    attroff(COLOR_PAIR(2));
    
    mvprintw(12, 20, "[2] MAGE - The Glass Cannon");
    attron(COLOR_PAIR(5));
    mvprintw(13, 22, "+30 mana, -20 HP");
    mvprintw(14, 22, "Spells cost 25%% less mana");
    mvprintw(15, 22, "15%% crit, 5%% dodge");
    mvprintw(16, 22, "High risk, high reward!");
    attroff(COLOR_PAIR(5));
    
    mvprintw(18, 20, "[3] ROGUE - The Agile Striker");
    attron(COLOR_PAIR(3));
    mvprintw(19, 22, "+2 damage, +10 mana");
    mvprintw(20, 22, "25%% crit chance!");
    mvprintw(21, 22, "20%% dodge chance!");
    mvprintw(22, 22, "For advanced players!");
    attroff(COLOR_PAIR(3));
    
    mvprintw(25, 28, "Press 1, 2, or 3 to choose");
    
    refresh();
}

void draw_title_screen() {
    clear();
    
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(2, 20, "=========================================");
    mvprintw(3, 20, "                                         ");
    mvprintw(4, 20, "        ROGUELIKE DUNGEON QUEST          ");
    mvprintw(5, 20, "                                         ");
    mvprintw(6, 20, "=========================================");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    mvprintw(9, 30, "Welcome, adventurer!");
    mvprintw(11, 25, "Your quest: Delve deep into the");
    mvprintw(12, 25, "dungeon and defeat the bosses!");
    
    mvprintw(15, 30, "[N] New Game");
    
    if (save_exists()) {
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(16, 30, "[F9] Load Game");
        attroff(COLOR_PAIR(3) | A_BOLD);
    } else {
        attron(A_DIM);
        mvprintw(16, 30, "[F9] Load Game (no save found)");
        attroff(A_DIM);
    }
    
    mvprintw(17, 30, "[Q] Quit");
    
    mvprintw(20, 20, "Controls: WASD/Arrows=Move, I=Inventory");
    mvprintw(21, 20, "          C=Abilities, F5=Save, F9=Load");
    
    refresh();
}

int main() {
    srand(time(NULL));
    
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);
    init_pair(8, COLOR_BLUE, COLOR_BLACK);
    init_pair(9, COLOR_GREEN, COLOR_BLACK);
    init_pair(10, COLOR_BLUE, COLOR_BLACK);
    
    Game game;
    bool start_game = false;
    
    // Show title screen
    while (!start_game) {
        draw_title_screen();
        
        int ch = getch();
        
        switch (ch) {
            case 'n':
            case 'N':
                // Show class selection screen
                {
                    CharacterClass chosen_class = CLASS_WARRIOR;
                    bool class_chosen = false;
                    
                    while (!class_chosen) {
                        draw_class_selection_screen();
                        int class_ch = getch();
                        
                        switch (class_ch) {
                            case '1':
                                chosen_class = CLASS_WARRIOR;
                                class_chosen = true;
                                break;
                            case '2':
                                chosen_class = CLASS_MAGE;
                                class_chosen = true;
                                break;
                            case '3':
                                chosen_class = CLASS_ROGUE;
                                class_chosen = true;
                                break;
                        }
                    }
                    
                    // Initialize game then apply class
                    init_game(&game);
                    choose_character_class(&game.player, chosen_class);
                    start_game = true;
                }
                break;
                
            case KEY_F(9):  // F9 to load
                // Load game
                if (save_exists()) {
                    if (load_game(&game)) {
                        calculate_fov(&game);
                        start_game = true;
                    } else {
                        mvprintw(23, 25, "Failed to load game! Press any key...");
                        refresh();
                        getch();
                    }
                } else {
                    mvprintw(23, 25, "No save file found! Press any key...");
                    refresh();
                    getch();
                }
                break;
                
            case 'q':
            case 'Q':
                endwin();
                return 0;
        }
    }
    
    // Run game
    game_loop(&game);
    
    endwin();
    return 0;
}