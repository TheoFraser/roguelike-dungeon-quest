#ifndef SAVE_H
#define SAVE_H

#include "types.h"

#define SAVE_FILE "roguelike_save.dat"
#define MAX_MESSAGE_LOG 5

// Save game to file
bool save_game(Game *game);

// Load game from file
bool load_game(Game *game);

// Check if save file exists
bool save_exists(void);

// Delete save file
void delete_save(void);

#endif