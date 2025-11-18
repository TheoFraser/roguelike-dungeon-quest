#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "game.h"
#include "save.h"
#include "fov.h"

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
                // Start new game
                init_game(&game);
                start_game = true;
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