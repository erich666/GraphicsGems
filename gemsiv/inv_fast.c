/*
 * ANSI C code from the article
 * "Fast Inversion of Length- and Angle-Preserving Matrices"
 * by Kevin Wu, Kevin.Wu@eng.sun.com
 * in "Graphics Gems IV", Academic Press, 1994
 *
 * compile with "cc -DMAIN ..." to create a test program
 */

#include "GraphicsGems.h"
#include <stdio.h>
/****
 *
 * angle_preserving_matrix4_inverse
 *
 * Computes the inverse of a 3-D angle-preserving matrix.
 *
 * This procedure treats the 4 by 4 angle-preserving matrix as a block
 * matrix and calculates the inverse of one submatrix for a significant
 * performance improvement over a general procedure that can invert any
 * nonsingular matrix:
 *	    --	      --	  --		       --
 *	    |	       | -1	  |   -2 T	-2  T	|
 *	    | A	     C |	  |  s	A     - s  A  C |
 *    -1    |	       |	  |			|
 *   M	 =  |	       |     =	  |			|
 *	    | 0	     1 |	  |    0	   1	|
 *	    |	       |	  |			|
 *	    --	      --	  --		       --
 * where      M is a 4 by 4 angle-preserving matrix,
 *	      A is the 3 by 3 upper-left submatrix of M,
 *	      C is the 3 by 1 upper-right submatrix of M.
 *
 * Input:
 *   in	  - 3-D angle-preserving matrix
 *
 * Output:
 *   out  - inverse of 3-D angle-preserving matrix
 *
 * Returned value:
 *   TRUE   if input matrix is nonsingular
 *   FALSE  otherwise
 *
 ***/

boolean
angle_preserving_matrix4_inverse (Matrix4 *in, Matrix4 *out)
{
    double  scale;

    /* Calculate the square of the isotropic scale factor */
    scale = in->element[0][0] * in->element[0][0] +
	    in->element[0][1] * in->element[0][1] +
	    in->element[0][2] * in->element[0][2];

    /* Is the submatrix A singular? */
    if (scale == 0.0) {

	/* Matrix M has no inverse */
	fprintf (stderr, "angle_preserving_matrix4_inverse: singular matrix\n");
	return FALSE;
    }

    /* Calculate the inverse of the square of the isotropic scale factor */
    scale = 1.0 / scale;

    /* Transpose and scale the 3 by 3 upper-left submatrix */
    out->element[0][0] = scale * in->element[0][0];
    out->element[1][0] = scale * in->element[0][1];
    out->element[2][0] = scale * in->element[0][2];
    out->element[0][1] = scale * in->element[1][0];
    out->element[1][1] = scale * in->element[1][1];
    out->element[2][1] = scale * in->element[1][2];
    out->element[0][2] = scale * in->element[2][0];
    out->element[1][2] = scale * in->element[2][1];
    out->element[2][2] = scale * in->element[2][2];

    /* Calculate -(transpose(A) / s*s) C */
    out->element[0][3] = - ( out->element[0][0] * in->element[0][3] +
			     out->element[0][1] * in->element[1][3] +
			     out->element[0][2] * in->element[2][3] );
    out->element[1][3] = - ( out->element[1][0] * in->element[0][3] +
			     out->element[1][1] * in->element[1][3] +
			     out->element[1][2] * in->element[2][3] );
    out->element[2][3] = - ( out->element[2][0] * in->element[0][3] +
			     out->element[2][1] * in->element[1][3] +
			     out->element[2][2] * in->element[2][3] );

    /* Fill in last row */
    out->element[3][0] = out->element[3][1] = out->element[3][2] = 0.0;
    out->element[3][3] = 1.0;

    return TRUE;
}

#ifdef MAIN	/* test program for inverter */

/*
 * Angle preserving matrix:
 * M = S(-3.67, 1.85, 9.52) T(6.93, 6.93, 6.93) Ry(0.19) Rz(-1.32) Rx(0.87)
 * where the angles are in radians.
 */
static double	m[4][4] =
	{{  1.6889057579031668e+00,  5.2512935661266260e+00,
	   -4.1948078887213214e+00, -3.6699999999999999e+00 },
	 { -6.7131956438195779e+00,  1.1090087288191814e+00,
	   -1.3145356165599698e+00,  1.8500000000000001e+00 },
	 { -3.2481008100637232e-01,  4.3839383574315880e+00,
	    5.3572831630889803e+00,  9.5199999999999996e+00 },
	 {  0.0000000000000000e+00,  0.0000000000000000e+00,
	    0.0000000000000000e+00,  1.0000000000000000e+00 }};

main()
{
    Matrix4	in, out, prod;
    int		i, j, k;

    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    in.element[i][j] = m[i][j];
	}
    }

    printf ("Original matrix:\n");
    printf ("%13.6e %13.6e %13.6e %13.6e\n", in.element[0][0],
	    in.element[0][1], in.element[0][2], in.element[0][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", in.element[1][0],
	    in.element[1][1], in.element[1][2], in.element[1][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", in.element[2][0],
	    in.element[2][1], in.element[2][2], in.element[2][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", in.element[3][0],
	    in.element[3][1], in.element[3][2], in.element[3][3]);

    /* Calculate inverse with utility */
    angle_preserving_matrix4_inverse(&in, &out);

    printf ("\nCalculated inverse matrix:\n");
    printf ("%13.6e %13.6e %13.6e %13.6e\n", out.element[0][0],
	    out.element[0][1], out.element[0][2], out.element[0][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", out.element[1][0],
	    out.element[1][1], out.element[1][2], out.element[1][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", out.element[2][0],
	    out.element[2][1], out.element[2][2], out.element[2][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", out.element[3][0],
	    out.element[3][1], out.element[3][2], out.element[3][3]);

    /*
     * Calculate the product of the original matrix and calculated inverse.
     * The product should be the identity if the utility is correct.
     */

    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    prod.element[i][j] = 0.0;
	}
    }

    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    for (k = 0; k < 4; k++) {
		prod.element[i][j] += in.element[i][k] * out.element[k][j];
	    }
	}
    }

    printf ("\nProduct of original matrix and calculated inverse:\n");
    printf ("%13.6e %13.6e %13.6e %13.6e\n", prod.element[0][0],
	    prod.element[0][1], prod.element[0][2], prod.element[0][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", prod.element[1][0],
	    prod.element[1][1], prod.element[1][2], prod.element[1][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", prod.element[2][0],
	    prod.element[2][1], prod.element[2][2], prod.element[2][3]);
    printf ("%13.6e %13.6e %13.6e %13.6e\n", prod.element[3][0],
	    prod.element[3][1], prod.element[3][2], prod.element[3][3]);

}

#endif
