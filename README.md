# Roguelike Dungeon Quest

A feature-rich roguelike dungeon crawler built in C with ncurses, featuring procedurally generated dungeons, tactical combat, ranged weapons, random events, status effects, and deep character progression.

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

## 🎮 Features

### Core Gameplay
- **Procedurally Generated Dungeons**: Every floor is unique with rooms, corridors, and varied layouts
- **Multiple Room Types**: Normal rooms, treasure rooms, monster dens, safe zones, and boss rooms
- **Field of View (FOV)**: Explore and discover the dungeon with line-of-sight mechanics
- **Random Floor Events**: 8 unique events that change each floor's experience (40% chance)
- **Permadeath**: Classic roguelike difficulty with save/load system

### Combat System
- **Turn-Based Tactical Combat**: Strategic battles against multiple enemy types
- **Critical Hits**: Deal 2x damage with critical strikes (varies by class)
- **Dodge System**: Evade incoming attacks based on your character class
- **Ranged Combat**: Use bows and arrows for tactical distance fighting
- **Melee Combat**: Traditional close-quarters weapon combat
- **Status Effects**: 
  - 🔥 **Burn** - Fire damage over time (3 dmg/turn)
  - ☠️ **Poison** - Toxic damage that persists (2 dmg/turn)
  - 🐌 **Slow** - Reduced movement speed (50% slower)
  - 💫 **Stun** - Complete immobilization (1 turn)
  - 💚 **Regeneration** - Heal over time

### Character Classes
Choose your playstyle with 3 distinct classes:

- **⚔️ Warrior** - Tank Role
  - +30 Max HP, +5 Base Damage
  - 10% Critical Hit, 5% Dodge
  - High survivability, reliable damage

- **🔮 Mage** - Glass Cannon
  - +30 Max Mana, 25% cheaper spells
  - 15% Critical Hit, 5% Dodge
  - High spell damage, lower HP

- **🗡️ Rogue** - High Risk/High Reward
  - 25% Critical Hit, 20% Dodge
  - Balanced stats
  - **Best for bow builds!**

### Enemy Variety
- **Basic Enemies**:
  - Goblins (fast, aggressive, moves twice per turn)
  - Orcs (strong, slower, high HP)
  - Bats (erratic movement, unpredictable)
  - Bosses (powerful dungeon guardians)

- **Special Enemy Types** (spawn from floor 2+):
  - 🏹 **Archers** - Ranged attacks, keep distance
  - 👻 **Summoners** - Spawn minions when damaged
  - 💚 **Healers** - Restore HP to nearby enemies
  - 🌀 **Teleporters** - Unpredictable blink movement
  - 🛡️ **Tanks** - High HP defensive units

- **👑 Elite/Champion Enemies** (7% spawn rate):
  - Gold/reversed display for visibility
  - 2x HP, 1.5x damage
  - 2.5x gold rewards, 2x XP
  - Elite titles: "Swift", "Iron", "Venomous", "Savage", "Ancient"
  - Create memorable boss-like encounters

### Weapon Systems
- **Melee Weapons**: Dagger, Sword, Axe, Greatsword
- **Ranged Weapons**: Bow (requires arrows)
- **Armor Types**: Leather, Chainmail, Plate

- **🔥 Weapon Special Effects** (30% of weapons):
  - **Fire** (20% proc) - Burns enemies for 3 dmg/turn, 3 turns
  - **Poison** (25% proc) - Poisons for 2 dmg/turn, 4 turns
  - **Frost** (15% proc) - Slows enemies by 50%, 3 turns
  - **Vampiric** (10% proc) - Steals 10% damage as HP
  - **Stunning** (10% proc) - Stuns for 1 turn
  - Weapon names: "Flaming Sword", "Venomous Dagger", etc.

### Ranged Combat System
- **Bow & Arrows**: Tactical ranged weapon system
- **Aiming Mode**: Press F to enter 8-direction aiming
- **Arrow Management**: Find/buy arrows (start with 10)
- **Projectile Physics**: Arrows fly up to 8 tiles
- **Critical Shots**: Crits work with bows!
- **Tactical Kiting**: Hit enemies before they reach you

### Random Floor Events
8 unique events that can trigger each floor (40% chance):

- **[!] AMBUSH** (15%) - 3-5 extra enemies spawn around you!
- **[$] TREASURE** (15%) - Bonus gold + 2-3 items spawn
- **[*] BLESSING** (15%) - +5 damage, +20 max HP (permanent!)
- **[X] CURSE** (10%) - -3 damage this floor only
- **[M] MERCHANT** (10%) - Free merchant appears
- **[?] SHRINE** (10%) - Free shrine buff spawns
- **[+] HEALING** (10%) - Full HP/Mana restore
- **[^] XP BONUS** (15%) - +50% XP gains this floor

