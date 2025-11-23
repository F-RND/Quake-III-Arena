# Quake III Arena - Open World Modernization Project

## Project Vision

Transform Quake III Arena into a modern, open-world multiplayer experience with:
- **Minecraft-esque procedural environments** - Chunk-based world generation with voxel/block systems
- **AI-driven NPCs** - Interactive characters with dialogue systems ready for AI-generated content
- **Expanded multiplayer** - Modern networking to support larger player counts
- **RTX Remix compatibility** - Modern rendering pipeline for ray tracing and enhanced graphics
- **Easy character modification** - Streamlined systems for adding and customizing NPCs

## Progress Overview

### ✅ Completed

#### 1. Modern Build System (CMake)
- ✅ Created comprehensive CMakeLists.txt replacing ancient `cons` build system
- ✅ Updated from GCC 2.95 requirement to modern compilers (GCC 13+)
- ✅ Separated client and dedicated server builds
- ✅ Added 64-bit (x86_64) architecture support
- ✅ Platform-specific code organization (Linux/Windows/Mac)
- ✅ Game module (qagame, cgame, ui) compilation with architecture-specific naming
- ✅ Modern C11/C++17 standards

**Files Added:**
- `/CMakeLists.txt` - Root build configuration
- `/code/game/CMakeLists.txt` - Server-side game module
- `/code/cgame/CMakeLists.txt` - Client-side game module
- `/code/ui/CMakeLists.txt` - UI module

#### 2. 64-bit Compatibility (In Progress)
- ✅ Added x86_64 architecture detection
- ✅ Fixed dynamic library loading for 64-bit (code/unix/unix_main.c:727-728)
- ✅ Removed deprecated GameSpy rankings system
- ✅ Fixed BOTLIB compilation with proper preprocessor defines
- ✅ Cleaned up JPEG library integration
- ⚠️ Remaining: Fix inline function linker errors in botlib
- ⚠️ Remaining: Fix missing Com_Allocate/Com_Dealloc symbols

### 🔄 In Progress

#### Build Stabilization
**Current Issues:**
1. **Botlib inline functions** - Modern GCC requires external definitions for inline functions
   - `AAS_TravelFlagForType_inline`
   - `AAS_AreaContentsTravelFlags_inline`
   - `AAS_ClusterAreaNum`
   - `AAS_RoutingTime`

2. **Missing symbols**
   - `Com_Allocate` / `Com_Dealloc` (used by splines)
   - `Sys_SnapVector` (server-side)

**Next Steps:**
- Define or stub missing functions
- Complete successful first build
- Test basic engine functionality

### 📋 Planned - Core Architecture

#### 1. Chunk-Based World System
**Goal:** Replace static BSP maps with dynamic, streaming chunk-based world

**Key Changes:**
- New world manager (`code/world/`) directory
  - `world_chunk.c` - Chunk management (16x16x256 blocks)
  - `world_gen.c` - Procedural terrain generation
  - `world_stream.c` - Dynamic chunk loading/unloading
  - `world_mesh.c` - Runtime mesh generation from voxel data

- Entity system expansion
  - Increase `MAX_GENTITIES` from 1024 to 65536
  - Implement spatial partitioning (octree/quadtree)
  - Chunk-based entity management

- Remove/bypass BSP dependencies
  - Keep BSP for legacy map support
  - New `worldType` cvar (bsp/chunks/hybrid)

**Files to Modify:**
- `code/qcommon/cm_load.c` - Add chunk collision system
- `code/server/sv_world.c` - Spatial partitioning for entities
- `code/game/g_main.c` - World type initialization

#### 2. Voxel/Block System
**Goal:** Minecraft-style block-based environment

**Implementation:**
- Block registry system (`code/world/block_registry.c`)
  - Block types, properties, rendering data
  - Scriptable block behaviors

- Voxel storage
  - Efficient chunk storage (palette compression)
  - Block metadata (rotation, state)
  - Light propagation for dynamic lighting

