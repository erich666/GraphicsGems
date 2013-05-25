/* ------------------------------------------------------------------------- *\
   VEC2.H :

   Definition and manipulation of a 2D vector (either integers or reals)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _VEC2_
#define _VEC2_

#include "tool.h"

/*
** These macros are equivalent (for 2D vectors) to the ones given in tool.h
*/

#define MAKE_VEC2(V,X,Y)\
        ((V).x = X,\
         (V).y = Y)

#define COPY_VEC2(V,A)\
        ((V).x = (A).x,\
         (V).y = (A).y)

#define SWAP_VEC2(A,B,t)\
        (SWAP ((A).x, (B).x, t),\
         SWAP ((A).y, (B).y, t))

#define ABS_VEC2(V,A)\
        ((V).x = ABS ((A).x),\
         (V).y = ABS ((A).y))

#define SGN_VEC2(V,A)\
        ((V).x = SGN ((A).x),\
         (V).y = SGN ((A).y))

#define NEG_VEC2(V,A)\
        ((V).x = -(A).x,\
         (V).y = -(A).y)

#define ROUND_VEC2(V,A)\
        ((V).x = ROUND ((A).x),\
         (V).y = ROUND ((A).y))

#define ZERO_VEC2(A)\
        (ZERO ((A).x) &&\
         ZERO ((A).y))

#define ZERO_TOL_VEC2(A,t)\
        (ZERO_TOL ((A).x,t) &&\
         ZERO_TOL ((A).y,t))

#define SAME_VEC2(A,B)\
        (SAME ((A).x,(B).x) &&\
         SAME ((A).y,(B).y))

#define SAME_TOL_VEC2(A,B,t)\
        (SAME_TOL ((A).x,(B).x,t) &&\
         SAME_TOL ((A).y,(B).y,t))

#define IN_VEC2(V,A,B)\
        (IN ((V).x,(A).x,(B).x) &&\
         IN ((V).y,(A).y,(B).y))

#define OUT_VEC2(V,A,B)\
        (OUT ((V).x,(A).x,(B).x) ||\
         OUT ((V).y,(A).y,(B).y))

#define CLAMP_VEC2(V,A,B)\
        ((V).x = CLAMP ((A).x,(B).x),\
         (V).y = CLAMP ((A).y,(B).y))

#define MIN_VEC2(V,A,B)\
        ((V).x = MIN ((A).x,(B).x),\
         (V).y = MIN ((A).y,(B).y))

#define MAX_VEC2(V,A,B)\
        ((V).x = MAX ((A).x,(B).x),\
         (V).y = MAX ((A).y,(B).y))

#define MINMIN_VEC2(V,A,B,C)\
        ((V).x = MINMIN ((A).x,(B).x,(C).x),\
         (V).y = MINMIN ((A).y,(B).y,(C).y))

#define MAXMAX_VEC2(V,A,B,C)\
        ((V).x = MAXMAX ((A).x,(B).x,(C).x),\
         (V).y = MAXMAX ((A).y,(B).y,(C).y))

/*
** Addition, subtraction, multiplication, division (by a vector element)
*/

#define INC_VEC2(V,A)\
        ((V).x += (A).x,\
         (V).y += (A).y)

#define DEC_VEC2(V,A)\
        ((V).x -= (A).x,\
         (V).y -= (A).y)

#define ADD_VEC2(V,A,B)\
        ((V).x = (A).x + (B).x,\
         (V).y = (A).y + (B).y)

#define SUB_VEC2(V,A,B)\
        ((V).x = (A).x - (B).x,\
         (V).y = (A).y - (B).y)

#define MUL_VEC2(V,A,B)\
        ((V).x = (A).x * (B).x,\
         (V).y = (A).y * (B).y)

#define DIV_VEC2(V,A,B)\
        ((V).x = (A).x / (B).x,\
         (V).y = (A).y / (B).y)

/*
** Addition, subtraction, multiplication, division (by a scalar element)
*/

#define INCS_VEC2(V,s)\
        ((V).x += (s),\
         (V).y += (s))

#define DECS_VEC2(V,s)\
        ((V).x -= (s),\
         (V).y -= (s))

#define ADDS_VEC2(V,A,s)\
        ((V).x = (A).x + (s),\
         (V).y = (A).y + (s))

#define SUBS_VEC2(V,A,s)\
        ((V).x = (A).x - (s),\
         (V).y = (A).y - (s))

#define MULS_VEC2(V,A,s)\
        ((V).x = (A).x * (s),\
         (V).y = (A).y * (s))

#define DIVS_VEC2(V,A,s)\
        ((V).x = (A).x / (s),\
         (V).y = (A).y / (s))

/*
** Linear combinations, linear/hermite/cardinal interpolations (see "tool.h")
*/

#define COMB2_VEC2(V,a,A,b,B)\
        ((V).x = (a) * (A).x + (b) * (B).x,\
         (V).y = (a) * (A).y + (b) * (B).y)

#define COMB3_VEC2(V,a,A,b,B,c,C)\
        ((V).x = (a) * (A).x + (b) * (B).x + (c) * (C).x,\
         (V).y = (a) * (A).y + (b) * (B).y + (c) * (C).y)

#define COMB4_VEC2(V,a,A,b,B,c,C,d,D)\
        ((V).x = (a) * (A).x + (b) * (B).x + (c) * (C).x + (d) * (D).x,\
         (V).y = (a) * (A).y + (b) * (B).y + (c) * (C).y + (d) * (D).y)

#define LERP_VEC2(V,t,A,B)\
        ((V).x = LERP ((t), (A).x, (B).x),\
         (V).y = LERP ((t), (A).y, (B).y))

#define HERP_VEC2(V,t,A,B)\
        ((V).x = HERP ((t), (A).x, (B).x),\
         (V).y = HERP ((t), (A).y, (B).y))

#define CERP_VEC2(V,t,AA,A,B,BB)\
        ((V).x = CERP ((t), (AA).x, (A).x, (B).x, (BB).x),\
         (V).y = CERP ((t), (AA).y, (A).y, (B).y, (BB).y))

/*
** Perpendicular, determinant, dot product, length, normalization
*/

#define PERP_VEC2(V,A)\
        ((V).x = -(A).y,\
         (V).y = (A).x)

#define DELTA_VEC2(A,B)\
        ((A).x * (B).y - (A).y * (B).x)

#define DOT_VEC2(A,B)\
        ((A).x * (B).x + (A).y * (B).y)

#define LEN_VEC2(A)\
        (sqrt ((double) DOT_VEC2 (A,A)))

#define UNIT_VEC2(V,A,s)\
        ((s) = LEN_VEC2(A), (s) ? (DIVS_VEC2 (V,A,s), TRUE) : FALSE)

#endif

/* ------------------------------------------------------------------------- */
