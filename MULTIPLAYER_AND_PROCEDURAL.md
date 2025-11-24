# Multiplayer Testing & Procedural Map Generation

## Current Status: ✅ BUILD SUCCESSFUL

All executables compile and run on modern Linux (GCC 13, 64-bit)

### Built Components
- **quake3** (3.6MB) - Full game client with multiplayer support
- **quake3ded** (1.6MB) - Dedicated server for hosting games
- **Game modules**: qagame, cgame, ui (architecture-specific .so files)

---

## Phase 1: Multiplayer Verification (CURRENT)

### What We Need
1. **Game Data** (pak0.pk3 from original Q3A or use demo/test assets)
   - Maps, textures, sounds
   - Default player models
   - Weapon definitions

2. **Network Test Setup**
   ```bash
   # Terminal 1: Start dedicated server
   ./quake3ded +set dedicated 2 +set sv_maxclients 16 +map q3dm1

   # Terminal 2: Connect client
   ./quake3 +connect localhost
   ```

3. **Verification Checklist**
   - [ ] Server starts and loads a map
   - [ ] Client can connect to server
   - [ ] Player movement syncs across network
   - [ ] Weapon firing replicates
   - [ ] Multiple clients can join
   - [ ] LAN discovery works
   - [ ] Internet play (port forwarding/NAT)

### Network Architecture (Already Implemented)
- **Protocol**: UDP with reliability layer
- **Delta compression**: Snapshots for bandwidth efficiency
- **Client prediction**: Smooth movement despite latency
- **Server authoritative**: Prevents cheating
- **Max clients**: Currently 64 (can be expanded)

---

## Phase 2: Procedural Arena Generation (ROGUELIKE MODE)

### Vision: Two Game Modes

#### Mode 1: **Adventure Mode** (Persistent Open World)
- Minecraft-style chunk-based world
- Build, explore, NPCs with AI dialogue
- Persistent progression
- Cooperative multiplayer

#### Mode 2: **Roguelike Mode** (NEW!)
- Procedurally generated combat arenas
- Fast-paced arena shooter gameplay
- Permadeath with meta-progression
- Leaderboards and scoring
- Single-player or co-op runs

### Procedural Arena System Design

#### Architecture Overview
```
┌─────────────────────────────────────────┐
│      Roguelike Mode Entry Point         │
│  (New UI menu: "Start Roguelike Run")   │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│       Arena Generator (New System)       │
│                                          │
│  • Seed-based generation                 │
│  • Difficulty scaling                    │
│  • Theme selection (tech/gothic/etc)     │
│  • Size variation (small/medium/large)   │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│      BSP Map Generator (New Code)        │
│                                          │
│  • Room/corridor generation              │
│  • Connectivity graph                    │
│  • Spawn point placement                 │
│  • Weapon/item distribution              │
│  • Lighting calculation                  │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│    Runtime Map Compilation (New)         │
│                                          │
│  • Generate .map format in memory        │
│  • Compile to BSP on-the-fly            │
│  • Load into engine                      │
│  • Cache for re-runs (optional)          │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│       Roguelike Game Logic (New)         │
│                                          │
│  • Wave-based enemy spawning             │
│  • Permadeath handling                   │
│  • Score tracking                        │
│  • Powerup system                        │
│  • Exit portal spawning                  │
└──────────────────────────────────────────┘
```

### Implementation Plan

#### Step 1: Procedural BSP Generator
**New Files:**
- `code/game/g_arena_gen.c` - Main arena generation logic
- `code/game/g_arena_gen.h` - Generator interface
- `code/game/g_bsp_builder.c` - Runtime BSP compilation
- `code/game/g_room_templates.c` - Room prefab library

**Key Components:**
1. **Room Generator**
   - Rectangular rooms (various sizes)
   - Circular arenas
   - Multi-level structures
   - Theme-based geometry (tech, gothic, space)

2. **Corridor System**
   - Connect rooms with hallways
   - Ensure full connectivity (graph traversal)
   - Add verticality (stairs, jumppads)

3. **Brush Generation**
   ```c
   // Example: Generate a room brush
   typedef struct {
       vec3_t mins, maxs;  // Bounding box
       char texture[64];    // Wall texture
       int contents;        // Solid, lava, etc.
   } procBrush_t;

   void GenerateRoom(vec3_t origin, int width, int height, int depth);
   void GenerateCorr idor(vec3_t start, vec3_t end, int width);
   void AddSpawnPoint(vec3_t pos, int teamNum);
   ```

