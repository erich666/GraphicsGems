/* ------------------------------------------------------------------------- *\
   MAT2.H :

   Definition and manipulation of a 2D matrix (either integers or reals)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _MAT2_
#define _MAT2_

#include "tool.h"
#include "vec2.h"

/*
** Creation, duplication, swapping
*/

#define MAKE_MAT2(M,A,B)\
        (COPY_VEC2 ((M)[0],(A)),\
         COPY_VEC2 ((M)[1],(B)))

#define COPY_MAT2(M,A)\
        (COPY_VEC2 ((M)[0],(A)[0]),\
         COPY_VEC2 ((M)[1],(A)[1]))

#define SWAP_MAT2(A,B,t)\
        (SWAP_VEC2 ((A)[0], (B)[0], t),\
         SWAP_VEC2 ((A)[1], (B)[1], t))

/*
** Addition, subtraction, multiplication, division (by a matrix element)
*/

#define INC_MAT2(M,A)\
        (INC_VEC2 ((M)[0],(A)[0]),\
         INC_VEC2 ((M)[1],(A)[1]))

#define DEC_MAT2(M,A)\
        (DEC_VEC2 ((M)[0],(A)[0]),\
         DEC_VEC2 ((M)[1],(A)[1]))

#define ADD_MAT2(M,A,B)\
        (ADD_VEC2 ((M)[0],(A)[0],(B)[0]),\
         ADD_VEC2 ((M)[1],(A)[1],(B)[1]))

#define SUB_MAT2(M,A,B)\
        (SUB_VEC2 ((M)[0],(A)[0],(B)[0]),\
         SUB_VEC2 ((M)[1],(A)[1],(B)[1]))

#define MUL_MAT2(M,A,B)\
        (MUL_VEC2 ((M)[0],(A)[0],(B)[0]),\
         MUL_VEC2 ((M)[1],(A)[1],(B)[1]))

#define DIV_MAT2(M,A,B)\
        (DIV_VEC2 ((M)[0],(A)[0],(B)[0]),\
         DIV_VEC2 ((M)[1],(A)[1],(B)[1]))

/*
** Addition, subtraction, multiplication, division (by a scalar element)
*/

#define ADDS_MAT2(M,A,s)\
        (ADDS_VEC2 ((M)[0],(A)[0],s),\
         ADDS_VEC2 ((M)[1],(A)[1],s))  

#define SUBS_MAT2(M,A,B)\
        (SUBS_VEC2 ((M)[0],(A)[0],s),\
         SUBS_VEC2 ((M)[1],(A)[1],s))

#define MULS_MAT2(M,A,B)\
        (MULS_VEC2 ((M)[0],(A)[0],s),\
         MULS_VEC2 ((M)[1],(A)[1],s))

#define DIVS_MAT2(M,A,B)\
        (DIVS_VEC2 ((M)[0],(A)[0],s),\
         DIVS_VEC2 ((M)[1],(A)[1],s))

/*
** Determinant, transposition, adjunction, inversion
*/

#define DELTA_MAT2(M)\
        (DELTA_VEC2 ((M)[0],(M)[1]))

#define TRANS_MAT2(M,A)\
        ((M)[0].x = (A)[0].x, (M)[0].y = (A)[1].x,\
         (M)[1].x = (A)[0].y, (M)[1].y = (A)[1].y)  

#define ADJ_MAT2(M,A)\
        ((M)[0].x = (A)[1].y, (M)[0].y = -(A)[0].y,\
         (M)[1].x = -(A)[1].x, (M)[1].y = (A)[0].x)

#define INV_MAT2(M,A,s)\
        (ADJ_MAT2 (M,A), (s) = DOT_VEC2 ((M)[0],(A)[0]),\
         ZERO (s) ? (DIVS_MAT2 (M,M,s), TRUE) : FALSE)

/*
** Matrix product, left vector product, right vector product
*/

#define ROW_VEC2(V,M,n)\
        ((V).x*(M)[0].n + (V).y*(M)[1].n)

#define PROD_MAT2(M,A,B)\
        ((M)[0].x = ROW_VEC2 ((A)[0],(B),x),\
         (M)[0].y = ROW_VEC2 ((A)[0],(B),y),\
         (M)[1].x = ROW_VEC2 ((A)[1],(B),x),\
         (M)[1].y = ROW_VEC2 ((A)[1],(B),y))

#define LMAT_VEC2(V,A,M)\
        ((V).x = ROW_VEC2 ((A),(M),x),\
         (V).y = ROW_VEC2 ((A),(M),y))

#define RMAT_VEC2(V,M,A)\
        ((V).x = DOT_VEC2 ((A),(M)[0]),\
         (V).y = DOT_VEC2 ((A),(M)[1]))

/*
** MAKE_FRAME2(F,O,A,B,s) = Create a local frame F where O is the origin,
**                          (A,B) are the axis and 's' a dummy real variable
** LOCAL_FRAME2(V,F,A) = Transform A from world frame into local frame F
** WORLD_FRAME2(V,F,A) = Transform A from local frame F into world frame
*/

#define MAKE_FRAME2(F,O,A,B,s)\
        (COPY_VEC2 ((F)[0],(A)),\
         COPY_VEC2 ((F)[1],(B)),\
         COPY_VEC2 ((F)[2],(O)),\
	 INV_MAT2 ((F)+6,(F),s))

#define LOCAL_FRAME2(V,F,A)\
        (DEC_VEC2 ((A),(F)[2]),\
         LMAT_VEC2 ((V),(A),(F)+6),\
         INC_VEC2 ((A),(F)[2]))

#define WORLD_FRAME2(V,F,A)\
        (LMAT_VEC2 ((V),(A),(F)),\
         INC_VEC2 ((V),(F)[2]))

#endif

/* ------------------------------------------------------------------------- */
