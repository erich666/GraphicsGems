/* ------------------------------------------------------------------------- *\
   VEC3.H :

   Definition and manipulation of a 3D vector (either integers or reals)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _VEC3_
#define _VEC3_

#include "tool.h"

/*
** These macros are equivalent (for 3D vectors) to the ones given in tool.h
*/

#define MAKE_VEC3(V,X,Y,Z)\
        ((V).x = X,\
         (V).y = Y,\
         (V).z = Z)

#define COPY_VEC3(V,A)\
        ((V).x = (A).x,\
         (V).y = (A).y,\
         (V).z = (A).z)

#define SWAP_VEC3(A,B,t)\
        (SWAP ((A).x, (B).x, t),\
         SWAP ((A).y, (B).y, t),\
         SWAP ((A).z, (B).z, t))

#define ABS_VEC3(V,A)\
        ((V).x = ABS ((A).x),\
         (V).y = ABS ((A).y),\
         (V).z = ABS ((A).z))

#define SGN_VEC3(V,A)\
        ((V).x = SGN ((A).x),\
         (V).y = SGN ((A).y),\
         (V).z = SGN ((A).z))

#define NEG_VEC3(V,A)\
        ((V).x = -(A).x,\
         (V).y = -(A).y,\
         (V).z = -(A).z)

#define ROUND_VEC3(V,A)\
        ((V).x = ROUND ((A).x),\
         (V).y = ROUND ((A).y),\
         (V).z = ROUND ((A).z))

#define ZERO_VEC3(A)\
        (ZERO ((A).x) &&\
         ZERO ((A).y) &&\
         ZERO ((A).z))

#define ZERO_TOL_VEC3(A,t)\
        (ZERO_TOL ((A).x,t) &&\
         ZERO_TOL ((A).y,t) &&\
         ZERO_TOL ((A).z,t))

#define SAME_VEC3(A,B)\
        (SAME ((A).x,(B).x) &&\
         SAME ((A).y,(B).y) &&\
         SAME ((A).z,(B).z))

#define SAME_TOL_VEC3(A,B,t)\
        (SAME_TOL ((A).x,(B).x,t) &&\
         SAME_TOL ((A).y,(B).y,t) &&\
         SAME_TOL ((A).z,(B).z,t))

#define IN_VEC3(V,A,B)\
        (IN ((V).x,(A).x,(B).x) &&\
         IN ((V).y,(A).y,(B).y) &&\
         IN ((V).z,(A).z,(B).z))

#define OUT_VEC3(V,A,B)\
        (OUT ((V).x,(A).x,(B).x) ||\
         OUT ((V).y,(A).y,(B).y) ||\
         OUT ((V).z,(A).z,(B).z))

#define CLAMP_VEC3(V,A,B)\
        ((V).x = CLAMP ((A).x,(B).x),\
         (V).y = CLAMP ((A).y,(B).y),\
         (V).z = CLAMP ((A).z,(B).z))

#define MIN_VEC3(V,A,B)\
        ((V).x = MIN ((A).x,(B).x),\
         (V).y = MIN ((A).y,(B).y),\
         (V).z = MIN ((A).z,(B).z))

#define MAX_VEC3(V,A,B)\
        ((V).x = MAX ((A).x,(B).x),\
         (V).y = MAX ((A).y,(B).y),\
         (V).z = MAX ((A).z,(B).z))

#define MINMIN_VEC3(V,A,B,C)\
        ((V).x = MINMIN ((A).x,(B).x,(C).x),\
         (V).y = MINMIN ((A).y,(B).y,(C).y),\
         (V).z = MINMIN ((A).z,(B).z,(C).z))

#define MAXMAX_VEC3(V,A,B,C)\
        ((V).x = MAXMAX ((A).x,(B).x,(C).x),\
         (V).y = MAXMAX ((A).y,(B).y,(C).y),\
         (V).z = MAXMAX ((A).z,(B).z,(C).z))

/*
** Addition, subtraction, multiplication, division (by a vector element)
*/

#define INC_VEC3(V,A)\
        ((V).x += (A).x,\
         (V).y += (A).y,\
         (V).z += (A).z)

#define DEC_VEC3(V,A)\
        ((V).x -= (A).x,\
         (V).y -= (A).y,\
         (V).z -= (A).z)

#define ADD_VEC3(V,A,B)\
        ((V).x = (A).x + (B).x,\
         (V).y = (A).y + (B).y,\
         (V).z = (A).z + (B).z)

#define SUB_VEC3(V,A,B)\
        ((V).x = (A).x - (B).x,\
         (V).y = (A).y - (B).y,\
         (V).z = (A).z - (B).z)

