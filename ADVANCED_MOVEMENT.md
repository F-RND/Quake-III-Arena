# Advanced Movement System Design

## Overview
Transform Quake III Arena into a modern fast-paced indie FPS with advanced movement mechanics inspired by games like Titanfall, Doom Eternal, and Dark Souls.

## Movement Mechanics Summary

### 1. Bullet Time
- **Activation**: Hold Z + Shoot
- **Effect**: Slow down time (0.3x speed) while maintaining player speed
- **Duration**: Limited by energy meter
- **Visual**: Slow-motion effects, trails, sound pitch down

### 2. Double Jump
- **Activation**: Press Space twice
- **Effect**: Second jump in midair with 0.8x height of first jump
- **Cooldown**: Resets on landing

### 3. Dodge/Roll
- **Activation**: Shift + Movement Direction
- **Effect**: Quick burst of speed in that direction (Dark Souls style)
- **Features**:
  - Invulnerability frames during roll
  - Short cooldown (0.5s)
  - Can be chained with other moves

### 4. Wall Running
- **Activation**: C + Space + Shift (near wall)
- **Effect**: Run along vertical surfaces
- **Features**:
  - Automatically detects nearby walls
  - Maintains momentum
  - Limited duration (3 seconds)
  - Jump off wall for boost

### 5. Sliding
- **Activation**: Ctrl + Space + Shift (while moving)
- **Effect**: Slide along ground, maintaining momentum
- **Features**:
  - Can shoot while sliding
  - Smaller hitbox
  - Speed boost on slopes
  - Can transition into jump

### 6. Grappling Hook
- **Activation**: Weapon slot (already exists in Q3A!)
- **Enhancement**: Improve existing grapple
- **Features**:
  - Faster pull speed
  - Can grapple enemies
  - Momentum-based swinging

### 7. Portal Gun
- **Activation**: New weapon slot
- **Effect**: Create two portals (entry/exit)
- **Features**:
  - Momentum preserved through portals
  - Can see through portals
  - Portals persist until new ones placed

## Technical Implementation

### New Button Flags (q_shared.h)
```c
#define BUTTON_MODIFIER1    4096    // Z key - bullet time modifier
#define BUTTON_MODIFIER2    8192    // C key - wall run modifier
#define BUTTON_MODIFIER3    16384   // Ctrl key - slide modifier
#define BUTTON_DODGE        32768   // Shift key - dodge/roll
```

### New Player Movement Flags (bg_public.h)
```c
#define PMF_DOUBLE_JUMPED   32768   // Used second jump
#define PMF_WALL_RUNNING    65536   // Currently wall running
#define PMF_SLIDING         131072  // Currently sliding
#define PMF_DODGING         262144  // Currently dodging/rolling
#define PMF_BULLET_TIME     524288  // Bullet time active
```

### New PlayerState Fields
Need to extend playerState_t (or use generic/powerup slots):
- `jumpCount` - Track jumps for double jump
- `dodgeTime` - Last dodge timestamp
- `wallRunTime` - Wall run duration
- `bulletTimeEnergy` - Bullet time resource (0-100)
- `wallRunSurface` - Which wall we're running on

## Movement State Machine

```
IDLE/GROUND
├─> JUMP (Space)
│   ├─> DOUBLE_JUMP (Space again)
│   ├─> WALL_RUN (C+Space+Shift near wall)
│   └─> DODGE (Shift+Direction)
│
├─> SLIDE (Ctrl+Space+Shift)
│   ├─> JUMP (maintain momentum)
│   └─> DODGE (quick recovery)
│
└─> DODGE (Shift+Direction)
    └─> Any move (cooldown)
```

## Physics Parameters

### Double Jump
- `pm_doublejumpspeed = 270` (vs normal 270)
- `pm_doublejumpheight = 0.8` (80% of first jump)

### Wall Running
- `pm_wallrunspeed = 400` (faster than normal)
- `pm_wallrunduration = 3000` (3 seconds max)
- `pm_wallrunangle = 15` (degrees from vertical)

### Sliding
- `pm_slidespeed = 450` (initial boost)
- `pm_slidefriction = 3.0` (slow down over time)
- `pm_slideduration = 1500` (1.5 seconds max)

### Dodge/Roll
- `pm_dodgespeed = 500` (quick burst)
- `pm_dodgeduration = 400` (400ms i-frames)
- `pm_dodgecooldown = 500` (500ms between dodges)

### Bullet Time
- `pm_bullettimescale = 0.3` (30% time speed)
- `pm_bullettimeenergy = 100` (max energy)
- `pm_bullettim edrain = 20` (per second)
- `pm_bullettimerecharge = 10` (per second when inactive)

## File Structure

### New Files
- `code/game/bg_movement.c` - Advanced movement implementation
- `code/game/bg_movement.h` - Movement API and constants
- `code/game/g_portalgun.c` - Portal gun mechanics
- `code/game/g_bullettime.c` - Bullet time system

### Modified Files
- `code/game/q_shared.h` - Add button flags and playerState fields
- `code/game/bg_public.h` - Add PMF flags
- `code/game/bg_pmove.c` - Integrate new movement calls
- `code/game/g_active.c` - Update player think for bullet time

## Animation System

### New Animations Needed
- `LEGS_DOUBLE_JUMP` - Second jump animation
- `LEGS_WALL_RUN` - Wall running animation
- `LEGS_SLIDE` - Sliding animation
- `LEGS_DODGE_F/B/L/R` - Dodge rolls in 4 directions
- `TORSO_SLIDE_ATTACK` - Shooting while sliding

## Network Considerations

### Delta Compression
New fields must be added to network protocol:
- Movement flags (PMF_*)
- Bullet time state
- Wall run surface normal
- Portal locations

### Client Prediction
All movement must be fully predictable on client:
- Same physics on client/server
- Deterministic random (for effects only)
- No server-authoritative movement changes

## Progression

### Phase 1: Core Movement (THIS PHASE)
1. Add button/flag definitions
2. Implement double jump
3. Implement dodge/roll
4. Implement slide

### Phase 2: Advanced Movement
5. Implement wall running
6. Enhance grappling hook
7. Add bullet time

### Phase 3: Weapons
8. Implement portal gun
9. Balance all mechanics

## Testing Commands

```
/god - invulnerability for testing
/noclip - test wall detection
/timescale 0.5 - test bullet time visually
/give all - test with all weapons
```

## Future Enhancements

- **Ledge Grab**: Hang from edges (E key)
- **Dash**: Quick forward burst (Q key)
- **Ground Pound**: Slam down from air (Ctrl in air)
- **Air Strafe**: Enhanced air control
- **Bunny Hop**: Maintain speed across jumps
