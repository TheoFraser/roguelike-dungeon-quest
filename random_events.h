#ifndef RANDOM_EVENTS_H
#define RANDOM_EVENTS_H

#include "types.h"

// Trigger a random event when entering a new floor
void trigger_random_event(Game *game);

// Apply the effects of the triggered event
void apply_event_effects(Game *game);

// Spawn ambush enemies around the player
void spawn_ambush_enemies(Game *game);

// Spawn a bonus treasure item
void spawn_bonus_item(Game *game);

// Spawn an event merchant
void spawn_event_merchant(Game *game);

// Spawn an event shrine
void spawn_event_shrine(Game *game);

// Modify XP gains if XP bonus event is active
void modify_xp_gain(Game *game, int *xp);

// Clear temporary event effects when changing floors
void clear_floor_events(Game *game);

#endif