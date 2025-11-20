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
#define MAX_INTERACTIVE_OBJECTS 15
#define MAX_HAZARDS 20

// Tile types
typedef enum {
    TILE_WALL = '#',
    TILE_FLOOR = '.',
    TILE_PLAYER = '@',
    TILE_STAIRS = '>',
    TILE_EMPTY = ' ',
    TILE_LAVA = '~',
    TILE_SPIKE_TRAP = '^',
    TILE_POISON_GAS = '%',
    TILE_CHEST = 'C',
    TILE_MIMIC = 'M',
    TILE_LEVER = 'L',
    TILE_PRESSURE_PLATE = '_',
    TILE_SHRINE = 'A',
    TILE_SECRET_DOOR = '+',
    TILE_WATER = 'W'
} TileType;

// Item types
typedef enum {
    ITEM_NONE,
    ITEM_HEALTH_POTION,
    ITEM_MANA_POTION,
    ITEM_GOLD,
    ITEM_WEAPON,
    ITEM_ARMOR,
    ITEM_ARROWS        // Arrow ammunition
} ItemType;

// Weapon types
typedef enum {
    WEAPON_NONE,
    WEAPON_DAGGER,
    WEAPON_SWORD,
    WEAPON_AXE,
    WEAPON_GREATSWORD,
    WEAPON_BOW          // Ranged weapon
} WeaponType;

// Weapon special effects
typedef enum {
    WEAPON_EFFECT_NONE,
    WEAPON_EFFECT_FIRE,      // Burning damage over time
    WEAPON_EFFECT_POISON,    // Poison damage over time
    WEAPON_EFFECT_FROST,     // Slows enemy
    WEAPON_EFFECT_VAMPIRIC,  // Steal HP
    WEAPON_EFFECT_STUNNING   // Chance to stun
} WeaponEffect;

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
    ENEMY_BOSS,
    // NEW SPECIAL TYPES
    ENEMY_ARCHER,       // Ranged attacker
    ENEMY_SUMMONER,     // Spawns minions
    ENEMY_HEALER,       // Heals other enemies
    ENEMY_TELEPORTER,   // Blinks around
    ENEMY_TANK          // High HP, slow
} EnemyType;

// Ability types
typedef enum {
    ABILITY_FIREBALL,
    ABILITY_HEAL,
    ABILITY_TELEPORT,
    ABILITY_DASH
} AbilityType;

// Character Classes
typedef enum {
    CLASS_WARRIOR,
    CLASS_MAGE,
    CLASS_ROGUE
} CharacterClass;

// Random event types
typedef enum {
    EVENT_NONE,
    EVENT_AMBUSH,           // Extra enemies spawn!
    EVENT_TREASURE,         // Bonus gold/items
    EVENT_BLESSING,         // Stat buffs
    EVENT_CURSE,            // Stat debuffs
    EVENT_MERCHANT,         // Free merchant appears
    EVENT_SHRINE,           // Free shrine buff
    EVENT_HEALING,          // Free full heal
    EVENT_XP_BONUS          // Bonus XP this floor
} RandomEventType;

// Shop item types
typedef enum {
    SHOP_HEALTH_POTION,
    SHOP_MANA_POTION,
    SHOP_WEAPON,
    SHOP_ARMOR,
    SHOP_ABILITY_SCROLL
} ShopItemType;

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
    ROOM_BOSS,
    ROOM_SHOP
} RoomType;

// Biome types
typedef enum {
    BIOME_DUNGEON,      // Standard stone dungeon
    BIOME_CRYPT,        // Undead themed
    BIOME_FOREST,       // Nature/overgrown
    BIOME_LAVA_CAVES,   // Fire themed
    BIOME_ICE_CAVERN    // Ice themed
} BiomeType;

// Interactive object types
typedef enum {
    OBJ_NONE,
    OBJ_CHEST,
    OBJ_MIMIC,
    OBJ_LEVER,
    OBJ_PRESSURE_PLATE,
    OBJ_SHRINE,
    OBJ_SECRET_DOOR,
    OBJ_MERCHANT
} InteractiveObjectType;

// Shrine buff types
typedef enum {
    SHRINE_NONE,
    SHRINE_STRENGTH,    // +damage
    SHRINE_DEFENSE,     // +armor
    SHRINE_VITALITY,    // +max HP
    SHRINE_MYSTIC       // +max mana
} ShrineType;

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
    BiomeType biome;
} Room;

// Interactive object structure
typedef struct {
    Position pos;
    InteractiveObjectType type;
    bool activated;
    bool opened;      // For chests
    ShrineType shrine_buff;
    int linked_door_x, linked_door_y;  // For levers/pressure plates
    char name[30];
} InteractiveObject;

// Environmental hazard structure
typedef struct {
    Position pos;
    TileType type;
    int damage;
    bool active;
} EnvironmentalHazard;

// Equipment structure
typedef struct {
    WeaponType weapon;
    ArmorType armor;
    int weapon_damage;
    int armor_defense;
    char weapon_name[50];            // Increased for prefixes
    char armor_name[50];
    WeaponEffect weapon_effect;      // Special effect
    int weapon_effect_chance;        // Proc chance (0-100)
} Equipment;

// Inventory item structure
typedef struct {
    ItemType type;
    WeaponType weapon_type;
    ArmorType armor_type;
    int value;
    char name[50];              // Increased for prefixes
    bool exists;
    WeaponEffect weapon_effect;
    int weapon_effect_chance;
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
    char name[40];           // Increased for elite names
    StatusEffect status_effects[MAX_STATUS_EFFECTS];
    int status_effect_count;
    bool is_elite;           // Elite/Champion enemy
    char elite_title[20];    // "Swift", "Venomous", etc.
} Enemy;

// Projectile structure (arrows, magic missiles, etc.)
typedef struct {
    Position pos;
    int dx, dy;           // Direction (-1, 0, 1)
    int damage;
    bool active;
    char symbol;
    int color_pair;
    int range_remaining;
    bool is_player_projectile;  // True if player shot it
} Projectile;

// Item structure (on ground)
typedef struct {
    Position pos;
    ItemType type;
    int value;
    bool collected;
    WeaponType weapon_type;
    ArmorType armor_type;
    int bonus;
    char name[50];                   // Increased for prefixes
    WeaponEffect weapon_effect;      // Special effect
    int weapon_effect_chance;        // Proc chance
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
    CharacterClass class;
    int crit_chance;      // Percentage (0-100)
    int dodge_chance;     // Percentage (0-100)
    float spell_cost_modifier;  // 1.0 = normal, 0.75 = 25% cheaper
    int arrows;           // Arrow ammunition count
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
    InteractiveObject objects[MAX_INTERACTIVE_OBJECTS];
    int object_count;
    EnvironmentalHazard hazards[MAX_HAZARDS];
    int hazard_count;
    Position stairs_pos;
    BiomeType current_biome;
    char message[100];
    char message_log[5][100];                 // Last 5 messages
    int message_log_count;
    int turn_count;
    int dungeon_level;
    bool show_inventory;
    bool show_abilities;
    int floors_since_merchant;  // Track floors without merchant for guaranteed spawn
    RandomEventType current_event;  // Current floor's event
    bool event_active;              // Is event effect active?
    int event_duration;             // Turns remaining for event
    Projectile projectiles[20];     // Active projectiles
    int projectile_count;
    bool in_firing_mode;            // Is player aiming?
    int aim_dx, aim_dy;             // Aim direction
} Game;

#endif