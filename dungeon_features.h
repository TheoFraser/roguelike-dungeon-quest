#ifndef DUNGEON_FEATURES_H
#define DUNGEON_FEATURES_H

#include "types.h"

// Spawn interactive objects in rooms
void spawn_interactive_objects(Game *game);

// Spawn environmental hazards
void spawn_hazards(Game *game);

// Get interactive object at position
InteractiveObject* get_object_at(Game *game, int x, int y);

// Get hazard at position
EnvironmentalHazard* get_hazard_at(Game *game, int x, int y);

// Interact with object at position
void interact_with_object(Game *game, int x, int y);

// Process environmental hazard damage
void process_hazard_damage(Game *game);

// Choose biome for dungeon level
BiomeType choose_biome(int dungeon_level);

// Apply biome properties to room
void apply_biome_to_room(Game *game, Room *room);

#endif