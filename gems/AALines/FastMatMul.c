/*  FILENAME: FastMatMul.c  [revised 18 AUG 90]

    AUTHOR:  Kelvin Thompson

    DESCRIPTION:  Routines to multiply different kinds of 4x4
      matrices as fast as possible.  Based on ideas on pages 454,
      460-461, and 646 of _Graphics_Gems_.

	These routines offer two advantages over the standard
      V3MatMul in the _Graphics_Gems_ vector library GGVecLib.c.
      First, the routines are faster.  Second, they can handle
      input matrices that are the same as the output matrix.
      The routines have the disadvantage of taking more code
      space (from unwound loops).

        The routines are about as fast as you can get for
      general-purpose multiplication.  If you have special
      knowledge about your system, you may be able to improve
      them a little more:

        [1]  If you know that your input and output matrices will
      never overlap: remove the tests near the beginning and end of
      each routine, and just #define 'mptr' to 'result'.  (The
      standard library's V3MatMul makes this assumption.)

	[2]  If you know that your compiler supports more than
      three pointer-to-double registers in a subroutine: make
      'result' in each routine a register variable.  You might
      also make the 'usetemp' boolean a register.

	[3]  If you have limited stack space, or your system can
      access global memory faster than stack:  make each routine's
      'tempx' a static, or let all routines share a global 'tempx'.
      (This is useless if assumption [1] holds.)
*/

/* definitions from "GraphicsGems.h" */
typedef struct Matrix3Struct {	/* 3-by-3 matrix */
	double element[3][3];
	} Matrix3;
typedef struct Matrix4Struct {	/* 4-by-4 matrix */
	double element[4][4];
	} Matrix4;

/* routines in this file */
Matrix3 *V2MatMul();     /* general 3x3 matrix multiplier */
Matrix4 *V3MatMul();     /* general 4x4 matrix multiplier */
Matrix4 *V3AffMatMul();  /* affine 4x4 matrix multiplier */
Matrix4 *V3LinMatMul();  /* linear 4x4 matrix multiplier */

/* macro to access matrix element */
#define MVAL(mptr,row,col)  ((mptr)->element[row][col])





/*  ************************************
    *                                  *
    *           V2MatMul               *
    *                                  *
    ************************************

    DESCRIPTION:  Multiply two general 3x3 matrices.  If one of
      the input matrices is the same as the output, write the
      result to a temporary matrix during multiplication, then
      copy to the output matrix.

    ENTRY:
      a -- pointer to left matrix
      b -- pointer to right matrix
      result -- result matrix

    EXIT:  returns 'result'
*/

Matrix3 *V2MatMul ( a, b, result )
register Matrix3 *a,*b;
Matrix3 *result;
{
register Matrix3 *mptr;
int usetemp;  /* boolean */
Matrix3 tempx;

/* decide where intermediate result goes */
usetemp = ( a == result  ||  b == result );
if ( usetemp )
  mptr = & tempx;
else
  mptr = result;

MVAL(mptr,0,0) =
     MVAL(a,0,0) * MVAL(b,0,0)
  +  MVAL(a,0,1) * MVAL(b,1,0)
  +  MVAL(a,0,2) * MVAL(b,2,0);

MVAL(mptr,0,1) =
     MVAL(a,0,0) * MVAL(b,0,1)
  +  MVAL(a,0,1) * MVAL(b,1,1)
  +  MVAL(a,0,2) * MVAL(b,2,1);

MVAL(mptr,0,2) =
     MVAL(a,0,0) * MVAL(b,0,2)
  +  MVAL(a,0,1) * MVAL(b,1,2)
  +  MVAL(a,0,2) * MVAL(b,2,2);

MVAL(mptr,1,0) =
     MVAL(a,1,0) * MVAL(b,0,0)
  +  MVAL(a,1,1) * MVAL(b,1,0)
  +  MVAL(a,1,2) * MVAL(b,2,0);

MVAL(mptr,1,1) =
     MVAL(a,1,0) * MVAL(b,0,1)
  +  MVAL(a,1,1) * MVAL(b,1,1)
  +  MVAL(a,1,2) * MVAL(b,2,1);

MVAL(mptr,1,2) =
     MVAL(a,1,0) * MVAL(b,0,2)
  +  MVAL(a,1,1) * MVAL(b,1,2)
  +  MVAL(a,1,2) * MVAL(b,2,2);

MVAL(mptr,2,0) =
     MVAL(a,2,0) * MVAL(b,0,0)
  +  MVAL(a,2,1) * MVAL(b,1,0)
  +  MVAL(a,2,2) * MVAL(b,2,0);

MVAL(mptr,2,1) =
     MVAL(a,2,0) * MVAL(b,0,1)
  +  MVAL(a,2,1) * MVAL(b,1,1)
  +  MVAL(a,2,2) * MVAL(b,2,1);

MVAL(mptr,2,2) =
     MVAL(a,2,0) * MVAL(b,0,2)
  +  MVAL(a,2,1) * MVAL(b,1,2)
  +  MVAL(a,2,2) * MVAL(b,2,2);

/* copy temp matrix to result if needed */
if ( usetemp )
  *result = *mptr;

return result;
}