### Character Progression
- **Level Up System**: Gain XP from defeating enemies
- **Equipment Upgrades**: Find progressively better gear
- **Abilities**:
  - **Fireball** (25 mana) - AOE damage + burn effect
  - **Heal** (20 mana) - Restore HP and cure poison/burn
  - **Teleport** (30 mana) - Escape to random safe location
  - **Dash** (15 mana) - Quick 5-tile movement in any direction
- **Inventory System**: Collect and manage potions, equipment, and treasures
- **Merchant Shops**: Buy potions, weapons, armor, arrows, and elixirs
- **Interactive Objects**: Chests, shrines, levers, traps, and merchants

### UI Features
- **Health & Mana Bars**: Visual progress indicators with gradients
- **Mini-Map**: Overview of explored areas
- **Message Log**: Track last 5 events and combat results
- **Status Indicators**: See active effects on yourself and enemies
- **Colored Graphics**: Color-coded enemies, items, effects, and elites
- **Enemy Health Bars**: Visual HP indicators above damaged enemies
- **Arrow Counter**: Track your ammunition

## 📋 Requirements

- **GCC** compiler
- **ncurses** library
- **Make** build tool
- Linux/Unix environment (tested on Ubuntu)

## 🔧 Installation

### Ubuntu/Debian
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential libncurses5-dev libncursesw5-dev

# Clone repository
git clone https://github.com/TheoFraser/roguelike-dungeon-quest.git
cd roguelike-dungeon-quest

# Compile
make

# Run
./roguelike
```

### Other Linux Distributions
```bash
# Fedora/RHEL
sudo dnf install gcc ncurses-devel make

# Arch
sudo pacman -S gcc ncurses make

# Then compile as above
make
./roguelike
```

## 🎯 Controls

### Movement
- **Arrow Keys** or **WASD** or **HJKL** - Move in 4 directions
- Movement into enemies automatically attacks them

### Actions
- **I** - Open/close inventory
- **C** - Open abilities menu
- **F** - Enter firing mode (aim bow)
- **E** - Interact with objects (chests, shrines, merchants)
- **M** - Open merchant shop (when near merchant)
- **F5** - Save game
- **F9** - Load game
- **Q** - Quit to menu

### Firing Mode (when bow equipped)
- **Arrow Keys** - Aim in 8 directions
- **ENTER** - Fire arrow
- **ESC** - Cancel firing mode

### Abilities (from abilities menu)
- **F** - Cast Fireball
- **H** - Cast Heal
- **T** - Teleport
- **Shift + Arrow** - Dash in direction

### Inventory (when open)
- **1-9** - Use/equip item
- **D + 1-9** - Drop item

## 🎲 Gameplay Tips

### General Strategy
1. **Explore Carefully**: Use FOV to scout ahead and avoid surprises
2. **Manage Resources**: Potions, mana, and arrows are limited - use wisely
3. **Safe Rooms**: Perfect for resting and planning your approach
4. **Treasure Rooms**: High risk, high reward - usually well-guarded
5. **Level Up First**: Grind early floors before descending deeper

### Combat Tips
6. **Critical Builds**: Rogue class gets 25% crit rate - devastating with bow!
7. **Status Effects**: Fireball's burn + Poison weapon = massive DOT
8. **Kiting**: Use bow to damage enemies before they reach you
9. **Dodge Tank**: Rogue's 20% dodge makes them surprisingly tanky
10. **Teleport Escapes**: When surrounded, teleport to safety

### Equipment Strategy
11. **Weapon Effects**: "Flaming Bow" + "Venomous Dagger" = unstoppable
12. **Elite Hunting**: Hunt gold/reversed enemies for 2.5x gold rewards
13. **Arrow Management**: Always buy arrows from merchants (15 for ~30g)
14. **Save Potions**: Use Heal spell before drinking health potions

### Event Strategy
15. **Blessing Events**: Stack multiple blessings for +10, +15 damage!
16. **XP Bonus**: Clear entire floor for maximum level gains
17. **Ambush Prep**: Keep bow equipped after floor 2 for ambush events
18. **Curse Floors**: Play defensively, use ranged combat

### Advanced Tactics
19. **Crit Archer Build**: Rogue + Bow + Crit = 25% chance for 2x damage arrows
20. **DOT Stacking**: Burn + Poison = 5 damage per turn (no mana cost!)
21. **Merchant Events**: Buy arrows and elixirs when merchant events trigger
22. **Position Matters**: Use ranged advantage, bottleneck corridors

## 📁 Project Structure

```
roguelike-game/
├── ability.c/h           # Ability system (fireball, heal, teleport, dash)
├── dungeon.c/h           # Dungeon generation and room creation
├── dungeon_features.c/h  # Interactive objects (chests, shrines, traps, levers)
├── enemy.c/h             # Enemy AI, special types, and behavior
├── fov.c/h               # Field of view calculations
├── game.c/h              # Main game loop and state management
├── inventory.c/h         # Inventory management
├── item.c/h              # Item generation and properties
├── player.c/h            # Player stats, movement, and actions
├── ranged.c/h            # Ranged combat system (bows, arrows, projectiles)
├── random_events.c/h     # Random floor events system
├── render.c/h            # Display and UI rendering
├── save.c/h              # Save/load system
├── shop.c/h              # Merchant and shopping system
├── status.c/h            # Status effect system
├── types.h               # Type definitions and constants
├── utils.c/h             # Utility functions
├── main.c                # Entry point and title screen
├── Makefile              # Build configuration
└── README.md             # This file
```

## 🛠️ Building from Source

```bash
# Clean previous builds
make clean

