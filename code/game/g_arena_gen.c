/*
===========================================================================
Procedural Arena Generator Implementation

Initial implementation - Basic room generation and connectivity
===========================================================================
*/

#include "g_local.h"
#include "g_arena_gen.h"

// Global state
static roguelikeRun_t *currentRun = NULL;
static unsigned int arenaRandSeed = 0;

// Seeded random number generator for deterministic generation
static unsigned int ArenaRand(void) {
    arenaRandSeed = arenaRandSeed * 1103515245 + 12345;
    return (arenaRandSeed / 65536) % 32768;
}

static void ArenaSetSeed(unsigned int seed) {
    arenaRandSeed = seed;
}

static int ArenaRandRange(int min, int max) {
    if (max <= min) return min;
    return min + (ArenaRand() % (max - min + 1));
}

static float ArenaRandFloat(void) {
    return (float)ArenaRand() / 32768.0f;
}

//=================
// Theme Data
//=================

typedef struct {
    const char *name;
    const char *wallTexture;
    const char *floorTexture;
    const char *ceilTexture;
} themeData_t;

static themeData_t g_themeData[THEME_COUNT] = {
    { "Tech",   "base_wall/concrete",  "base_floor/diamond2c", "base_ceil/c_met5_2" },
    { "Gothic", "gothic_block/blocks18b", "gothic_floor/pent03", "gothic_ceil/c_met5_2" },
    { "Space",  "base_wall/metaltech01", "base_floor/grid", "base_ceil/c_met7_2" },
    { "Hell",   "hell/lava1", "hell/lavafloor", "hell/lavaceil" },
    { "Random", "base_wall/concrete", "base_floor/diamond2c", "base_ceil/c_met5_2" }
};

const char* G_GetThemeName(arenaTheme_t theme) {
    if (theme < 0 || theme >= THEME_COUNT) return "Unknown";
    return g_themeData[theme].name;
}

static const char* g_roomTypeNames[ROOM_TYPE_COUNT] = {
    "Arena", "Corridor", "Junction", "Multilevel", "Hazard",
    "Boss", "Start", "Exit"
};

const char* G_GetRoomTypeName(roomType_t type) {
    if (type < 0 || type >= ROOM_TYPE_COUNT) return "Unknown";
    return g_roomTypeNames[type];
}

//=================
// Initialization
//=================

void G_InitArenaGenerator(void) {
    G_Printf("Procedural Arena Generator initialized\n");
    currentRun = NULL;
}

//=================
// Room Creation
//=================

arenaRoom_t* G_CreateRoom(roomType_t type, vec3_t origin, int width, int height, int depth) {
    arenaRoom_t *room = (arenaRoom_t *)G_Alloc(sizeof(arenaRoom_t));

    room->type = type;
    VectorCopy(origin, room->origin);
    room->width = width;
    room->height = height;
    room->depth = depth;

    // Calculate bounding box
    room->mins[0] = origin[0] - width / 2;
    room->mins[1] = origin[1] - depth / 2;
    room->mins[2] = origin[2];

    room->maxs[0] = origin[0] + width / 2;
    room->maxs[1] = origin[1] + depth / 2;
    room->maxs[2] = origin[2] + height;

    room->theme = THEME_TECH; // Default
    room->difficulty = DIFFICULTY_MEDIUM;
    room->numConnections = 0;
    room->isBossRoom = qfalse;
    room->isStartRoom = (type == ROOM_TYPE_START);
    room->isExitRoom = (type == ROOM_TYPE_EXIT);
    room->entityCount = 0;
    room->next = NULL;

    return room;
}

//=================
// Arena Generation
//=================