/*  ************************************
    *                                  *
    *           V3MatMul               *
    *                                  *
    ************************************

    DESCRIPTION:  Multiply two general 4x4 matrices.  If one of
      the input matrices is the same as the output, write the
      result to a temporary matrix during multiplication, then
      copy to the output matrix.

    ENTRY:
      a -- pointer to left matrix
      b -- pointer to right matrix
      result -- result matrix

    EXIT:  returns 'result'
*/

Matrix4 *V3MatMul ( a, b, result )
register Matrix4 *a,*b;
Matrix4 *result;
{
register Matrix4 *mptr;
int usetemp;  /* boolean */
Matrix4 tempx;

/* decide where intermediate result goes */
usetemp = ( a == result  ||  b == result );
if ( usetemp )
  mptr = & tempx;
else
  mptr = result;

MVAL(mptr,0,0) =
     MVAL(a,0,0) * MVAL(b,0,0)
  +  MVAL(a,0,1) * MVAL(b,1,0)
  +  MVAL(a,0,2) * MVAL(b,2,0)
  +  MVAL(a,0,3) * MVAL(b,3,0);

MVAL(mptr,0,1) =
     MVAL(a,0,0) * MVAL(b,0,1)
  +  MVAL(a,0,1) * MVAL(b,1,1)
  +  MVAL(a,0,2) * MVAL(b,2,1)
  +  MVAL(a,0,3) * MVAL(b,3,1);

MVAL(mptr,0,2) =
     MVAL(a,0,0) * MVAL(b,0,2)
  +  MVAL(a,0,1) * MVAL(b,1,2)
  +  MVAL(a,0,2) * MVAL(b,2,2)
  +  MVAL(a,0,3) * MVAL(b,3,2);

MVAL(mptr,0,3) =
     MVAL(a,0,0) * MVAL(b,0,3)
  +  MVAL(a,0,1) * MVAL(b,1,3)
  +  MVAL(a,0,2) * MVAL(b,2,3)
  +  MVAL(a,0,3) * MVAL(b,3,3);

MVAL(mptr,1,0) =
     MVAL(a,1,0) * MVAL(b,0,0)
  +  MVAL(a,1,1) * MVAL(b,1,0)
  +  MVAL(a,1,2) * MVAL(b,2,0)
  +  MVAL(a,1,3) * MVAL(b,3,0);

MVAL(mptr,1,1) =
     MVAL(a,1,0) * MVAL(b,0,1)
  +  MVAL(a,1,1) * MVAL(b,1,1)
  +  MVAL(a,1,2) * MVAL(b,2,1)
  +  MVAL(a,1,3) * MVAL(b,3,1);

MVAL(mptr,1,2) =
     MVAL(a,1,0) * MVAL(b,0,2)
  +  MVAL(a,1,1) * MVAL(b,1,2)
  +  MVAL(a,1,2) * MVAL(b,2,2)
  +  MVAL(a,1,3) * MVAL(b,3,2);

MVAL(mptr,1,3) =
     MVAL(a,1,0) * MVAL(b,0,3)
  +  MVAL(a,1,1) * MVAL(b,1,3)
  +  MVAL(a,1,2) * MVAL(b,2,3)
  +  MVAL(a,1,3) * MVAL(b,3,3);

MVAL(mptr,2,0) =
     MVAL(a,2,0) * MVAL(b,0,0)
  +  MVAL(a,2,1) * MVAL(b,1,0)
  +  MVAL(a,2,2) * MVAL(b,2,0)
  +  MVAL(a,2,3) * MVAL(b,3,0);

MVAL(mptr,2,1) =
     MVAL(a,2,0) * MVAL(b,0,1)
  +  MVAL(a,2,1) * MVAL(b,1,1)
  +  MVAL(a,2,2) * MVAL(b,2,1)
  +  MVAL(a,2,3) * MVAL(b,3,1);

MVAL(mptr,2,2) =
     MVAL(a,2,0) * MVAL(b,0,2)
  +  MVAL(a,2,1) * MVAL(b,1,2)
  +  MVAL(a,2,2) * MVAL(b,2,2)
  +  MVAL(a,2,3) * MVAL(b,3,2);

MVAL(mptr,2,3) =
     MVAL(a,2,0) * MVAL(b,0,3)
  +  MVAL(a,2,1) * MVAL(b,1,3)
  +  MVAL(a,2,2) * MVAL(b,2,3)
  +  MVAL(a,2,3) * MVAL(b,3,3);

MVAL(mptr,3,0) =
     MVAL(a,3,0) * MVAL(b,0,0)
  +  MVAL(a,3,1) * MVAL(b,1,0)
  +  MVAL(a,3,2) * MVAL(b,2,0)
  +  MVAL(a,3,3) * MVAL(b,3,0);

MVAL(mptr,3,1) =
     MVAL(a,3,0) * MVAL(b,0,1)
  +  MVAL(a,3,1) * MVAL(b,1,1)
  +  MVAL(a,3,2) * MVAL(b,2,1)
  +  MVAL(a,3,3) * MVAL(b,3,1);

MVAL(mptr,3,2) =
     MVAL(a,3,0) * MVAL(b,0,2)
  +  MVAL(a,3,1) * MVAL(b,1,2)
  +  MVAL(a,3,2) * MVAL(b,2,2)
  +  MVAL(a,3,3) * MVAL(b,3,2);

MVAL(mptr,3,3) =
     MVAL(a,3,0) * MVAL(b,0,3)
  +  MVAL(a,3,1) * MVAL(b,1,3)
  +  MVAL(a,3,2) * MVAL(b,2,3)
  +  MVAL(a,3,3) * MVAL(b,3,3);

/* copy temp matrix to result if needed */
if ( usetemp )
  *result = *mptr;

return result;
}