# Build with debug symbols
make

# Build and run
make run
```

## 💾 Save System

- Games are automatically saved to `roguelike_save.dat`
- Press **F5** during gameplay to save
- Press **F9** from title screen or during gameplay to load
- Only one save slot available (classic roguelike style)

## 🎨 Color Guide

### Status Effects
- 🔴 **Red** - Burn damage
- 🟢 **Green** - Poison/Regeneration
- 🔵 **Blue** - Slow effect
- 🟡 **Yellow** - Stun effect

### Enemies & Items
- **Gold/Reversed** - Elite/Champion enemies (2.5x loot!)
- **Green** - Healing items, friendly NPCs
- **Blue** - Mana items, magic
- **Yellow** - Gold, treasure
- **Red** - Enemies, danger
- **Cyan** - Player, important items

## 🏆 Build Examples

### 1. Crit Archer (Rogue)
- **Class**: Rogue (25% crit, 20% dodge)
- **Weapon**: Bow + Arrows
- **Strategy**: Ranged critical hits, kiting
- **Power**: Extreme burst damage

### 2. Tank Warrior
- **Class**: Warrior (high HP)
- **Weapon**: Stunning Mace + Heavy Armor
- **Strategy**: Face-tank with stun control
- **Power**: Unbreakable defense

### 3. DOT Mage
- **Class**: Mage (cheap spells)
- **Weapon**: Venomous Dagger
- **Strategy**: Fireball + Poison weapon
- **Power**: Sustained damage over time

### 4. Blessing Hunter (Any Class)
- **Strategy**: Look for Blessing events
- **Goal**: Stack +5 damage multiple times
- **Power**: Exponential scaling

### 5. Hybrid Rogue
- **Class**: Rogue
- **Weapons**: Bow + Flaming Sword
- **Strategy**: Range + melee versatility
- **Power**: Tactical flexibility

## 📊 Game Statistics

- **15+ Source Files**: ~6000+ lines of C code
- **8 Event Types**: Dynamic floor events
- **5 Special Enemy Behaviors**: Unique AI patterns
- **6 Weapon Effects**: Build-defining properties
- **3 Character Classes**: Distinct playstyles
- **5 Status Effects**: Deep combat mechanics
- **Infinite Replayability**: Procedural generation + random events

## 🐛 Known Issues

- Terminal must be at least 80x30 characters for proper display
- Some terminals may have color display issues
- Save files are not portable between different versions
- Unicode characters in events replaced with ASCII for compatibility

## 🎯 Achievement Ideas

- [ ] Defeat an Elite enemy
- [ ] Get 3 critical hits in a row
- [ ] Stack 2 Blessing events
- [ ] Kill an enemy with arrow from 8 tiles away
- [ ] Reach floor 10
- [ ] Defeat boss with bow only
- [ ] Collect 500+ gold
- [ ] Complete run without dying
- [ ] Find a weapon with special effect
- [ ] Survive an Ambush event

## 🔮 Future Enhancement Ideas

- [ ] More biome types (ice caves, lava, forest, crypt)
- [ ] Additional enemy types
- [ ] Magic scrolls and spell books
- [ ] Crafting system
- [ ] Pet/companion system
- [ ] Multiple difficulty levels
- [ ] Achievement system
- [ ] Daily challenge mode
- [ ] Sound effects (terminal beep variations)
- [ ] Leaderboard system

## 📝 License

This project is released under the MIT License. See LICENSE file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 👨‍💻 Author

Created with ❤️ by **Theo Fraser**

