/* ------------------------------------------------------------------------- *\
   SINT.H :

   Definition of a signed integer type (and related types)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _SINT_
#define _SINT_

#include "tool.h"

/*
** By default, signed integers are defined in double precision (ie long)
** To get single precision, simply add "#define SINGLE_SINT" in your program
*/

/*
** Scalar type definition (single or double precision)
*/

#ifdef SINGLE_SINT

typedef short sint;

#define SINT1FILE "%d\n"
#define SINT2FILE "%d %d\n"
#define SINT3FILE "%d %d %d\n"
#define SINT4FILE "%d %d %d %d\n"

#else

typedef long sint;

#define SINT1FILE "%ld\n"
#define SINT2FILE "%ld %ld\n"
#define SINT3FILE "%ld %ld %ld\n"
#define SINT4FILE "%ld %ld %ld %ld\n"

#endif

/*
** Vector type definition
*/

typedef struct {
  sint x,y;
} sintvec2;                         /* 2D vector of signed integers */

typedef struct {
  sint x,y,z;
} sintvec3;                         /* 3D vector of signed integers */

typedef struct {
  sint x,y,z,w;
} sintvec4;                         /* 4D vector of signed integers */

/*
** Matrix type definition
*/

typedef sintvec2   sintmat2[2];     /* 2D matrix of signed integers */

typedef sintvec3   sintmat3[3];     /* 3D matrix of signed integers */

typedef sintvec4   sintmat4[4];     /* 4D matrix of signed integers */

/*
** Aliases for lazy programmers
*/

typedef sintvec2 sv2;
typedef sintvec3 sv3;
typedef sintvec4 sv4;
typedef sintmat2 sm2;
typedef sintmat3 sm3;
typedef sintmat4 sm4;

/*
** Get values from file
*/

#define GET_SINT(File,Var)\
        (fscanf (File, SINT1FILE, &(Var)))

#define GET_SINTVEC2(File,Var)\
        (fscanf (File, SINT2FILE, &(Var).x, &(Var).y))

#define GET_SINTVEC3(File,Var)\
        (fscanf (File, SINT3FILE, &(Var).x, &(Var).y, &(Var).z))

#define GET_SINTVEC4(File,Var)\
        (fscanf (File, SINT4FILE, &(Var).x, &(Var).y, &(Var).z, &(Var).w))

#define GET_SINTMAT2(File,Var)\
        (GET_SINTVEC2(File,(Var)[0]),\
         GET_SINTVEC2(File,(Var)[1]))

#define GET_SINTMAT3(File,Var)\
        (GET_SINTVEC3(File,(Var)[0]),\
         GET_SINTVEC3(File,(Var)[1]),\
         GET_SINTVEC3(File,(Var)[2]))

#define GET_SINTMAT4(File,Var)\
        (GET_SINTVEC4(File,(Var)[0]),\
         GET_SINTVEC4(File,(Var)[1]),\
         GET_SINTVEC4(File,(Var)[2]),\
         GET_SINTVEC4(File,(Var)[3]))

/*
** Put values in file
*/

#define PUT_SINT(File,Var)\
        (fprintf (File, SINT1FILE, (Var)))

#define PUT_SINTVEC2(File,Var)\
        (fprintf (File, SINT2FILE, (Var).x, (Var).y))

#define PUT_SINTVEC3(File,Var)\
        (fprintf (File, SINT3FILE, (Var).x, (Var).y, (Var).z))

#define PUT_SINTVEC4(File,Var)\
        (fprintf (File, SINT4FILE, (Var).x, (Var).y, (Var).z, (Var).w))

#define PUT_SINTMAT2(File,Var)\
        (PUT_SINTVEC2 (File,(Var)[0]),\
         PUT_SINTVEC2 (File,(Var)[1]))

#define PUT_SINTMAT3(File,Var)\
        (PUT_SINTVEC3(File,(Var)[0]),\
         PUT_SINTVEC3(File,(Var)[1]),\
         PUT_SINTVEC3(File,(Var)[2]))

#define PUT_SINTMAT4(File,Var)\
        (PUT_SINTVEC4(File,(Var)[0]),\
         PUT_SINTVEC4(File,(Var)[1]),\
         PUT_SINTVEC4(File,(Var)[2]),\
         PUT_SINTVEC4(File,(Var)[3]))

#endif

/* ------------------------------------------------------------------------- */