4. **Entity Placement**
   - Player spawn points (balanced)
   - Weapon spawns (tiered by arena depth)
   - Item spawns (health, armor, powerups)
   - Enemy spawn points
   - Exit portal location

5. **BSP Compilation**
   - Convert procedural geometry to .map format
   - Call q3map2 (BSP compiler) at runtime OR
   - Implement simplified BSP builder in code
   - Generate lighting data
   - Create collision mesh

#### Step 2: Roguelike Game Mode
**New Files:**
- `code/game/g_roguelike.c` - Roguelike mode controller
- `code/game/g_waves.c` - Enemy wave spawning
- `code/game/g_scoring.c` - Score and progression
- `code/cgame/cg_roguelike_hud.c` - Roguelike UI elements

**Features:**
1. **Run Initialization**
   ```c
   typedef struct {
       int seed;             // Deterministic generation
       int depth;            // Current arena number
       int difficulty;       // Enemy scaling
       int score;            // Player score
       qboolean permadeath;  // True for roguelike mode
       int lives;            // Extra lives earned
   } roguelikeRun_t;
   ```

2. **Wave System**
   - Spawn enemies in waves
   - Difficulty ramps with arena depth
   - Boss arenas every N levels
   - Clear condition: Kill all enemies

3. **Progression**
   - Complete arena → Generate next deeper arena
   - Carry weapons/health between arenas (limited)
   - Meta-progression: Unlock starting loadouts
   - Leaderboard: Highest depth reached

4. **Permadeath**
   - Death ends run
   - Return to menu with final score
   - Option to share seed for challenge runs

#### Step 3: UI Integration
**New Files:**
- `code/ui/ui_roguelike.c` - Roguelike menu screens
- `code/cgame/cg_roguelike.c` - In-game roguelike HUD

**Screens:**
1. **Main Menu Addition**
   - "Roguelike Mode" button
   - Difficulty selection
   - Seed input (optional - for challenges)
   - Leaderboard view

2. **In-Run HUD**
   - Current depth display
   - Score counter
   - Wave progress
   - Time survived
   - Minimap of generated arena

3. **Death/Victory Screen**
   - Final score
   - Statistics (kills, accuracy, etc.)
   - Share seed option
   - Retry button

### Procedural Generation Algorithm

#### Dungeon Graph Approach
```
1. Start with empty grid (spatial partitioning)
2. Generate main path (guaranteed progression route)
3. Add branch rooms (exploration/loot)
4. Connect rooms with corridors
5. Add vertical elements (platforms, pits)
6. Place entities (spawns, items, enemies)
7. Compile to BSP
```

#### Example Room Templates
```c
// Template system for variety
typedef enum {
    ROOM_ARENA,       // Circular open combat
    ROOM_CORRIDOR,    // Long hallway
    ROOM_MULTILEVEL,  // Platforms and verticality
    ROOM_LAVA_PIT,    // Environmental hazards
    ROOM_BOSS,        // Large arena for boss fights
} roomType_t;

roomTemplate_t g_roomTemplates[] = {
    { ROOM_ARENA, 400, 400, 200, "tech" },
    { ROOM_CORRIDOR, 800, 200, 200, "gothic" },
    // ... more templates
};
```

#### Seed-Based Generation
```c
// Deterministic generation for challenge seeds
void InitArenaGenerator(int seed) {
    srand(seed);
    // Now all rand() calls are deterministic
}

// Generate arena from seed
arena_t *GenerateArena(int seed, int depth, int theme) {
    InitArenaGenerator(seed + depth); // Vary by depth

    arena_t *arena = AllocArena();
    GenerateRoomGraph(arena, depth);
    ConnectRooms(arena);
    PlaceEntities(arena, depth);
    CompileToBS P(arena);

    return arena;
}
```

### Advanced Features (Phase 3+)

1. **Modular Rooms**
   - Pre-designed room chunks that snap together
   - Mix procedural + hand-crafted for quality

2. **Environmental Hazards**
   - Lava pits, acid pools
   - Moving platforms
   - Crushers, spike traps

3. **Special Events**
   - Timed challenges (kill X enemies in Y seconds)
   - Survival waves
   - Low-gravity rooms
   - Darkness (limited visibility)

4. **Biome System**
   - Tech facilities (clean, industrial)
   - Gothic castles (dark, stone)
   - Space stations (futuristic)
   - Hell landscapes (lava, fire)
   - Each biome has unique textures and hazards

