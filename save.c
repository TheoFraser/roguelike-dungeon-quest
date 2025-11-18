#include "save.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

bool save_game(Game *game) {
    FILE *file = fopen(SAVE_FILE, "wb");
    if (!file) {
        set_message(game, "Failed to save game!");
        return false;
    }
    
    // Write a version marker
    int version = 1;
    fwrite(&version, sizeof(int), 1, file);
    
    // Write entire game state
    fwrite(game, sizeof(Game), 1, file);
    
    fclose(file);
    set_message(game, "Game saved successfully!");
    return true;
}

bool load_game(Game *game) {
    FILE *file = fopen(SAVE_FILE, "rb");
    if (!file) {
        return false;
    }
    
    // Read version marker
    int version;
    if (fread(&version, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    // Check version compatibility
    if (version != 1) {
        fclose(file);
        return false;
    }
    
    // Read entire game state
    if (fread(game, sizeof(Game), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    fclose(file);
    set_message(game, "Game loaded successfully!");
    return true;
}

bool save_exists(void) {
    FILE *file = fopen(SAVE_FILE, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

void delete_save(void) {
    remove(SAVE_FILE);
}