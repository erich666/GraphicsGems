/* ------------------------------------------------------------------------- *\
   MAT4.H :

   Definition and manipulation of a 4D matrix (either integers or reals)

   by Christophe Schlick (1 June 1992)
\* ------------------------------------------------------------------------- */

#ifndef _MAT4_
#define _MAT4_

#include "tool.h"
#include "vec4.h"

/*
** Creation, duplication, swapping
*/

#define MAKE_MAT4(M,A,B,C,D)\
        (COPY_VEC4 ((M)[0],(A)),\
         COPY_VEC4 ((M)[1],(B)),\
         COPY_VEC4 ((M)[2],(C)),\
         COPY_VEC4 ((M)[3],(D)))

#define COPY_MAT4(M,A)\
        (COPY_VEC4 ((M)[0],(A)[0]),\
         COPY_VEC4 ((M)[1],(A)[1]),\
         COPY_VEC4 ((M)[2],(A)[2]),\
         COPY_VEC4 ((M)[3],(A)[3]))

#define SWAP_MAT4(A,B,t)\
        (SWAP_VEC4 ((A)[0], (B)[0], t),\
         SWAP_VEC4 ((A)[1], (B)[1], t),\
         SWAP_VEC4 ((A)[2], (B)[2], t))
         SWAP_VEC4 ((A)[3], (B)[3], t))

/*
** Addition, subtraction, multiplication, division (by a matrix element)
*/

#define INC_MAT4(M,A)\
        (INC_VEC4 ((M)[0],(A)[0]),\
         INC_VEC4 ((M)[1],(A)[1]),\
         INC_VEC4 ((M)[2],(A)[2]),\
         INC_VEC4 ((M)[3],(A)[3]))

#define DEC_MAT4(M,A)\
        (DEC_VEC4 ((M)[0],(A)[0]),\
         DEC_VEC4 ((M)[1],(A)[1]),\
         DEC_VEC4 ((M)[2],(A)[2]),\
         DEC_VEC4 ((M)[3],(A)[3]))

#define ADD_MAT4(M,A,B)\
        (ADD_VEC4 ((M)[0],(A)[0],(B)[0]),\
         ADD_VEC4 ((M)[1],(A)[1],(B)[1]),\
         ADD_VEC4 ((M)[2],(A)[2],(B)[2]),\
         ADD_VEC4 ((M)[3],(A)[3],(B)[3]))

#define SUB_MAT4(V,A,B)\
        (SUB_VEC4 ((M)[0],(A)[0],(B)[0]),\
         SUB_VEC4 ((M)[1],(A)[1],(B)[1]),\
         SUB_VEC4 ((M)[2],(A)[2],(B)[2]),\
         SUB_VEC4 ((M)[3],(A)[3],(B)[3]))

#define MUL_MAT4(V,A,B)\
        (MUL_VEC4 ((M)[0],(A)[0],(B)[0]),\
         MUL_VEC4 ((M)[1],(A)[1],(B)[1]),\
         MUL_VEC4 ((M)[2],(A)[2],(B)[2]),\
         MUL_VEC4 ((M)[3],(A)[3],(B)[3]))

#define DIV_MAT4(V,A,B)\
        (DIV_VEC4 ((M)[0],(A)[0],(B)[0]),\
         DIV_VEC4 ((M)[1],(A)[1],(B)[1]),\
         DIV_VEC4 ((M)[2],(A)[2],(B)[2]),\
         DIV_VEC4 ((M)[3],(A)[3],(B)[3]))

/*
** Addition, subtraction, multiplication, division (by a scalar element)
*/

#define ADDS_MAT4(M,A,s)\
        (ADDS_VEC4 ((M)[0],(A)[0],s),\
         ADDS_VEC4 ((M)[1],(A)[1],s),\  
         ADDS_VEC4 ((M)[2],(A)[2],s),\
         ADDS_VEC4 ((M)[3],(A)[3],s))

#define SUBS_MAT4(V,A,B)\
        (SUBS_VEC4 ((M)[0],(A)[0],s),\
         SUBS_VEC4 ((M)[1],(A)[1],s),\
         SUBS_VEC4 ((M)[2],(A)[2],s),\
         SUBS_VEC4 ((M)[3],(A)[3],s))

#define MULS_MAT4(V,A,B)\
        (MULS_VEC4 ((M)[0],(A)[0],s),\
         MULS_VEC4 ((M)[1],(A)[1],s),\
         MULS_VEC4 ((M)[2],(A)[2],s),\
         MULS_VEC4 ((M)[3],(A)[3],s))

#define DIVS_MAT4(V,A,B)\
        (DIVS_VEC4 ((M)[0],(A)[0],s),\
         DIVS_VEC4 ((M)[1],(A)[1],s),\
         DIVS_VEC4 ((M)[2],(A)[2],s),\
         DIVS_VEC4 ((M)[3],(A)[3],s))

/*
** Determinant, transposition, ajunction, inversion --> YET TO DO
*/ 

/*
#define DELTA_MAT4(M)

#define TRANS_MAT4(M,A)

#define ADJ_MAT4(M,A)

#define INV_MAT4(M,A,s)
*/

/*
** Matrix product, left vector product, right vector product
*/

#define ROW_VEC4(M,n,V)\
        ((M)[0].n*(V).x + (M)[1].n*(V).y + (M)[2].n*(V).z + (M)[3].n*(V).w)

#define PROD_MAT4(M,A,B)\
        ((M)[0].x = ROW_VEC4 ((A),x,(B)[0]),\
         (M)[0].y = ROW_VEC4 ((A),y,(B)[0]),\
         (M)[0].z = ROW_VEC4 ((A),z,(B)[0]),\
         (M)[0].w = ROW_VEC4 ((A),w,(B)[0]),\
         (M)[1].x = ROW_VEC4 ((A),x,(B)[1]),\
         (M)[1].y = ROW_VEC4 ((A),y,(B)[1]),\
         (M)[1].z = ROW_VEC4 ((A),z,(B)[1]),\
         (M)[1].w = ROW_VEC4 ((A),w,(B)[1]),\
         (M)[2].x = ROW_VEC4 ((A),x,(B)[2]),\
         (M)[2].y = ROW_VEC4 ((A),y,(B)[2]),\
         (M)[2].z = ROW_VEC4 ((A),z,(B)[2]),\
         (M)[2].w = ROW_VEC4 ((A),w,(B)[2]),\
         (M)[3].x = ROW_VEC4 ((A),x,(B)[3]),\
         (M)[3].y = ROW_VEC4 ((A),y,(B)[3]),\
         (M)[3].z = ROW_VEC4 ((A),z,(B)[3]),\
         (M)[3].w = ROW_VEC4 ((A),w,(B)[3]))

#define LMAT_VEC4(V,A,M)\
        ((V).x = ROW_VEC4 ((M),x,(A)),\
         (V).y = ROW_VEC4 ((M),y,(A)),\
         (V).z = ROW_VEC4 ((M),z,(A)),\
         (V).w = ROW_VEC4 ((M),w,(A)))

#define RMAT_VEC4(V,M,A)\
        ((V).x = DOT_VEC4 ((A),(M)[0]),\
         (V).y = DOT_VEC4 ((A),(M)[1]),\
         (V).z = DOT_VEC4 ((A),(M)[2]),\
         (V).w = DOT_VEC4 ((A),(M)[3]))

#endif

/* ------------------------------------------------------------------------- */