5. **Power-up Modifiers**
   - Run-specific modifiers (quad damage, haste)
   - Curse modifiers (increased difficulty for higher scores)
   - Random modifiers each run

---

## Technical Considerations

### Procedural BSP Generation Challenges

1. **Performance**
   - BSP compilation can be slow
   - **Solution**: Simplified BSP builder OR pre-compile common patterns
   - **Solution**: Background thread for next arena compilation

2. **Quality**
   - Random generation can create unfair layouts
   - **Solution**: Playtest heuristics (can player reach all areas?)
   - **Solution**: Balance checks (equal spawn distances, item distribution)

3. **Multiplayer**
   - Server generates arena, clients download
   - **Solution**: Send seed to clients, they generate locally (deterministic)
   - **Solution**: Cache compiled BSPs for common seeds

### BSP vs. Runtime Mesh

**Option A: Full BSP Compilation (More Work, Better Performance)**
- Generate .map file in memory
- Call q3map2 or build internal compiler
- Full PVS culling, lightmaps
- Compatible with existing engine

**Option B: Runtime Mesh Generation (Faster to Implement)**
- Generate simple brush geometry
- Skip PVS compilation (just render everything)
- Dynamic lighting only
- Requires some renderer modifications

**Recommendation**: Start with Option B for prototyping, migrate to Option A for production

---

## Development Roadmap

### Sprint 1: Multiplayer Verification (1-2 days)
- [ ] Acquire or create test game data (pak files)
- [ ] Test local multiplayer (localhost)
- [ ] Test LAN multiplayer
- [ ] Verify all netcode features work
- [ ] Document setup process

### Sprint 2: Procedural Arena Prototype (1 week)
- [ ] Implement simple room generator (rectangular rooms)
- [ ] Add corridor connection algorithm
- [ ] Runtime mesh generation (basic geometry)
- [ ] Test with player spawn
- [ ] Iterate on layout quality

### Sprint 3: Roguelike Mode Logic (1 week)
- [ ] Wave-based enemy spawning
- [ ] Permadeath system
- [ ] Score tracking
- [ ] Arena progression (depth increase)
- [ ] Victory/defeat conditions

### Sprint 4: Polish & UI (3-5 days)
- [ ] Roguelike mode UI
- [ ] HUD elements (depth, score, waves)
- [ ] Leaderboard system (local first)
- [ ] Seed sharing feature

### Sprint 5: Content & Balance (1 week)
- [ ] Multiple room templates
- [ ] Biome themes (textures, hazards)
- [ ] Difficulty curve tuning
- [ ] Item/weapon progression
- [ ] Boss arenas

### Sprint 6: Advanced Features (2-3 weeks)
- [ ] Full BSP compilation (if needed)
- [ ] Environmental hazards
- [ ] Special events/challenges
- [ ] Meta-progression unlocks
- [ ] Online leaderboards

---

## Example: Roguelike Run Flow

```
Player: "Start Roguelike Run"
  ↓
Game: Generate Arena (seed=12345, depth=1)
  ↓
  - Create 5 rooms (small arena)
  - Connect with corridors
  - Place 2 spawn points
  - Spawn 5 weak enemies
  - Place exit portal (locked)
  ↓
Player: Fight enemies
  ↓
Game: All enemies dead → Unlock portal
  ↓
Player: Enter portal
  ↓
Game: Generate Arena (seed=12345, depth=2)
  ↓
  - Create 8 rooms (larger)
  - Spawn 10 enemies (harder)
  - Add lava hazard
  ↓
Player: Dies
  ↓
Game: Show final score (Depth 2, Score 1250)
  ↓
Player: "Try again with same seed" OR "New random run"
```

---

## Why This Is Awesome

1. **Replayability**: Every run is different
2. **Challenge Mode**: Share seeds for competitive runs
3. **Quick Sessions**: Jump in for a fast roguelike arena
4. **Skills**: Master the game through repeated runs
5. **Combines Q3A Shooting with Roguelike Progression**
6. **Multiplayer Roguelike**: Co-op arena runs!

---

## Next Steps

1. ✅ **Verify multiplayer works** (current priority)
2. 📐 **Design procedural generator architecture** (this document)
3. 💻 **Implement basic room generator**
4. 🎮 **Create roguelike mode prototype**
5. 🎨 **Add content and polish**

This is going to be INCREDIBLE! A modern Q3A with roguelike procedural arenas AND open-world exploration!