arena_t* G_GenerateArena(int seed, int depth, arenaTheme_t theme) {
    arena_t *arena;
    int i;
    int startTime = trap_Milliseconds();

    G_Printf("Generating arena: seed=%d, depth=%d, theme=%s\n",
             seed, depth, G_GetThemeName(theme));

    arena = (arena_t *)G_Alloc(sizeof(arena_t));
    memset(arena, 0, sizeof(arena_t));

    arena->seed = seed;
    arena->depth = depth;
    arena->theme = theme;

    ArenaSetSeed(seed + depth); // Vary seed by depth

    // Determine number of rooms based on depth
    int numRooms = 3 + depth + ArenaRandRange(0, depth);
    if (numRooms > MAX_ARENA_ROOMS) numRooms = MAX_ARENA_ROOMS;

    // Generate rooms in a grid pattern (simple algorithm)
    vec3_t currentPos = {0, 0, 0};
    arenaRoom_t *prevRoom = NULL;

    for (i = 0; i < numRooms; i++) {
        roomType_t type = ROOM_TYPE_ARENA;

        // First room is always start
        if (i == 0) {
            type = ROOM_TYPE_START;
        }
        // Last room is exit
        else if (i == numRooms - 1) {
            type = ROOM_TYPE_EXIT;
        }
        // Boss room every 5th depth
        else if (depth % 5 == 0 && i == numRooms - 2) {
            type = ROOM_TYPE_BOSS;
        }
        // Random type otherwise
        else {
            int r = ArenaRandRange(0, 100);
            if (r < 40) type = ROOM_TYPE_ARENA;
            else if (r < 60) type = ROOM_TYPE_CORRIDOR;
            else if (r < 75) type = ROOM_TYPE_JUNCTION;
            else if (r < 90) type = ROOM_TYPE_MULTILEVEL;
            else type = ROOM_TYPE_HAZARD;
        }

        // Size varies by type and depth
        int width = 256 + ArenaRandRange(0, 128) + (depth * 16);
        int height = 256 + ArenaRandRange(0, 64);
        int roomDepth = 256 + ArenaRandRange(0, 128) + (depth * 16);

        if (type == ROOM_TYPE_CORRIDOR) {
            width = 512 + ArenaRandRange(0, 256);
            roomDepth = 128;
        }
        else if (type == ROOM_TYPE_BOSS) {
            width = 512 + (depth * 32);
            roomDepth = 512 + (depth * 32);
            height = 384;
        }

        arenaRoom_t *room = G_CreateRoom(type, currentPos, width, height, roomDepth);
        room->theme = theme;

        // Link rooms
        if (prevRoom) {
            room->next = prevRoom;
        }
        prevRoom = room;

        arena->numRooms++;

        // Move position for next room (simple linear layout for now)
        currentPos[0] += width + 128; // 128 unit gap between rooms

        // Occasional branching
        if (ArenaRand() % 3 == 0 && i > 0) {
            currentPos[1] += ArenaRandRange(-256, 256);
        }
    }

    arena->rooms = prevRoom; // Head of linked list

    // Connect adjacent rooms with corridors
    G_Printf("Connecting rooms...\n");
    arenaRoom_t *room = arena->rooms;
    int roomIndex = 0;
    while (room && room->next) {
        arena->corridors[arena->numCorridors].roomA = roomIndex;
        arena->corridors[arena->numCorridors].roomB = roomIndex + 1;
        VectorCopy(room->origin, arena->corridors[arena->numCorridors].start);
        VectorCopy(room->next->origin, arena->corridors[arena->numCorridors].end);
        arena->corridors[arena->numCorridors].width = 128;
        arena->corridors[arena->numCorridors].theme = theme;
        arena->numCorridors++;

        room = room->next;
        roomIndex++;
    }

    // Place entities
    G_PlacePlayerSpawns(arena, 8); // Support up to 8 players
    G_PlaceEnemySpawns(arena, depth);
    G_PlaceItems(arena, depth);
    G_PlaceExitPortal(arena);

    // Calculate world bounds
    VectorSet(arena->worldMins, -2048, -2048, -512);
    VectorSet(arena->worldMaxs, 2048, 2048, 512);

    arena->generationTime = (trap_Milliseconds() - startTime) / 1000.0f;

    G_Printf("Arena generated: %d rooms, %d corridors in %.2fs\n",
             arena->numRooms, arena->numCorridors, arena->generationTime);

    return arena;
}

void G_FreeArena(arena_t *arena) {
    if (!arena) return;

    // Free room linked list
    arenaRoom_t *room = arena->rooms;
    while (room) {
        arenaRoom_t *next = room->next;
        G_Free(room);
        room = next;
    }

    G_Free(arena);
}

