#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

// Game constants
#define MAP_WIDTH 80
#define MAP_HEIGHT 24
#define MAX_ROOMS 10
#define MAX_ENEMIES 25
#define MAX_ITEMS 20
#define MAX_INVENTORY 20
#define FOV_RADIUS 8

// Tile types
typedef enum {
    TILE_WALL = '#',
    TILE_FLOOR = '.',
    TILE_PLAYER = '@',
    TILE_STAIRS = '>',
    TILE_EMPTY = ' '
} TileType;

// Item types
typedef enum {
    ITEM_NONE,
    ITEM_HEALTH_POTION,
    ITEM_MANA_POTION,
    ITEM_GOLD,
    ITEM_WEAPON,
    ITEM_ARMOR
} ItemType;

// Weapon types
typedef enum {
    WEAPON_NONE,
    WEAPON_DAGGER,
    WEAPON_SWORD,
    WEAPON_AXE,
    WEAPON_GREATSWORD
} WeaponType;

// Armor types
typedef enum {
    ARMOR_NONE,
    ARMOR_LEATHER,
    ARMOR_CHAINMAIL,
    ARMOR_PLATE
} ArmorType;

// Enemy types
typedef enum {
    ENEMY_GOBLIN,
    ENEMY_ORC,
    ENEMY_BAT,
    ENEMY_BOSS
} EnemyType;

// Ability types
typedef enum {
    ABILITY_FIREBALL,
    ABILITY_HEAL,
    ABILITY_TELEPORT,
    ABILITY_DASH
} AbilityType;

// Status effect types
typedef enum {
    STATUS_NONE,
    STATUS_POISON,
    STATUS_BURN,
    STATUS_SLOW,
    STATUS_STUN,
    STATUS_REGENERATION
} StatusEffectType;

// Status effect structure
typedef struct {
    StatusEffectType type;
    int duration;        // Turns remaining
    int power;          // Damage/heal per turn or movement penalty
} StatusEffect;

#define MAX_STATUS_EFFECTS 5

// Room types
typedef enum {
    ROOM_NORMAL,
    ROOM_TREASURE,
    ROOM_MONSTER_DEN,
    ROOM_SAFE,
    ROOM_BOSS
} RoomType;

// Position structure
typedef struct {
    int x;
    int y;
} Position;

// Room structure
typedef struct {
    int x, y;
    int width, height;
    bool is_boss_room;
    RoomType room_type;
} Room;

// Equipment structure
typedef struct {
    WeaponType weapon;
    ArmorType armor;
    int weapon_damage;
    int armor_defense;
    char weapon_name[30];
    char armor_name[30];
} Equipment;

// Inventory item structure
typedef struct {
    ItemType type;
    WeaponType weapon_type;
    ArmorType armor_type;
    int value;
    char name[30];
    bool exists;
} InventoryItem;

// Enemy structure
typedef struct {
    Position pos;
    int hp;
    int max_hp;
    int damage;
    int gold_drop;
    int xp_value;
    bool alive;
    char symbol;
    EnemyType type;
    int color_pair;
    int move_counter;
    char name[20];
    StatusEffect status_effects[MAX_STATUS_EFFECTS];
    int status_effect_count;
} Enemy;

// Item structure (on ground)
typedef struct {
    Position pos;
    ItemType type;
    int value;
    bool collected;
    WeaponType weapon_type;
    ArmorType armor_type;
    int bonus;
    char name[30];
} Item;

// Player structure
typedef struct {
    Position pos;
    int hp;
    int max_hp;
    int mana;
    int max_mana;
    int base_damage;
    int gold;
    int xp;
    int level;
    int xp_to_next_level;
    Equipment equipment;
    InventoryItem inventory[MAX_INVENTORY];
    int inventory_count;
    StatusEffect status_effects[MAX_STATUS_EFFECTS];
    int status_effect_count;
} Player;

// Game state
typedef struct {
    char map[MAP_HEIGHT][MAP_WIDTH];
    bool visible[MAP_HEIGHT][MAP_WIDTH];      // Currently visible
    bool explored[MAP_HEIGHT][MAP_WIDTH];     // Has been seen before
    Player player;
    Room rooms[MAX_ROOMS];
    int room_count;
    Enemy enemies[MAX_ENEMIES];
    int enemy_count;
    Item items[MAX_ITEMS];
    int item_count;
    Position stairs_pos;
    char message[100];
    char message_log[5][100];                 // Last 5 messages
    int message_log_count;
    int turn_count;
    int dungeon_level;
    bool show_inventory;
    bool show_abilities;
} Game;

#endif