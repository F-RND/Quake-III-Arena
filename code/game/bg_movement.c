/*
===========================================================================
Advanced Movement System Implementation

Modern FPS movement mechanics implementation
===========================================================================
*/

#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"
#include "bg_movement.h"

// External pmove parameters
extern float pm_stopspeed;

/*
=================
BG_ForceLegsAnim

Helper to force legs animation
=================
*/
static void BG_ForceLegsAnim( pmove_t *pm, int anim ) {
	pm->ps->legsTimer = 0;
	pm->ps->legsAnim = ( ( pm->ps->legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
}

/*
=================
BG_IsInAir

Check if player is in the air
=================
*/
qboolean BG_IsInAir( playerState_t *ps ) {
	return ps->groundEntityNum == ENTITYNUM_NONE;
}

/*
=================
BG_IsMoving

Check if player is moving faster than minSpeed
=================
*/
qboolean BG_IsMoving( playerState_t *ps, float minSpeed ) {
	vec3_t horizontal;
	VectorCopy( ps->velocity, horizontal );
	horizontal[2] = 0;
	return VectorLength( horizontal ) >= minSpeed;
}

/*
=================
BG_GetMovementDirection

Get direction from command input (0-7 for 8 directions, -1 for none)
=================
*/
int BG_GetMovementDirection( usercmd_t *cmd ) {
	int forward = cmd->forwardmove;
	int right = cmd->rightmove;

	if ( forward == 0 && right == 0 ) {
		return -1; // No movement
	}

	// 8 directions:
	// 0 = forward, 1 = forward-right, 2 = right, 3 = back-right,
	// 4 = back, 5 = back-left, 6 = left, 7 = forward-left

	if ( forward > 0 && right == 0 ) return 0;
	if ( forward > 0 && right > 0 ) return 1;
	if ( forward == 0 && right > 0 ) return 2;
	if ( forward < 0 && right > 0 ) return 3;
	if ( forward < 0 && right == 0 ) return 4;
	if ( forward < 0 && right < 0 ) return 5;
	if ( forward == 0 && right < 0 ) return 6;
	if ( forward > 0 && right < 0 ) return 7;

	return 0; // Default forward
}

/*
=================
BG_ApplyFriction

Apply friction to velocity
Note: frametime parameter should be passed from pmove
=================
*/
void BG_ApplyFriction( vec3_t velocity, float friction, float frametime ) {
	float speed, newspeed, control, drop;
	vec3_t vec;

	VectorCopy( velocity, vec );
	vec[2] = 0; // Ignore vertical

	speed = VectorLength( vec );
	if ( speed < 1.0f ) {
		velocity[0] = 0;
		velocity[1] = 0;
		return;
	}

	control = speed < pm_stopspeed ? pm_stopspeed : speed;
	drop = control * friction * frametime;

	newspeed = speed - drop;
	if ( newspeed < 0 ) newspeed = 0;
	newspeed /= speed;

	VectorScale( velocity, newspeed, velocity );
}

//=================
// DOUBLE JUMP
//=================

/*
=================
BG_CanDoubleJump

Check if player can perform double jump
=================
*/
qboolean BG_CanDoubleJump( playerState_t *ps ) {
	// Must be in air
	if ( !BG_IsInAir( ps ) ) {
		return qfalse;
	}

	// Haven't already double jumped
	if ( ps->pm_flags & PMF_DOUBLE_JUMPED ) {
		return qfalse;
	}

	// Jump button not held from first jump (prevent accidental double jump)
	if ( ps->pm_flags & PMF_JUMP_HELD ) {
		return qfalse;
	}

	return qtrue;
}

/*
=================
BG_DoDoubleJump

Execute double jump
=================
*/
void BG_DoDoubleJump( playerState_t *ps, pmove_t *pm ) {
	ps->velocity[2] = PM_DOUBLEJUMP_VELOCITY * PM_DOUBLEJUMP_SCALE;
	ps->pm_flags |= PMF_DOUBLE_JUMPED;
	ps->pm_flags |= PMF_JUMP_HELD;

	// Add event for sound/visual feedback
	PM_AddEvent( EV_JUMP );

	// Force legs animation
	BG_ForceLegsAnim( pm, LEGS_JUMP );
}

//=================
// DODGE/ROLL
//=================

/*
=================
BG_CanDodge

Check if player can dodge
=================
*/
qboolean BG_CanDodge( playerState_t *ps, int time ) {
	// Already dodging
	if ( ps->pm_flags & PMF_DODGING ) {
		return qfalse;
	}

	// Check cooldown
	int lastDodge = PS_DODGETIME( ps );
	if ( time - lastDodge < PM_DODGE_COOLDOWN ) {
		return qfalse;
	}

	return qtrue;
}

/*
=================
BG_DoDodge

Execute dodge/roll in given direction
=================
*/
void BG_DoDodge( playerState_t *ps, pmove_t *pm, int direction ) {
	vec3_t dodgeDir;
	float yaw;

	// Calculate dodge direction based on view angle and input
	yaw = ps->viewangles[YAW] * (M_PI / 180.0f);

	// Direction angles (in radians)
	float dirAngle = yaw + (direction * M_PI / 4.0f);

	dodgeDir[0] = cos( dirAngle );
	dodgeDir[1] = sin( dirAngle );
	dodgeDir[2] = 0;

	VectorNormalize( dodgeDir );
	VectorScale( dodgeDir, PM_DODGE_SPEED, dodgeDir );

	// Apply dodge velocity
	VectorCopy( dodgeDir, ps->velocity );

	// Set dodge state
	ps->pm_flags |= PMF_DODGING;
	PS_DODGETIME( ps ) = pm->cmd.serverTime;
	ps->pm_time = PM_DODGE_DURATION;

	// Add event
	PM_AddEvent( EV_FOOTSTEP ); // TODO: Add EV_DODGE event

	// Force animation based on direction
	if ( direction == 0 || direction == 1 || direction == 7 ) {
		BG_ForceLegsAnim( pm, LEGS_JUMP ); // TODO: Add LEGS_DODGE_FORWARD
	} else if ( direction == 4 || direction == 3 || direction == 5 ) {
		BG_ForceLegsAnim( pm, LEGS_JUMPB ); // TODO: Add LEGS_DODGE_BACK
	} else {
		BG_ForceLegsAnim( pm, LEGS_JUMP ); // TODO: Add LEGS_DODGE_SIDE
	}
}

//=================
// SLIDE
//=================

/*
=================
BG_CanSlide

Check if player can slide
=================
*/
qboolean BG_CanSlide( playerState_t *ps ) {
	// Must be on ground
	if ( BG_IsInAir( ps ) ) {
		return qfalse;
	}

	// Must be moving fast enough
	if ( !BG_IsMoving( ps, PM_SLIDE_MIN_SPEED ) ) {
		return qfalse;
	}

	// Not already sliding
	if ( ps->pm_flags & PMF_SLIDING ) {
		return qfalse;
	}

	return qtrue;
}

/*
=================
BG_DoSlide

Execute slide
=================
*/
void BG_DoSlide( playerState_t *ps, pmove_t *pm ) {
	// Set slide state
	ps->pm_flags |= PMF_SLIDING;
	PS_SLIDETIME( ps ) = pm->cmd.serverTime;
	ps->pm_time = PM_SLIDE_DURATION;

	// Lower view height (default is 26)
	ps->viewheight = 26 * PM_SLIDE_VIEWHEIGHT_SCALE;

	// Apply speed boost in current movement direction
	vec3_t forward, right, slideDir;
	float yaw = ps->viewangles[YAW] * (M_PI / 180.0f);

	forward[0] = cos( yaw );
	forward[1] = sin( yaw );
	forward[2] = 0;

	right[0] = -sin( yaw );
	right[1] = cos( yaw );
	right[2] = 0;

	VectorScale( forward, pm->cmd.forwardmove, slideDir );
	VectorMA( slideDir, pm->cmd.rightmove, right, slideDir );
	VectorNormalize( slideDir );

	// Boost speed
	float currentSpeed = VectorLength( ps->velocity );
	if ( currentSpeed < PM_SLIDE_SPEED ) {
		VectorScale( slideDir, PM_SLIDE_SPEED, ps->velocity );
	}

	// Add event
	PM_AddEvent( EV_FOOTSTEP ); // TODO: Add EV_SLIDE event

	// Force animation
	BG_ForceLegsAnim( pm, LEGS_IDLECR ); // TODO: Add LEGS_SLIDE animation
}

//=================
// WALL RUNNING
//=================

/*
=================
BG_FindWallRunSurface

Find nearby wall suitable for wall running
Returns qtrue and sets wallNormal if found
=================
*/
qboolean BG_FindWallRunSurface( pmove_t *pm, vec3_t wallNormal ) {
	trace_t trace;
	vec3_t point, forward, right;
	int i;

	// Get view angles
	AngleVectors( pm->ps->viewangles, forward, right, NULL );

	// Try left and right sides
	vec3_t directions[2];
	VectorScale( right, -1, directions[0] ); // Left
	VectorCopy( right, directions[1] ); // Right

	for ( i = 0; i < 2; i++ ) {
		VectorMA( pm->ps->origin, PM_WALLRUN_DETECT_DIST, directions[i], point );

		pm->trace( &trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask );

		if ( trace.fraction < 1.0f && !trace.allsolid ) {
			// Check if surface is suitable (near vertical)
			float angle = acos( trace.plane.normal[2] ) * (180.0f / M_PI);

			if ( angle >= PM_WALLRUN_MIN_ANGLE && angle <= PM_WALLRUN_MAX_ANGLE ) {
				VectorCopy( trace.plane.normal, wallNormal );
				return qtrue;
			}
		}
	}

	return qfalse;
}

/*
=================
BG_CanWallRun

Check if player can wall run
=================
*/
qboolean BG_CanWallRun( playerState_t *ps, vec3_t wallNormal ) {
	// Must be in air
	if ( !BG_IsInAir( ps ) ) {
		return qfalse;
	}

	// Must be moving
	if ( !BG_IsMoving( ps, 100 ) ) {
		return qfalse;
	}

	// Not already wall running
	if ( ps->pm_flags & PMF_WALL_RUNNING ) {
		return qfalse;
	}

	// Wall normal must be set
	if ( VectorLength( wallNormal ) < 0.1f ) {
		return qfalse;
	}

	return qtrue;
}

/*
=================
BG_DoWallRun

Execute wall run
=================
*/
void BG_DoWallRun( playerState_t *ps, pmove_t *pm, vec3_t wallNormal ) {
	vec3_t up = {0, 0, 1};
	vec3_t forward, runDir;

	// Set wall run state
	ps->pm_flags |= PMF_WALL_RUNNING;
	PS_WALLRUNTIME( ps ) = pm->cmd.serverTime;
	ps->pm_time = PM_WALLRUN_DURATION;

	// Calculate run direction (perpendicular to wall normal and up vector)
	CrossProduct( wallNormal, up, forward );
	VectorNormalize( forward );

	// Match direction to current velocity
	if ( DotProduct( ps->velocity, forward ) < 0 ) {
		VectorNegate( forward, forward );
	}

	VectorScale( forward, PM_WALLRUN_SPEED, runDir );
	VectorCopy( runDir, ps->velocity );

	// Neutralize gravity while wall running
	ps->velocity[2] = 0;

	// Add event
	PM_AddEvent( EV_FOOTSTEP ); // TODO: Add EV_WALLRUN event
}

//=================
// BULLET TIME
//=================

/*
=================
BG_UpdateBulletTime

Update bullet time energy and state
Called every frame with msec = frame time in milliseconds
=================
*/
void BG_UpdateBulletTime( playerState_t *ps, int msec, qboolean active ) {
	int *energy = &PS_BULLETTIME_ENERGY( ps );

	// Initialize energy if needed
	if ( *energy == 0 && !active ) {
		*energy = PM_BULLETTIME_MAX_ENERGY;
	}

	if ( active ) {
		// Drain energy
		int drain = (PM_BULLETTIME_DRAIN_RATE * msec) / 1000;
		*energy -= drain;

		if ( *energy <= 0 ) {
			*energy = 0;
			ps->pm_flags &= ~PMF_BULLET_TIME;
		} else {
			ps->pm_flags |= PMF_BULLET_TIME;
		}
	} else {
		// Recharge energy
		int recharge = (PM_BULLETTIME_RECHARGE_RATE * msec) / 1000;
		*energy += recharge;

		if ( *energy > PM_BULLETTIME_MAX_ENERGY ) {
			*energy = PM_BULLETTIME_MAX_ENERGY;
		}

		ps->pm_flags &= ~PMF_BULLET_TIME;
	}
}
