/* ------------------------------------------------------------------------- *\
   MAT3.H :

   Definition and manipulation of a 3D matrix (either integers or reals)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _MAT3_
#define _MAT3_

#include "tool.h"
#include "vec3.h"

/*
** Creation, duplication, swapping
*/

#define MAKE_MAT3(M,A,B,C)\
        (COPY_VEC3 ((M)[0],(A)),\
         COPY_VEC3 ((M)[1],(B)),\
         COPY_VEC3 ((M)[2],(C)))

#define COPY_MAT3(M,A)\
        (COPY_VEC3 ((M)[0],(A)[0]),\
         COPY_VEC3 ((M)[1],(A)[1]),\
         COPY_VEC3 ((M)[2],(A)[2]))

#define SWAP_MAT3(A,B,t)\
        (SWAP_VEC3 ((A)[0], (B)[0], t),\
         SWAP_VEC3 ((A)[1], (B)[1], t),\
         SWAP_VEC3 ((A)[2], (B)[2], t))

/*
** Addition, subtraction, multiplication, division (by a matrix element)
*/

#define INC_MAT3(M,A)\
        (INC_VEC3 ((M)[0],(A)[0]),\
         INC_VEC3 ((M)[1],(A)[1]),\
         INC_VEC3 ((M)[2],(A)[2]))

#define DEC_MAT3(M,A)\
        (DEC_VEC3 ((M)[0],(A)[0]),\
         DEC_VEC3 ((M)[1],(A)[1]),\
         DEC_VEC3 ((M)[2],(A)[2]))

#define ADD_MAT3(M,A,B)\
        (ADD_VEC3 ((M)[0],(A)[0],(B)[0]),\
         ADD_VEC3 ((M)[1],(A)[1],(B)[1]),\
         ADD_VEC3 ((M)[2],(A)[2],(B)[2]))

#define SUB_MAT3(M,A,B)\
        (SUB_VEC3 ((M)[0],(A)[0],(B)[0]),\
         SUB_VEC3 ((M)[1],(A)[1],(B)[1]),\
         SUB_VEC3 ((M)[2],(A)[2],(B)[2]))

#define MUL_MAT3(M,A,B)\
        (MUL_VEC3 ((M)[0],(A)[0],(B)[0]),\
         MUL_VEC3 ((M)[1],(A)[1],(B)[1]),\
         MUL_VEC3 ((M)[2],(A)[2],(B)[2]))

#define DIV_MAT3(M,A,B)\
        (DIV_VEC3 ((M)[0],(A)[0],(B)[0]),\
         DIV_VEC3 ((M)[1],(A)[1],(B)[1]),\
         DIV_VEC3 ((M)[2],(A)[2],(B)[2]))

/*
** Addition, subtraction, multiplication, division (by a scalar element)
*/

#define ADDS_MAT3(M,A,s)\
        (ADDS_VEC3 ((M)[0],(A)[0],s),\
         ADDS_VEC3 ((M)[1],(A)[1],s),\
         ADDS_VEC3 ((M)[2],(A)[2],s))

#define SUBS_MAT3(M,A,B)\
        (SUBS_VEC3 ((M)[0],(A)[0],s),\
         SUBS_VEC3 ((M)[1],(A)[1],s),\
         SUBS_VEC3 ((M)[2],(A)[2],s))

#define MULS_MAT3(M,A,B)\
        (MULS_VEC3 ((M)[0],(A)[0],s),\
         MULS_VEC3 ((M)[1],(A)[1],s),\
         MULS_VEC3 ((M)[2],(A)[2],s))

#define DIVS_MAT3(M,A,B)\
        (DIVS_VEC3 ((M)[0],(A)[0],s),\
         DIVS_VEC3 ((M)[1],(A)[1],s),\
         DIVS_VEC3 ((M)[2],(A)[2],s))

/*
** Determinant, transposition, adjunction, inversion
*/ 

#define DELTA_MAT3(M)\
        (DELTA_VEC3 ((M)[0],(M)[1],(M)[2]))

