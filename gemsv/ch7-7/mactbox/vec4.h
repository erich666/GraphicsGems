/* ------------------------------------------------------------------------- *\
   VEC4.H :

   Definition and manipulation of a 4D vector (either integers or reals)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _VEC4_
#define _VEC4_

#include "tool.h"

/*
** These macros are equivalent (for 3D vectors) to the ones given in tool.h
*/

#define MAKE_VEC4(V,X,Y,Z,W)\
        ((V).x = X,\
         (V).y = Y,\
         (V).z = Z,\
         (V).w = W)

#define COPY_VEC4(V,A)\
        ((V).x = (A).x,\
         (V).y = (A).y,\
         (V).z = (A).z,\
         (V).w = (A).w)

#define SWAP_VEC4(A,B,t)\
        (SWAP ((A).x, (B).x, t),\
         SWAP ((A).y, (B).y, t),\
         SWAP ((A).z, (B).z, t),\
         SWAP ((A).w, (B).w, t))

#define ABS_VEC4(V,A)\
        ((V).x = ABS ((A).x),\
         (V).y = ABS ((A).y),\
         (V).z = ABS ((A).z),\
         (V).w = ABS ((A).w))

#define SGN_VEC4(V,A)\
        ((V).x = SGN ((A).x),\
         (V).y = SGN ((A).y),\
         (V).z = SGN ((A).z),\
         (V).w = SGN ((A).w))

#define NEG_VEC4(V)\
        ((V).x = -(V).x,\
         (V).y = -(V).y,\
         (V).z = -(V).z,\
         (V).w = -(V).w)

#define ROUND_VEC4(V,A)\
        ((V).x = ROUND ((A).x),\
         (V).y = ROUND ((A).y),\
         (V).z = ROUND ((A).z),\
         (V).w = ROUND ((A).w))

#define ZERO_VEC4(A)\
        (ZERO ((A).x) &&\
         ZERO ((A).y) &&\
         ZERO ((A).z) &&\
         ZERO ((A).w))

#define ZERO_TOL_VEC4(A,t)\
        (ZERO_TOL ((A).x,t) &&\
         ZERO_TOL ((A).y,t) &&\
         ZERO_TOL ((A).z,t) &&\
         ZERO_TOL ((A).w,t))

#define SAME_VEC4(A,B)\
        (SAME ((A).x,(B).x) &&\
         SAME ((A).y,(B).y) &&\
         SAME ((A).z,(B).z) &&\
         SAME ((A).w,(B).w))

#define SAME_TOL_VEC4(A,B,t)\
        (SAME_TOL ((A).x,(B).x,t) &&\
         SAME_TOL ((A).y,(B).y,t) &&\
         SAME_TOL ((A).z,(B).z,t) &&\
         SAME_TOL ((A).w,(B).w,t))

#define IN_VEC4(V,A,B)\
        (IN ((V).x,(A).x,(B).x) &&\
         IN ((V).y,(A).y,(B).y) &&\
         IN ((V).z,(A).z,(B).z) &&\
         IN ((V).w,(A).w,(B).w))

#define OUT_VEC4(V,A,B)\
        (OUT ((V).x,(A).x,(B).x) ||\
         OUT ((V).y,(A).y,(B).y) ||\
         OUT ((V).z,(A).z,(B).z) ||\
         OUT ((V).w,(A).w,(B).w))

#define CLAMP_VEC4(V,A,B)\
        ((V).x = CLAMP ((A).x,(B).x),\
         (V).y = CLAMP ((A).y,(B).y),\
         (V).z = CLAMP ((A).z,(B).z),\
         (V).w = CLAMP ((A).w,(B).w))

#define MIN_VEC4(V,A,B)\
        ((V).x = MIN ((A).x,(B).x),\
         (V).y = MIN ((A).y,(B).y),\
         (V).z = MIN ((A).z,(B).z),\ 
         (V).w = MIN ((A).w,(B).w))

#define MAX_VEC4(V,A,B)\
        ((V).x = MAX ((A).x,(B).x),\ 
         (V).y = MAX ((A).y,(B).y),\
         (V).z = MAX ((A).z,(B).z),\
         (V).w = MAX ((A).w,(B).w))

#define MINMIN_VEC4(V,A,B,C)\
        ((V).x = MINMIN ((A).x,(B).x,(C).x),\
         (V).y = MINMIN ((A).y,(B).y,(C).y),\
         (V).z = MINMIN ((A).z,(B).z,(C).z),\
         (V).w = MINMIN ((A).w,(B).w,(C).w))

#define MAXMAX_VEC4(V,A,B,C)\
        ((V).x = MAXMAX ((A).x,(B).x,(C).x),\
         (V).y = MAXMAX ((A).y,(B).y,(C).y),\
         (V).z = MAXMAX ((A).z,(B).z,(C).z),\
         (V).w = MAXMAX ((A).w,(B).w,(C).w))

/*
** Addition, subtraction, multiplication, division (by a vector element)
*/

#define INC_VEC4(V,A)\
        ((V).x += (A).x,\
         (V).y += (A).y,\
         (V).z += (A).z,\
         (V).w += (A).w)

