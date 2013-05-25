/* ------------------------------------------------------------------------- *\
   REAL.H :

   Definition of a real number type (and related types)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _REAL_
#define _REAL_

#include "tool.h"

/*
** By default, real numbers are defined in double precision (ie double)
** To get single precision, simply add "#define SINGLE_REAL" in your program
*/

/*
** Scalar type definition (single or double precision)
*/

#ifdef SINGLE_REAL

typedef float real;

#define REAL1FILE "%g\n"
#define REAL2FILE "%g %g\n"
#define REAL3FILE "%g %g %g\n"
#define REAL4FILE "%g %g %g %g\n"

#else

typedef double real;

#define REAL1FILE "%lg\n"
#define REAL2FILE "%lg %lg\n"
#define REAL3FILE "%lg %lg %lg\n"
#define REAL4FILE "%lg %lg %lg %lg\n"

#endif

/*
** Vector type definition
*/

typedef struct {
  real x,y;
} realvec2;                       /* 2D vector of reals */

typedef struct {
  real x,y,z;
} realvec3;                       /* 3D vector of reals */

typedef struct {
  real x,y,z,w;
} realvec4;                       /* 4D vector of reals */

/*
** Matrix type definition
*/

typedef realvec2 realmat2[2];     /* 2D matrix of reals */

typedef realvec3 realmat3[3];     /* 3D matrix of reals */

typedef realvec4 realmat4[4];     /* 4D matrix of reals */

/*
** Frame type definition
*/

typedef realvec2 frame2[5];       /* 2D cartesian frame */

typedef realvec3 frame3[7];       /* 3D cartesian frame */

typedef realvec4 frame4[9];       /* 4D cartesian frame */

/*
** Aliases for lazy programmers
*/

typedef realvec2 rv2;
typedef realvec3 rv3;
typedef realvec4 rv4;
typedef realmat2 rm2;
typedef realmat3 rm3;
typedef realmat4 rm4;

/*
** Get values from file
*/

#define GET_REAL(File,Var)\
        (fscanf (File, REAL1FILE, &(Var)))

#define GET_REALVEC2(File,Var)\
        (fscanf (File, REAL2FILE, &(Var).x, &(Var).y))

#define GET_REALVEC3(File,Var)\
        (fscanf (File, REAL3FILE, &(Var).x, &(Var).y, &(Var).z))

#define GET_REALVEC4(File,Var)\
        (fscanf (File, REAL4FILE, &(Var).x, &(Var).y, &(Var).z, &(Var).w))

#define GET_REALMAT2(File,Var)\
        (GET_REALVEC2(File,(Var)[0]),\
         GET_REALVEC2(File,(Var)[1]))

#define GET_REALMAT3(File,Var)\
        (GET_REALVEC3(File,(Var)[0]),\
         GET_REALVEC3(File,(Var)[1]),\
         GET_REALVEC3(File,(Var)[2]))

#define GET_REALMAT4(File,Var)\
        (GET_REALVEC4(File,(Var)[0]),\
         GET_REALVEC4(File,(Var)[1]),\
         GET_REALVEC4(File,(Var)[2]),\
         GET_REALVEC4(File,(Var)[3]))

/*
** Put values in file
*/

#define PUT_REAL(File,Var)\
        (fprintf (File, REAL1FILE, (Var)))

#define PUT_REALVEC2(File,Var)\
        (fprintf (File, REAL2FILE, (Var).x, (Var).y))

#define PUT_REALVEC3(File,Var)\
        (fprintf (File, REAL3FILE, (Var).x, (Var).y, (Var).z))

#define PUT_REALVEC4(File,Var)\
        (fprintf (File, REAL4FILE, (Var).x, (Var).y, (Var).z, (Var).w))

#define PUT_REALMAT2(File,Var)\
        (PUT_REALVEC2 (File,(Var[)0]),\
         PUT_REALVEC2 (File,(Var)[1]))

#define PUT_REALMAT3(File,Var)\
        (PUT_REALVEC3(File,(Var)[0]),\
         PUT_REALVEC3(File,(Var)[1]),\
         PUT_REALVEC3(File,(Var)[2]))

#define PUT_REALMAT4(File,Var)\
        (PUT_REALVEC4(File,(Var)[0]),\
         PUT_REALVEC4(File,(Var)[1]),\
         PUT_REALVEC4(File,(Var)[2]),\
         PUT_REALVEC4(File,(Var)[3]))

#endif

/* ------------------------------------------------------------------------- */
