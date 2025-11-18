# Roguelike Dungeon Quest

A classic roguelike dungeon crawler built in C with ncurses, featuring procedurally generated dungeons, tactical combat, status effects, and character progression.

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

## 🎮 Features

### Core Gameplay
- **Procedurally Generated Dungeons**: Every floor is unique with rooms, corridors, and varied layouts
- **Multiple Room Types**: Normal rooms, treasure rooms, monster dens, safe zones, and boss rooms
- **Field of View (FOV)**: Explore and discover the dungeon with line-of-sight mechanics
- **Permadeath**: Classic roguelike difficulty with save/load system

### Combat System
- **Turn-Based Combat**: Strategic battles against multiple enemy types
- **Status Effects**: 
  - 🔥 **Burn** - Fire damage over time
  - ☠️ **Poison** - Toxic damage that persists
  - 🐌 **Slow** - Reduced movement speed
  - 💫 **Stun** - Complete immobilization
  - 💚 **Regeneration** - Heal over time
- **Enemy Variety**:
  - Goblins (fast, aggressive)
  - Orcs (strong, slower)
  - Bats (erratic, can poison)
  - Bosses (powerful dungeon guardians)

### Character Progression
- **Level Up System**: Gain XP from defeating enemies
- **Equipment**: Weapons and armor with varying rarities and power levels
- **Abilities**:
  - **Fireball** (25 mana) - AOE damage + burn effect
  - **Heal** (20 mana) - Restore HP and cure poison/burn
  - **Teleport** (30 mana) - Escape to random safe location
  - **Dash** (15 mana) - Quick 5-tile movement in any direction
- **Inventory System**: Collect and manage potions, equipment, and treasures

### UI Features
- **Health & Mana Bars**: Visual progress indicators
- **Mini-Map**: Overview of explored areas
- **Message Log**: Track recent events and combat results
- **Status Indicators**: See active effects on yourself and enemies
- **Colored Graphics**: Color-coded enemies, items, and effects

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
git clone https://github.com/yourusername/roguelike-game.git
cd roguelike-game

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
- **F5** - Save game
- **F9** - Load game
- **Q** - Quit to menu

### Abilities (from abilities menu)
- **F** - Cast Fireball
- **H** - Cast Heal
- **T** - Teleport
- **Shift + Arrow** - Dash in direction

### Inventory (when open)
- **1-9** - Use/equip item
- **D + 1-9** - Drop item

## 🎲 Gameplay Tips

1. **Explore Carefully**: Use FOV to scout ahead and avoid surprises
2. **Manage Resources**: Potions and mana are limited - use them wisely
3. **Status Effects**: Fireball's burn damage helps with tough enemies
4. **Teleport Escapes**: When surrounded, teleport to safety
5. **Safe Rooms**: Look for safe rooms to rest and plan your approach
6. **Treasure Rooms**: High risk, high reward - usually well-guarded
7. **Level Up**: Don't rush - grind a bit to get stronger before descending
8. **Equipment Matters**: Always check for better weapons and armor

## 📁 Project Structure

```
roguelike-game/
├── ability.c/h       # Ability system (fireball, heal, teleport, dash)
├── dungeon.c/h       # Dungeon generation and room creation
├── enemy.c/h         # Enemy AI and behavior
├── fov.c/h           # Field of view calculations
├── game.c/h          # Main game loop and state management
├── inventory.c/h     # Inventory management
├── item.c/h          # Item generation and properties
├── player.c/h        # Player stats, movement, and actions
├── render.c/h        # Display and UI rendering
├── save.c/h          # Save/load system
├── status.c/h        # Status effect system
├── types.h           # Type definitions and constants
├── utils.c/h         # Utility functions
├── main.c            # Entry point and title screen
├── Makefile          # Build configuration
└── README.md         # This file
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

## 🎨 Status Effect Colors

- 🔴 **Red** - Burn damage
- 🟢 **Green** - Poison/Regeneration
- 🔵 **Blue** - Slow effect
- 🟡 **Yellow** - Stun effect

## 🐛 Known Issues

- Terminal must be at least 80x30 characters for proper display
- Some terminals may have color display issues
- Save files are not portable between different versions

## 🔮 Future Enhancements

- [ ] More enemy types with unique abilities
- [ ] Enchanted items with special properties
- [ ] Magic scrolls and spell system
- [ ] Multiple character classes
- [ ] Achievement system
- [ ] Difficulty levels
- [ ] Sound effects (terminal beep variations)
- [ ] Online leaderboard

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

Created with ❤️ by [Theo Fraser]

## 🙏 Acknowledgments

- Inspired by classic roguelikes like NetHack, Rogue, and Dungeon Crawl Stone Soup
- Built using the ncurses library for terminal graphics
- Thanks to the roguelike development community for inspiration and guidance

---

**Enjoy your descent into the dungeon!** 🏰⚔️