#define TRANS_MAT3(M,A)\
        ((M)[0].x = (A)[0].x, (M)[0].y = (A)[1].x, (M)[0].z = (A)[2].x,\
         (M)[1].x = (A)[0].y, (M)[1].y = (A)[1].y, (M)[1].z = (A)[2].y,\
         (M)[2].x = (A)[0].z, (M)[2].y = (A)[1].z, (M)[2].z = (A)[2].z)

#define ADJ_MAT3(M,A)\
        ((M)[0].x = (A)[1].y * (A)[2].z - (A)[1].z * (A)[2].y,\
         (M)[1].x = (A)[2].y * (A)[0].z - (A)[2].z * (A)[0].y,\
         (M)[2].x = (A)[0].y * (A)[1].z - (A)[0].z * (A)[1].y,\
         (M)[0].y = (A)[1].z * (A)[2].x - (A)[1].x * (A)[2].z,\
         (M)[1].y = (A)[2].z * (A)[0].x - (A)[2].x * (A)[0].z,\
         (M)[2].y = (A)[0].z * (A)[1].x - (A)[0].x * (A)[1].z,\
         (M)[0].z = (A)[1].x * (A)[2].y - (A)[1].y * (A)[2].x,\
         (M)[1].z = (A)[2].x * (A)[0].y - (A)[2].y * (A)[0].x,\
         (M)[2].z = (A)[0].x * (A)[1].y - (A)[0].y * (A)[1].x)

#define INV_MAT3(M,A,s)\
        (ADJ_MAT3 (M,A), (s) = DOT_VEC3((M)[0],(A)[0]),\
         ZERO (s) ? (DIVS_MAT3 (M,M,s), TRUE) : FALSE)

/*
** Matrix product, left vector product, right vector product
*/

#define ROW_VEC3(V,M,n)\
        ((V).x*(M)[0].n + (V).y*(M)[1].n + (V).z*(M)[2].n)

#define PROD_MAT3(M,A,B)\
        ((M)[0].x = ROW_VEC3 ((A)[0],(B),x),\
         (M)[0].y = ROW_VEC3 ((A)[0],(B),y),\
         (M)[0].z = ROW_VEC3 ((A)[0],(B),z),\
         (M)[1].x = ROW_VEC3 ((A)[1],(B),x),\
         (M)[1].y = ROW_VEC3 ((A)[1],(B),y),\
         (M)[1].z = ROW_VEC3 ((A)[1],(B),z),\
         (M)[2].x = ROW_VEC3 ((A)[2],(B),x),\
         (M)[2].y = ROW_VEC3 ((A)[2],(B),y),\
         (M)[2].z = ROW_VEC3 ((A)[2],(B),z))

#define LMAT_VEC3(V,A,M)\
        ((V).x = ROW_VEC3 ((A),(M),x),\
         (V).y = ROW_VEC3 ((A),(M),y),\
         (V).z = ROW_VEC3 ((A),(M),z))

#define RMAT_VEC3(V,M,A)\
        ((V).x = DOT_VEC3 ((A),(M)[0]),\
         (V).y = DOT_VEC3 ((A),(M)[1]),\
         (V).z = DOT_VEC3 ((A),(M)[2]))

/*
** MAKE_FRAME3(F,O,A,B,C,s) = Create a local frame F where O is the origin,
**                          (A,B,C) are the axis and 's' a dummy real variable
** LOCAL_FRAME3(V,F,A) = Transform A from world frame into local frame F
** WORLD_FRAME3(V,F,A) = Transform A from local frame F into world frame
*/

#define MAKE_FRAME3(F,O,A,B,C,s)\
        (COPY_VEC3 ((F)[0],(A)),\
         COPY_VEC3 ((F)[1],(B)),\
         COPY_VEC3 ((F)[2],(C)),\
         COPY_VEC3 ((F)[3],(O)),\
	 INV_MAT3 ((F)+12,(F),s))

#define LOCAL_FRAME3(V,F,A)\
        (DEC_VEC3 ((A),(F)[3]),\
         LMAT_VEC3 ((V),(A),(F)+12),\
         INC_VEC3 ((A),(F)[3]))

#define WORLD_FRAME3(V,F,A)\
        (LMAT_VEC3 ((V),(A),(F)),\
         INC_VEC3 ((V),(F)[3]))

#endif

/* ------------------------------------------------------------------------- */