/*  ************************************
    *                                  *
    *        V3AffMatMul               *
    *                                  *
    ************************************

    DESCRIPTION:  Multiply two affine 4x4 matrices.  The
      routine assumes the rightmost column of each input
      matrix is [0 0 0 1].  The output matrix will have the
      same property.
    
      If one of the input matrices is the same as the output,
      write the result to a temporary matrix during multiplication,
      then copy to the output matrix.

    ENTRY:
      a -- pointer to left matrix
      b -- pointer to right matrix
      result -- result matrix

    EXIT:  returns 'result'
*/

Matrix4 *V3AffMatMul ( a, b, result )
register Matrix4 *a,*b;
Matrix4 *result;
{
register Matrix4 *mptr;
int usetemp;  /* boolean */
Matrix4 tempx;

/* decide where intermediate result goes */
usetemp = ( a == result  ||  b == result );
if ( usetemp )
  mptr = & tempx;
else
  mptr = result;

MVAL(mptr,0,0) =
     MVAL(a,0,0) * MVAL(b,0,0)
  +  MVAL(a,0,1) * MVAL(b,1,0)
  +  MVAL(a,0,2) * MVAL(b,2,0);

MVAL(mptr,0,1) =
     MVAL(a,0,0) * MVAL(b,0,1)
  +  MVAL(a,0,1) * MVAL(b,1,1)
  +  MVAL(a,0,2) * MVAL(b,2,1);

MVAL(mptr,0,2) =
     MVAL(a,0,0) * MVAL(b,0,2)
  +  MVAL(a,0,1) * MVAL(b,1,2)
  +  MVAL(a,0,2) * MVAL(b,2,2);

MVAL(mptr,0,3) = 0.0;

MVAL(mptr,1,0) =
     MVAL(a,1,0) * MVAL(b,0,0)
  +  MVAL(a,1,1) * MVAL(b,1,0)
  +  MVAL(a,1,2) * MVAL(b,2,0);

MVAL(mptr,1,1) =
     MVAL(a,1,0) * MVAL(b,0,1)
  +  MVAL(a,1,1) * MVAL(b,1,1)
  +  MVAL(a,1,2) * MVAL(b,2,1);

MVAL(mptr,1,2) =
     MVAL(a,1,0) * MVAL(b,0,2)
  +  MVAL(a,1,1) * MVAL(b,1,2)
  +  MVAL(a,1,2) * MVAL(b,2,2);

MVAL(mptr,1,3) = 0.0;

MVAL(mptr,2,0) =
     MVAL(a,2,0) * MVAL(b,0,0)
  +  MVAL(a,2,1) * MVAL(b,1,0)
  +  MVAL(a,2,2) * MVAL(b,2,0);

MVAL(mptr,2,1) =
     MVAL(a,2,0) * MVAL(b,0,1)
  +  MVAL(a,2,1) * MVAL(b,1,1)
  +  MVAL(a,2,2) * MVAL(b,2,1);

MVAL(mptr,2,2) =
     MVAL(a,2,0) * MVAL(b,0,2)
  +  MVAL(a,2,1) * MVAL(b,1,2)
  +  MVAL(a,2,2) * MVAL(b,2,2);

MVAL(mptr,2,3) = 0.0;

MVAL(mptr,3,0) =
     MVAL(a,3,0) * MVAL(b,0,0)
  +  MVAL(a,3,1) * MVAL(b,1,0)
  +  MVAL(a,3,2) * MVAL(b,2,0)
  +                MVAL(b,3,0);

MVAL(mptr,3,1) =
     MVAL(a,3,0) * MVAL(b,0,1)
  +  MVAL(a,3,1) * MVAL(b,1,1)
  +  MVAL(a,3,2) * MVAL(b,2,1)
  +                MVAL(b,3,1);

MVAL(mptr,3,2) =
     MVAL(a,3,0) * MVAL(b,0,2)
  +  MVAL(a,3,1) * MVAL(b,1,2)
  +  MVAL(a,3,2) * MVAL(b,2,2)
  +                MVAL(b,3,2);

MVAL(mptr,3,3) = 1.0;

/* copy temp matrix to result if needed */
if ( usetemp )
  *result = *mptr;

return result;
}