- Block interaction
  - Place/break mechanics
  - Physics (falling blocks, fluids)
  - Crafting system foundation

**New Files:**
- `code/world/block.h` - Block type definitions
- `code/world/voxel_storage.c` - Chunk data structures
- `code/world/block_physics.c` - Block behavior simulation

#### 3. NPC Interaction Framework
**Goal:** Rich NPC system with AI-ready dialogue

**Components:**
- NPC entity type (`gentity_t` extension)
  - Personality/faction data
  - Dialogue state
  - Schedule/routine system
  - Inventory management

- Dynamic navigation
  - Replace static AAS with Recast/Detour
  - Runtime navmesh generation for procedural worlds
  - NPC path finding and avoidance

- Behavior system
  - Behavior trees or utility AI
  - State machines for NPC activities
  - Quest/task system

**Files to Create:**
- `code/game/npc_core.c` - NPC entity management
- `code/game/npc_behavior.c` - AI behavior trees
- `code/game/npc_navigation.c` - Modern pathfinding
- `code/game/npc_schedule.c` - Day/night routines

**Files to Modify:**
- `code/game/g_local.h` - Extend gentity_t for NPCs
- `code/botlib/*` - Adapt or replace with modern AI

#### 4. Dialogue System
**Goal:** Flexible dialogue with AI integration points

**Features:**
- Dialogue tree structure
  - JSON/Lua-based dialogue scripts
  - Branching conversations
  - Condition/consequence system
  - Variable substitution for dynamic content

- AI integration
  - Hooks for AI-generated responses
  - Context management for LLM calls
  - Fallback to scripted dialogue
  - Voice line placeholder system

- UI components
  - Dialogue interface (code/cgame/cg_dialogue.c)
  - Subtitle system
  - Choice presentation

**New Files:**
- `code/game/dialogue_tree.c` - Dialogue data structures
- `code/game/dialogue_engine.c` - Conversation management
- `code/game/dialogue_ai.c` - AI generation interface
- `code/cgame/cg_dialogue_ui.c` - Client-side UI
- `dialogues/` - Dialogue content directory (JSON/Lua)

#### 5. Renderer Modernization
**Goal:** Modern rendering for RTX Remix compatibility

**Phases:**
1. **OpenGL Modern Path**
   - Replace fixed-function with shader-based rendering
   - Deferred rendering pipeline
   - PBR (Physically-Based Rendering)
   - Dynamic global illumination

2. **Vulkan Backend** (RTX Remix target)
   - Vulkan 1.3 renderer
   - Ray tracing support (VK_KHR_ray_tracing)
   - GPU-driven rendering for voxels
   - Mesh shaders for terrain LOD

**Files to Create:**
- `code/renderer_modern/` - New renderer directory
  - `tr_init_modern.c`
  - `tr_shader_system.c`
  - `tr_deferred.c`
  - `tr_voxel_render.c`
  - `tr_vulkan/` - Vulkan backend

**Renderer Features:**
- Dynamic lighting (no more lightmaps)
- Volumetric fog/clouds
- Procedural skybox
- LOD system for distant terrain
- Particle systems for effects

#### 6. Network Framework Upgrade
**Goal:** Scalable modern multiplayer

**Improvements:**
- Replace UDP with modern alternatives
  - ENet for reliable UDP
  - WebRTC for web client support
  - SteamNetworkingSockets integration option

- Protocol updates
  - Increase entity limit in network packets
  - Delta compression for voxel chunks
  - Client-side prediction improvements
  - Server-authoritative validation

- Matchmaking
  - Lobby system
  - Skill-based matching
  - Session persistence

**Files to Modify:**
- `code/server/sv_net_chan.c` - New network backend
- `code/qcommon/msg.c` - Packet encoding
- `code/server/sv_snapshot.c` - Entity streaming

#### 7. Procedural World Generation
**Goal:** Infinite, interesting terrain

