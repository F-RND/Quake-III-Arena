/*
===========================================================================
Procedural Arena Generator for Roguelike Mode

Generates Q3A-style arenas on-the-fly for roguelike gameplay
===========================================================================
*/

#ifndef G_ARENA_GEN_H
#define G_ARENA_GEN_H

#include "q_shared.h"

// Arena generation configuration
#define MAX_ARENA_ROOMS     64
#define MAX_ARENA_CORRIDORS 128
#define MAX_ARENA_BRUSHES   4096
#define MAX_ARENA_ENTITIES  512

// Room types for variety
typedef enum {
    ROOM_TYPE_ARENA,        // Open circular combat arena
    ROOM_TYPE_CORRIDOR,     // Long hallway
    ROOM_TYPE_JUNCTION,     // Multi-way intersection
    ROOM_TYPE_MULTILEVEL,   // Platforms and verticality
    ROOM_TYPE_HAZARD,       // Lava pits, environmental dangers
    ROOM_TYPE_BOSS,         // Large boss arena
    ROOM_TYPE_START,        // Player spawn room
    ROOM_TYPE_EXIT,         // Arena exit/portal room
    ROOM_TYPE_COUNT
} roomType_t;

// Arena themes (determines textures, aesthetics)
typedef enum {
    THEME_TECH,       // Tech facilities (baseq3 style)
    THEME_GOTHIC,     // Gothic castles
    THEME_SPACE,      // Space stations
    THEME_HELL,       // Lava and fire
    THEME_RANDOM,     // Mix of all themes
    THEME_COUNT
} arenaTheme_t;

// Room difficulty affects enemy placement
typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD,
    DIFFICULTY_BOSS,
    DIFFICULTY_COUNT
} roomDifficulty_t;

// Procedural room definition
typedef struct arenaRoom_s {
    roomType_t type;
    vec3_t origin;              // Center position
    vec3_t mins, maxs;          // Bounding box
    int width, height, depth;   // Dimensions
    arenaTheme_t theme;
    roomDifficulty_t difficulty;
    int connectedRooms[8];      // Indices of connected rooms
    int numConnections;
    qboolean isBossRoom;
    qboolean isStartRoom;
    qboolean isExitRoom;
    int entityCount;            // Number of enemies/items
    struct arenaRoom_s *next;
} arenaRoom_t;

// Corridor connecting two rooms
typedef struct {
    int roomA, roomB;           // Room indices
    vec3_t start, end;
    int width;
    arenaTheme_t theme;
} arenaCorridor_t;

// Complete arena definition
typedef struct {
    int seed;                   // Generation seed
    int depth;                  // Difficulty level (1+)
    arenaTheme_t theme;

    arenaRoom_t *rooms;
    int numRooms;

    arenaCorridor_t corridors[MAX_ARENA_CORRIDORS];
    int numCorridors;

    vec3_t worldMins, worldMaxs; // Total arena bounds

    // Entity placement
    vec3_t playerSpawns[MAX_CLIENTS];
    int numPlayerSpawns;

    vec3_t enemySpawns[MAX_ARENA_ENTITIES];
    int numEnemySpawns;

    vec3_t itemSpawns[MAX_ARENA_ENTITIES];
    int numItemSpawns;

    vec3_t exitPortal;          // Exit to next arena

    // Generation statistics
    int totalArea;
    int totalBrushes;
    float generationTime;
} arena_t;

// Roguelike run state
typedef struct {
    int seed;                   // Master seed for run
    int currentDepth;           // Current arena number (1+)
    int maxDepth;               // Deepest reached
    int score;                  // Total score
    int kills;                  // Total kills
    qboolean permadeath;        // True = roguelike mode
    int lives;                  // Extra lives
    float timeElapsed;          // Total time in run
    arena_t *currentArena;      // Currently loaded arena
} roguelikeRun_t;

//=================
// Core API
//=================

// Initialize arena generator (call once at startup)
void        G_InitArenaGenerator(void);

// Generate a new arena
arena_t*    G_GenerateArena(int seed, int depth, arenaTheme_t theme);

// Free arena memory
void        G_FreeArena(arena_t *arena);

// Compile arena to playable map (creates runtime BSP)
qboolean    G_CompileArena(arena_t *arena);

// Load generated arena into game
qboolean    G_LoadArena(arena_t *arena);

//=================
// Room Generation
//=================

// Create a room of specified type
arenaRoom_t* G_CreateRoom(roomType_t type, vec3_t origin, int width, int height, int depth);

// Connect two rooms with a corridor
qboolean    G_ConnectRooms(arena_t *arena, int roomA, int roomB);

// Ensure all rooms are reachable (pathfinding check)
qboolean    G_ValidateConnectivity(arena_t *arena);

//=================
// Entity Placement
//=================

// Place player spawn points
void        G_PlacePlayerSpawns(arena_t *arena, int numPlayers);

// Place enemy spawn points based on difficulty
void        G_PlaceEnemySpawns(arena_t *arena, int depth);

// Place items (weapons, health, armor)
void        G_PlaceItems(arena_t *arena, int depth);

// Place exit portal
void        G_PlaceExitPortal(arena_t *arena);

//=================
// Roguelike Mode
//=================

// Start a new roguelike run
roguelikeRun_t* G_StartRoguelikeRun(int seed, qboolean permadeath);

// Advance to next arena (after completing current)
qboolean    G_AdvanceToNextArena(roguelikeRun_t *run);

// End roguelike run (death or victory)
void        G_EndRoguelikeRun(roguelikeRun_t *run, qboolean victory);

// Get current run state (for HUD display)
roguelikeRun_t* G_GetCurrentRun(void);

//=================
// Utility
//=================

// Get theme name string
const char* G_GetThemeName(arenaTheme_t theme);

// Get room type name string
const char* G_GetRoomTypeName(roomType_t type);

// Calculate arena difficulty score
int         G_CalculateArenaDifficulty(arena_t *arena);

// Debug: Print arena information
void        G_PrintArenaInfo(arena_t *arena);

// Debug: Export arena to .map file
void        G_ExportArenaToMap(arena_t *arena, const char *filename);

#endif // G_ARENA_GEN_H