#define DEC_VEC4(V,A)\
        ((V).x -= (A).x,\
         (V).y -= (A).y,\
         (V).z -= (A).z,\
         (V).w -= (A).w)

#define ADD_VEC4(V,A,B)\
        ((V).x = (A).x + (B).x,\
         (V).y = (A).y + (B).y,\
         (V).z = (A).z + (B).z,\
         (V).w = (A).w + (B).w)

#define SUB_VEC4(V,A,B)\
        ((V).x = (A).x - (B).x,\
         (V).y = (A).y - (B).y,\
         (V).z = (A).z - (B).z,\
         (V).w = (A).w - (B).w)

#define MUL_VEC4(V,A,B)\
        ((V).x = (A).x * (B).x,\
         (V).y = (A).y * (B).y,\
         (V).z = (A).z * (B).z,\
         (V).w = (A).w * (B).w)

#define DIV_VEC4(V,A,B)\
        ((V).x = (A).x / (B).x,\
         (V).y = (A).y / (B).y,\
         (V).z = (A).z / (B).z,\
         (V).w = (A).w / (B).w)

/*
** Addition, subtraction, multiplication, division (by a scalar element)
*/

#define INCS_VEC4(V,s)\
        ((V).x += (s),\
         (V).y += (s),\
         (V).z += (s),\
         (V).w += (s))

#define DECS_VEC4(V,s)\
        ((V).x -= (s),\
         (V).y -= (s),\
         (V).z -= (s),\
         (V).w -= (s))

#define ADDS_VEC4(V,A,s)\
        ((V).x = (A).x + (s),\
         (V).y = (A).y + (s),\
         (V).z = (A).z + (s),\
         (V).w = (A).w + (s))

#define SUBS_VEC4(V,A,s)\
        ((V).x = (A).x - (s),\
         (V).y = (A).y - (s),\
         (V).z = (A).z - (s),\
         (V).w = (A).w - (s))

#define MULS_VEC4(V,A,s)\
        ((V).x = (A).x * (s),\
         (V).y = (A).y * (s),\
         (V).z = (A).z * (s),\
         (V).w = (A).w * (s))

#define DIVS_VEC4(V,A,s)\
        ((V).x = (A).x / (s),\
          (V).y = (A).y / (s),\
         (V).z = (A).z / (s),\
          (V).w = (A).w / (s))

/*
** Linear combination
*/

#define COMB2_VEC4(V,a,A,b,B)\
        ((V).x = (a) * (A).x + (b) * (B).x,\
         (V).y = (a) * (A).y + (b) * (B).y,\
         (V).z = (a) * (A).z + (b) * (B).z,\
         (V).w = (a) * (A).w + (b) * (B).w)

#define COMB3_VEC4(V,a,A,b,B,c,C)\
        ((V).x = (a) * (A).x + (b) * (B).x + (c) * (C).x,\
         (V).y = (a) * (A).y + (b) * (B).y + (c) * (C).y,\
         (V).z = (a) * (A).z + (b) * (B).z + (c) * (C).z,\
         (V).w = (a) * (A).w + (b) * (B).w + (c) * (C).w)

#define COMB4_VEC4(V,a,A,b,B,c,C,d,D)\
        ((V).x = (a) * (A).x + (b) * (B).x + (c) * (C).x + (d) * (D).x,\
         (V).y = (a) * (A).y + (b) * (B).y + (c) * (C).y + (d) * (D).y,\
         (V).z = (a) * (A).z + (b) * (B).z + (c) * (C).z + (d) * (D).z,\
         (V).w = (a) * (A).w + (b) * (B).w + (c) * (C).w + (d) * (D).w)

#define LERP_VEC3(V,t,A,B)\
        ((V).x = LERP ((t), (A).x, (B).x),\
         (V).y = LERP ((t), (A).y, (B).y),\
         (V).z = LERP ((t), (A).z, (B).z),\
         (V).z = LERP ((t), (A).w, (B).w))

#define HERP_VEC3(V,t,A,B)\
        ((V).x = HERP ((t), (A).x, (B).x),\
         (V).y = HERP ((t), (A).y, (B).y),\
         (V).z = HERP ((t), (A).z, (B).z),\
         (V).w = HERP ((t), (A).w, (B).w))

#define CERP_VEC3(V,t,AA,A,B,BB)\
        ((V).x = CERP ((t), (AA).x, (A).x, (B).x, (BB).x),\
         (V).y = CERP ((t), (AA).y, (A).y, (B).y, (BB).y),\
         (V).z = CERP ((t), (AA).z, (A).z, (B).z, (BB).z),\
         (V).w = CERP ((t), (AA).w, (A).w, (B).w, (BB).w))

/*
** Dot product, length, normalization
*/

#define DOT_VEC4(A,B)\
        ((A).x * (B).x + (A).y * (B).y + (A).z * (B).z + (A).w * (B).w)

#define LEN_VEC4(A)\
        (sqrt ((double) DOT_VEC4 (A,A)))

#define UNIT_VEC4(V,A,s)\
        (s = LEN_REAL2 (A), (s) ? (DIVS_REAL2 (V,A,s), TRUE) : FALSE)

#endif

/* ------------------------------------------------------------------------- */