/*  ************************************
    *                                  *
    *        V3LinMatMul               *
    *                                  *
    ************************************

    DESCRIPTION:  Multiply two affine 4x4 matrices.  The
      routine assumes the right column and bottom line
      of each input matrix is [0 0 0 1].  The output matrix
      will have the same property.  This is pretty much the
      same thing as multiplying two 3x3 matrices.
    
      If one of the input matrices is the same as the output,
      write the result to a temporary matrix during multiplication,
      then copy to the output matrix.

    ENTRY:
      a -- pointer to left matrix
      b -- pointer to right matrix
      result -- result matrix

    EXIT:  returns 'result'
*/

Matrix4 *V3LinMatMul ( a, b, result )
register Matrix4 *a,*b;
Matrix4 *result;
{
register Matrix4 *mptr;
int usetemp;  /* boolean */
Matrix4 tempx;

/* decide where intermediate result goes */
usetemp = ( a == result  ||  b == result );
if ( usetemp )
  mptr = & tempx;
else
  mptr = result;

MVAL(mptr,0,0) =
     MVAL(a,0,0) * MVAL(b,0,0)
  +  MVAL(a,0,1) * MVAL(b,1,0)
  +  MVAL(a,0,2) * MVAL(b,2,0);

MVAL(mptr,0,1) =
     MVAL(a,0,0) * MVAL(b,0,1)
  +  MVAL(a,0,1) * MVAL(b,1,1)
  +  MVAL(a,0,2) * MVAL(b,2,1);

MVAL(mptr,0,2) =
     MVAL(a,0,0) * MVAL(b,0,2)
  +  MVAL(a,0,1) * MVAL(b,1,2)
  +  MVAL(a,0,2) * MVAL(b,2,2);

MVAL(mptr,0,3) = 0.0;

MVAL(mptr,1,0) =
     MVAL(a,1,0) * MVAL(b,0,0)
  +  MVAL(a,1,1) * MVAL(b,1,0)
  +  MVAL(a,1,2) * MVAL(b,2,0);

MVAL(mptr,1,1) =
     MVAL(a,1,0) * MVAL(b,0,1)
  +  MVAL(a,1,1) * MVAL(b,1,1)
  +  MVAL(a,1,2) * MVAL(b,2,1);

MVAL(mptr,1,2) =
     MVAL(a,1,0) * MVAL(b,0,2)
  +  MVAL(a,1,1) * MVAL(b,1,2)
  +  MVAL(a,1,2) * MVAL(b,2,2);

MVAL(mptr,1,3) = 0.0;

MVAL(mptr,2,0) =
     MVAL(a,2,0) * MVAL(b,0,0)
  +  MVAL(a,2,1) * MVAL(b,1,0)
  +  MVAL(a,2,2) * MVAL(b,2,0);

MVAL(mptr,2,1) =
     MVAL(a,2,0) * MVAL(b,0,1)
  +  MVAL(a,2,1) * MVAL(b,1,1)
  +  MVAL(a,2,2) * MVAL(b,2,1);

MVAL(mptr,2,2) =
     MVAL(a,2,0) * MVAL(b,0,2)
  +  MVAL(a,2,1) * MVAL(b,1,2)
  +  MVAL(a,2,2) * MVAL(b,2,2);

MVAL(mptr,2,3) = 0.0;

MVAL(mptr,3,0) = 0.0;
MVAL(mptr,3,1) = 0.0;
MVAL(mptr,3,2) = 0.0;
MVAL(mptr,3,3) = 1.0;

/* copy temp matrix to result if needed */
if ( usetemp )
  *result = *mptr;

return result;
}
