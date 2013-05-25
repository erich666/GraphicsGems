/* ------------------------------------------------------------------------- *\
   UINT.H :

   Definition of an unsigned integer type (and related types)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _UINT_
#define _UINT_

#include "tool.h"

/*
** By default, unsigned integers are defined in double precision (ie ulong)
** To get single precision, simply add "#define SINGLE_UINT" in your program
*/

/*
** Scalar type definition (single or double precision)
*/

#ifdef SINGLE_UINT

typedef unsigned short uint;

#define UINT1FILE "%u\n"
#define UINT2FILE "%u %u\n"
#define UINT3FILE "%u %u %u\n"
#define UINT4FILE "%u %u %u %u\n"

#else

typedef unsigned long uint;

#define UINT1FILE "%lu\n"
#define UINT2FILE "%lu %lu\n"
#define UINT3FILE "%lu %lu %lu\n"
#define UINT4FILE "%lu %lu %lu %lu\n"

#endif

/*
** Vector type definition
*/

typedef struct {
  uint x,y;
} uintvec2;                         /* 2D vector of signed integers */

typedef struct {
  uint x,y,z;
} uintvec3;                         /* 3D vector of signed integers */

typedef struct {
  uint x,y,z,w;
} uintvec4;                         /* 4D vector of signed integers */

/*
** Matrix type definition
*/

typedef uintvec2 uintmat2[2];       /* 2D matrix of signed integers */

typedef uintvec3 uintmat3[3];       /* 3D matrix of signed integers */

typedef uintvec4 uintmat4[4];       /* 4D matrix of signed integers */

/*
** Aliases for lazy programmers
*/

typedef uintvec2 uv2;
typedef uintvec3 uv3;
typedef uintvec4 uv4;
typedef uintmat2 um2;
typedef uintmat3 um3;
typedef uintmat4 um4;

/*
** Get values from file
*/

#define GET_UINT(File,Var)\
        (fscanf (File, UINT1FILE, &(Var)))

#define GET_UINTVEC2(File,Var)\
        (fscanf (File, UINT2FILE, &(Var).x, &(Var).y))

#define GET_UINTVEC3(File,Var)\
        (fscanf (File, UINT3FILE, &(Var).x, &(Var).y, &(Var).z))

#define GET_UINTVEC4(File,Var)\
        (fscanf (File, UINT4FILE, &(Var).x, &(Var).y, &(Var).z, &(Var).w))

#define GET_UINTMAT2(File,Var)\
        (GET_UINTVEC2(File,(Var)[0]),\
         GET_UINTVEC2(File,(Var)[1]))

#define GET_UINTMAT3(File,Var)\
        (GET_UINTVEC3(File,(Var)[0]),\
         GET_UINTVEC3(File,(Var)[1]),\
         GET_UINTVEC3(File,(Var)[2]))

#define GET_UINTMAT4(File,Var)\
        (GET_UINTVEC4(File,(Var)[0]),\
         GET_UINTVEC4(File,(Var)[1]),\
         GET_UINTVEC4(File,(Var)[2]),\
         GET_UINTVEC4(File,(Var)[3]))

/*
** Put values in file
*/

#define PUT_UINT(File,Var)\
        (fprintf (File, UINT1FILE, (Var)))

#define PUT_UINTVEC2(File,Var)\
        (fprintf (File, UINT2FILE, (Var).x, (Var).y))

#define PUT_UINTVEC3(File,Var)\
        (fprintf (File, UINT3FILE, (Var).x, (Var).y, (Var).z))

#define PUT_UINTVEC4(File,Var)\
        (fprintf (File, UINT4FILE, (Var).x, (Var).y, (Var).z, (Var).w))

#define PUT_UINTMAT2(File,Var)\
        (PUT_UINTVEC2 (File,(Var)[0]),\
         PUT_UINTVEC2 (File,(Var)[1]))

#define PUT_UINTMAT3(File,Var)\
        (PUT_UINTVEC3(File,(Var)[0]),\
         PUT_UINTVEC3(File,(Var)[1]),\
         PUT_UINTVEC3(File,(Var)[2]))

#define PUT_UINTMAT4(File,Var)\
        (PUT_UINTVEC4(File,(Var)[0]),\
         PUT_UINTVEC4(File,(Var)[1]),\
         PUT_UINTVEC4(File,(Var)[2]),\
         PUT_UINTVEC4(File,(Var)[3]))

#endif

/* ------------------------------------------------------------------------- */
