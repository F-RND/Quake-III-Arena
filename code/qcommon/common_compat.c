/*
===========================================================================
Compatibility stubs for modernized build
===========================================================================
*/

#include <stdlib.h>
#include <math.h>
#include "../game/q_shared.h"
#include "qcommon.h"

/*
==============
Com_Allocate
Memory allocation wrapper for splines library
==============
*/
void *Com_Allocate( int bytes ) {
    void *buf = malloc( bytes );
    if ( !buf ) {
        Com_Error( ERR_FATAL, "Com_Allocate: failed on allocation of %i bytes", bytes );
    }
    return buf;
}

/*
==============
Com_Dealloc
Memory deallocation wrapper for splines library
==============
*/
void Com_Dealloc( void *ptr ) {
    if ( ptr ) {
        free( ptr );
    }
}

/*
==============
Sys_SnapVector
Snap float vector to integer precision
Used for network-consistent rounding behavior

This is the C fallback for 64-bit where we don't have
the NASM assembly version
==============
*/
#if !defined(__APPLE__) && !defined(__i386__)
void Sys_SnapVector( float *v ) {
    v[0] = rint(v[0]);
    v[1] = rint(v[1]);
    v[2] = rint(v[2]);
}
#endif