**Generation Systems:**
- Noise-based terrain (Perlin/Simplex)
- Biome system
- Structure placement (buildings, dungeons)
- Cave networks
- Vegetation distribution

**Files to Create:**
- `code/world/noise.c` - Noise generation
- `code/world/biome.c` - Biome definitions
- `code/world/structures.c` - Procedural structures
- `code/world/world_seed.c` - Seed management

## Technical Debt & Cleanup

### Removed/Deprecated
- ✅ GameSpy rankings system (sv_rankings.c)
- ⏳ QVM bytecode system (will add native module support)
- ⏳ Old fixed-function renderer

### To Modernize
- Replace `cons` build scripts (✅ Done)
- Update Visual Studio projects (Windows)
- Add CI/CD pipeline
- Modern dependency management (vcpkg/conan)
- Hot-reload support for development
- Lua/Python scripting integration

## File Structure (Proposed)

```
Quake-III-Arena/
├── code/
│   ├── game/              # Server-side game logic
│   │   ├── npc_*.c        # NEW: NPC systems
│   │   ├── dialogue_*.c   # NEW: Dialogue engine
│   │   └── ...
│   ├── cgame/             # Client-side game
│   │   ├── cg_dialogue_ui.c  # NEW: Dialogue UI
│   │   └── ...
│   ├── world/             # NEW: World systems
│   │   ├── chunk.c/h
│   │   ├── voxel.c/h
│   │   ├── generation.c/h
│   │   ├── blocks.c/h
│   │   └── ...
│   ├── renderer_modern/   # NEW: Modern renderer
│   │   ├── vulkan/
│   │   └── ...
│   ├── network/           # NEW: Modern networking
│   └── ...
├── data/
│   ├── blocks/            # NEW: Block definitions
│   ├── biomes/            # NEW: Biome configs
│   ├── dialogues/         # NEW: NPC dialogues
│   └── structures/        # NEW: Procedural structures
└── ...
```

## Development Roadmap

### Phase 1: Foundation (Current)
- [x] Modern build system
- [ ] Complete first successful build
- [ ] Basic runtime testing
- [ ] Fix critical 64-bit issues

### Phase 2: Core Systems (Next 2-4 weeks)
- [ ] Chunk-based world manager
- [ ] Basic voxel rendering
- [ ] Entity system expansion
- [ ] Procedural terrain generation (simple)

### Phase 3: Interactivity (Next 4-6 weeks)
- [ ] NPC entity framework
- [ ] Basic AI behaviors
- [ ] Dialogue system implementation
- [ ] Block placement/interaction

### Phase 4: Rendering (Next 6-8 weeks)
- [ ] Modern OpenGL renderer
- [ ] Deferred shading
- [ ] Dynamic lighting
- [ ] Vulkan backend (RTX Remix prep)

### Phase 5: Networking (Next 8-10 weeks)
- [ ] Network protocol v2
- [ ] Chunk streaming
- [ ] Expanded player capacity
- [ ] Matchmaking basics

### Phase 6: Content & Polish (Ongoing)
- [ ] AI dialogue integration
- [ ] Advanced procedural generation
- [ ] Quest system
- [ ] Performance optimization

## Dependencies

### Current
- OpenGL
- X11 (Linux)
- JPEG library (internal)
- Standard C/C++ libraries

### Planned
- Vulkan SDK (for modern renderer)
- Recast/Detour (navigation)
- FastNoise2 (procedural generation)
- nlohmann/json or simdjson (dialogue/config)
- Lua or Python (scripting)
- ENet or SteamNetworkingSockets (networking)

## Building

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**Current Status:** Build configures but has linker errors (see Issues above)

## Notes

- Original Q3A GPL release (2005)
- Heavy modernization in progress
- Backwards compatibility with original maps maintained (via BSP loader)
- Target platforms: Linux (primary), Windows, Mac
- Goal: Blend Q3A's fast-paced action with modern open-world exploration

---

**Last Updated:** 2025-11-23
**Status:** Early development - Build system modernization phase