#define MUL_VEC3(V,A,B)\
        ((V).x = (A).x * (B).x,\
         (V).y = (A).y * (B).y,\
         (V).z = (A).z * (B).z)

#define DIV_VEC3(V,A,B)\
        ((V).x = (A).x / (B).x,\
         (V).y = (A).y / (B).y,\
         (V).z = (A).z / (B).z)

/*
** Addition, subtraction, multiplication, division (by a scalar element)
*/

#define INCS_VEC3(V,s)\
        ((V).x += (s),\
         (V).y += (s),\
         (V).z += (s))

#define DECS_VEC3(V,s)\
        ((V).x -= (s),\
         (V).y -= (s),\
         (V).z -= (s))

#define ADDS_VEC3(V,A,s)\
        ((V).x = (A).x + (s),\
         (V).y = (A).y + (s),\
         (V).z = (A).z + (s))

#define SUBS_VEC3(V,A,s)\
        ((V).x = (A).x - (s),\
         (V).y = (A).y - (s),\
         (V).z = (A).z - (s))

#define MULS_VEC3(V,A,s)\
        ((V).x = (A).x * (s),\
         (V).y = (A).y * (s),\
         (V).z = (A).z * (s))

#define DIVS_VEC3(V,A,s)\
        ((V).x = (A).x / (s),\
         (V).y = (A).y / (s),\
         (V).z = (A).z / (s))

/*
** Linear combinations, linear/hermite/cardinal interpolations (see "tool.h")
*/

#define COMB2_VEC3(V,a,A,b,B)\
        ((V).x = (a) * (A).x + (b) * (B).x,\
         (V).y = (a) * (A).y + (b) * (B).y,\
         (V).z = (a) * (A).z + (b) * (B).z)

#define COMB3_VEC3(V,a,A,b,B,c,C)\
        ((V).x = (a) * (A).x + (b) * (B).x + (c) * (C).x,\
         (V).y = (a) * (A).y + (b) * (B).y + (c) * (C).y,\
         (V).z = (a) * (A).z + (b) * (B).z + (c) * (C).z)

#define COMB4_VEC3(V,a,A,b,B,c,C,d,D)\
        ((V).x = (a) * (A).x + (b) * (B).x + (c) * (C).x + (d) * (D).x,\
         (V).y = (a) * (A).y + (b) * (B).y + (c) * (C).y + (d) * (D).y,\
         (V).z = (a) * (A).z + (b) * (B).z + (c) * (C).z + (d) * (D).z)

#define LERP_VEC3(V,t,A,B)\
        ((V).x = LERP ((t), (A).x, (B).x),\
         (V).y = LERP ((t), (A).y, (B).y),\
         (V).z = LERP ((t), (A).z, (B).z))

#define HERP_VEC3(V,t,A,B)\
        ((V).x = HERP ((t), (A).x, (B).x),\
         (V).y = HERP ((t), (A).y, (B).y),\
         (V).z = HERP ((t), (A).z, (B).z))

#define CERP_VEC3(V,t,AA,A,B,BB)\
        ((V).x = CERP ((t), (AA).x, (A).x, (B).x, (BB).x),\
         (V).y = CERP ((t), (AA).y, (A).y, (B).y, (BB).y),\
         (V).z = CERP ((t), (AA).z, (A).z, (B).z, (BB).z))

/*
** Perpendicular, determinant, dot product, length, normalization
*/

#define PERP_VEC3(V,A,B)\
        ((V).x = (A).y * (B).z - (A).z * (B).y,\
         (V).y = (A).z * (B).x - (A).x * (B).z,\
         (V).z = (A).x * (B).y - (A).y * (B).x)

#define CROSS_VEC3(V,A,B) PERP_VEC3 (V,A,B)   /* Perp is also cross product */
                             
#define DELTA_VEC3(A,B,C)\
        ((A).x * ((B).y * (C).z - (B).z * (C).y)\
        +(A).y * ((B).z * (C).x - (B).x * (C).z)\
        +(A).z * ((B).x * (C).y - (B).y * (C).x))

#define DOT_VEC3(A,B)\
        ((A).x * (B).x + (A).y * (B).y + (A).z * (B).z)

#define LEN_VEC3(A)\
        (sqrt ((double) DOT_VEC3 (A, A)))

#define UNIT_VEC3(V,A,s)\
        ((s) = LEN_VEC3 (A), (s) ? (DIVS_VEC3 (V,A,s), TRUE) : FALSE)

#endif

/* ------------------------------------------------------------------------- */