//=================
// Entity Placement
//=================

void G_PlacePlayerSpawns(arena_t *arena, int numPlayers) {
    arenaRoom_t *room = arena->rooms;

    // Find start room
    while (room) {
        if (room->isStartRoom) {
            // Place spawns in start room
            int spawnsPlaced = 0;
            for (int i = 0; i < numPlayers && i < MAX_CLIENTS; i++) {
                vec3_t spawnPos;
                spawnPos[0] = room->origin[0] + ArenaRandRange(-64, 64);
                spawnPos[1] = room->origin[1] + ArenaRandRange(-64, 64);
                spawnPos[2] = room->origin[2] + 24; // 24 units off floor

                VectorCopy(spawnPos, arena->playerSpawns[spawnsPlaced]);
                spawnsPlaced++;
            }
            arena->numPlayerSpawns = spawnsPlaced;
            G_Printf("Placed %d player spawns\n", spawnsPlaced);
            return;
        }
        room = room->next;
    }
}

void G_PlaceEnemySpawns(arena_t *arena, int depth) {
    // Enemy count scales with depth
    int numEnemies = 5 + (depth * 3) + ArenaRandRange(0, depth);
    if (numEnemies > MAX_ARENA_ENTITIES) numEnemies = MAX_ARENA_ENTITIES;

    int enemiesPlaced = 0;
    arenaRoom_t *room = arena->rooms;

    while (room && enemiesPlaced < numEnemies) {
        // Don't spawn in start room
        if (room->isStartRoom) {
            room = room->next;
            continue;
        }

        // More enemies in larger rooms
        int roomEnemies = (room->width * room->depth) / (256 * 256);
        if (roomEnemies < 1) roomEnemies = 1;
        if (room->isBossRoom) roomEnemies = 1; // Just the boss

        for (int i = 0; i < roomEnemies && enemiesPlaced < numEnemies; i++) {
            vec3_t spawnPos;
            spawnPos[0] = room->origin[0] + ArenaRandRange(-room->width/3, room->width/3);
            spawnPos[1] = room->origin[1] + ArenaRandRange(-room->depth/3, room->depth/3);
            spawnPos[2] = room->origin[2] + 24;

            VectorCopy(spawnPos, arena->enemySpawns[enemiesPlaced]);
            enemiesPlaced++;
        }

        room = room->next;
    }

    arena->numEnemySpawns = enemiesPlaced;
    G_Printf("Placed %d enemy spawns\n", enemiesPlaced);
}

void G_PlaceItems(arena_t *arena, int depth) {
    // Item count scales with depth
    int numItems = 3 + depth + ArenaRandRange(0, 3);
    if (numItems > MAX_ARENA_ENTITIES / 2) numItems = MAX_ARENA_ENTITIES / 2;

    int itemsPlaced = 0;
    arenaRoom_t *room = arena->rooms;

    while (room && itemsPlaced < numItems) {
        if (!room->isStartRoom) {
            vec3_t itemPos;
            itemPos[0] = room->origin[0] + ArenaRandRange(-room->width/4, room->width/4);
            itemPos[1] = room->origin[1] + ArenaRandRange(-room->depth/4, room->depth/4);
            itemPos[2] = room->origin[2] + 24;

            VectorCopy(itemPos, arena->itemSpawns[itemsPlaced]);
            itemsPlaced++;
        }
        room = room->next;
    }

    arena->numItemSpawns = itemsPlaced;
    G_Printf("Placed %d item spawns\n", itemsPlaced);
}

void G_PlaceExitPortal(arena_t *arena) {
    arenaRoom_t *room = arena->rooms;

    // Find exit room
    while (room) {
        if (room->isExitRoom) {
            VectorCopy(room->origin, arena->exitPortal);
            arena->exitPortal[2] = room->origin[2] + 24;
            G_Printf("Placed exit portal at (%.0f, %.0f, %.0f)\n",
                     arena->exitPortal[0], arena->exitPortal[1], arena->exitPortal[2]);
            return;
        }
        room = room->next;
    }
}

