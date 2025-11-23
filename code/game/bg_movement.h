/*
===========================================================================
Advanced Movement System for Quake III Arena

Modern FPS movement mechanics:
- Double Jump
- Dodge/Roll
- Wall Running
- Sliding
- Bullet Time
===========================================================================
*/

#ifndef BG_MOVEMENT_H
#define BG_MOVEMENT_H

// Forward declarations (avoid including headers to prevent circular dependencies)
struct playerState_s;
struct pmove_s;
struct usercmd_s;

// Use existing typedefs from q_shared.h
#ifndef __Q_SHARED_H
typedef struct playerState_s playerState_t;
typedef struct pmove_s pmove_t;
typedef struct usercmd_s usercmd_t;
typedef int qboolean;
typedef float vec_t;
typedef vec_t vec3_t[3];
#endif

// Movement parameters - tweakable for game feel
#define PM_DOUBLEJUMP_VELOCITY		270		// Same as normal jump
#define PM_DOUBLEJUMP_SCALE			0.8f	// 80% height of first jump

#define PM_WALLRUN_SPEED			400		// Speed while wall running
#define PM_WALLRUN_DURATION			3000	// Max 3 seconds
#define PM_WALLRUN_DETECT_DIST		32		// Wall detection range
#define PM_WALLRUN_JUMP_BOOST		1.3f	// Boost when jumping off wall
#define PM_WALLRUN_MIN_ANGLE		60		// Min angle from vertical (degrees)
#define PM_WALLRUN_MAX_ANGLE		120		// Max angle from vertical (degrees)

#define PM_SLIDE_SPEED				450		// Initial slide speed boost
#define PM_SLIDE_FRICTION			3.0f	// Friction while sliding
#define PM_SLIDE_DURATION			1500	// Max 1.5 seconds
#define PM_SLIDE_MIN_SPEED			200		// Must be moving to slide
#define PM_SLIDE_VIEWHEIGHT_SCALE	0.6f	// Crouch lower while sliding

#define PM_DODGE_SPEED				500		// Dodge burst speed
#define PM_DODGE_DURATION			400		// 400ms dodge
#define PM_DODGE_COOLDOWN			500		// 500ms between dodges
#define PM_DODGE_IFRAME_DURATION	300		// 300ms invulnerability

#define PM_BULLETTIME_SCALE			0.3f	// 30% time speed
#define PM_BULLETTIME_MAX_ENERGY	100		// Max energy
#define PM_BULLETTIME_DRAIN_RATE	20		// Per second when active
#define PM_BULLETTIME_RECHARGE_RATE	10		// Per second when inactive
#define PM_BULLETTIME_MIN_ACTIVATE	30		// Min energy to activate

// Extended player state (using powerup slots 11-15, which are unused in base Q3A)
// Note: PW_NUM_POWERUPS is 16, so slots 0-10 are standard powerups
#define PS_JUMPCOUNT(ps)			((ps)->powerups[15])
#define PS_DODGETIME(ps)			((ps)->powerups[14])
#define PS_WALLRUNTIME(ps)			((ps)->powerups[13])
#define PS_SLIDETIME(ps)			((ps)->powerups[12])
#define PS_BULLETTIME_ENERGY(ps)	((ps)->powerups[11])

// Movement state queries
qboolean	BG_CanDoubleJump( playerState_t *ps );
qboolean	BG_CanDodge( playerState_t *ps, int time );
qboolean	BG_CanSlide( playerState_t *ps );
qboolean	BG_CanWallRun( playerState_t *ps, vec3_t wallNormal );
qboolean	BG_IsInAir( playerState_t *ps );
qboolean	BG_IsMoving( playerState_t *ps, float minSpeed );

// Movement execution
void		BG_DoDoubleJump( playerState_t *ps, pmove_t *pm );
void		BG_DoDodge( playerState_t *ps, pmove_t *pm, int direction );
void		BG_DoSlide( playerState_t *ps, pmove_t *pm );
void		BG_DoWallRun( playerState_t *ps, pmove_t *pm, vec3_t wallNormal );
void		BG_UpdateBulletTime( playerState_t *ps, int msec, qboolean active );

// Wall detection
qboolean	BG_FindWallRunSurface( pmove_t *pm, vec3_t wallNormal );

// Utility
int			BG_GetMovementDirection( usercmd_t *cmd );
void		BG_ApplyFriction( vec3_t velocity, float friction, float frametime );

#endif // BG_MOVEMENT_H