//=================
// Roguelike Mode
//=================

roguelikeRun_t* G_StartRoguelikeRun(int seed, qboolean permadeath) {
    if (currentRun) {
        G_FreeArena(currentRun->currentArena);
        G_Free(currentRun);
    }

    currentRun = (roguelikeRun_t *)G_Alloc(sizeof(roguelikeRun_t));
    memset(currentRun, 0, sizeof(roguelikeRun_t));

    currentRun->seed = seed;
    currentRun->currentDepth = 1;
    currentRun->maxDepth = 1;
    currentRun->score = 0;
    currentRun->kills = 0;
    currentRun->permadeath = permadeath;
    currentRun->lives = permadeath ? 1 : 3;
    currentRun->timeElapsed = 0;

    // Generate first arena
    currentRun->currentArena = G_GenerateArena(seed, 1, THEME_TECH);

    G_Printf("Started roguelike run: seed=%d, permadeath=%d\n", seed, permadeath);

    return currentRun;
}

qboolean G_AdvanceToNextArena(roguelikeRun_t *run) {
    if (!run) return qfalse;

    // Free current arena
    if (run->currentArena) {
        G_FreeArena(run->currentArena);
    }

    // Advance depth
    run->currentDepth++;
    if (run->currentDepth > run->maxDepth) {
        run->maxDepth = run->currentDepth;
    }

    // Vary theme every few levels
    arenaTheme_t theme = THEME_TECH;
    if (run->currentDepth >= 10) theme = THEME_HELL;
    else if (run->currentDepth >= 6) theme = THEME_SPACE;
    else if (run->currentDepth >= 3) theme = THEME_GOTHIC;

    // Generate next arena
    run->currentArena = G_GenerateArena(run->seed, run->currentDepth, theme);

    G_Printf("Advanced to arena depth %d\n", run->currentDepth);

    return qtrue;
}

void G_EndRoguelikeRun(roguelikeRun_t *run, qboolean victory) {
    if (!run) return;

    G_Printf("Roguelike run ended: depth=%d, score=%d, kills=%d, time=%.1fs, victory=%d\n",
             run->maxDepth, run->score, run->kills, run->timeElapsed, victory);

    // TODO: Save to leaderboard

    if (run->currentArena) {
        G_FreeArena(run->currentArena);
    }

    if (run == currentRun) {
        currentRun = NULL;
    }

    G_Free(run);
}

roguelikeRun_t* G_GetCurrentRun(void) {
    return currentRun;
}

//=================
// Debug & Export
//=================

void G_PrintArenaInfo(arena_t *arena) {
    if (!arena) return;

    G_Printf("=== Arena Info ===\n");
    G_Printf("Seed: %d, Depth: %d, Theme: %s\n", arena->seed, arena->depth, G_GetThemeName(arena->theme));
    G_Printf("Rooms: %d, Corridors: %d\n", arena->numRooms, arena->numCorridors);
    G_Printf("Player Spawns: %d, Enemy Spawns: %d, Items: %d\n",
             arena->numPlayerSpawns, arena->numEnemySpawns, arena->numItemSpawns);
    G_Printf("Generation Time: %.2fs\n", arena->generationTime);

    arenaRoom_t *room = arena->rooms;
    int idx = 0;
    while (room) {
        G_Printf("  Room %d: %s (%.0f, %.0f, %.0f) size=%dx%dx%d\n",
                 idx, G_GetRoomTypeName(room->type),
                 room->origin[0], room->origin[1], room->origin[2],
                 room->width, room->height, room->depth);
        room = room->next;
        idx++;
    }
}

int G_CalculateArenaDifficulty(arena_t *arena) {
    if (!arena) return 0;

    // Simple difficulty score
    return arena->depth * 100 + arena->numEnemySpawns * 10 + arena->numRooms;
}

// TODO: Full .map export for debugging
void G_ExportArenaToMap(arena_t *arena, const char *filename) {
    G_Printf("Arena export to .map not yet implemented\n");
    // This would write a full Q3 .map file with brushes, entities, etc.
}
