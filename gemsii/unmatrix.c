/* unmatrix.c - given a 4x4 matrix, decompose it into standard operations.
 *
 * Author:	Spencer W. Thomas
 * 		University of Michigan
 */
#include <math.h>
#include "GraphicsGems.h"
#include "unmatrix.h"
#include "../gems/MatrixInvert.h"

/* unmatrix - Decompose a non-degenerate 4x4 transformation matrix into
 * 	the sequence of transformations that produced it.
 * [Sx][Sy][Sz][Shearx/y][Sx/z][Sz/y][Rx][Ry][Rz][Tx][Ty][Tz][P(x,y,z,w)]
 *
 * The coefficient of each transformation is returned in the corresponding
 * element of the vector tran.
 *
 * Returns 1 upon success, 0 if the matrix is singular.
 */
int
unmatrix( mat, tran )
Matrix4 *mat;
double tran[16];
{
 	register int i, j;
 	Matrix4 locmat;
 	Matrix4 pmat, invpmat, tinvpmat;
 	/* Vector4 type and functions need to be added to the common set. */
 	Vector4 prhs, psol;
 	Point3 row[3], pdum3;

 	locmat = *mat;
 	/* Normalize the matrix. */
 	if ( locmat.element[3][3] == 0 )
 		return 0;
 	for ( i=0; i<4;i++ )
 		for ( j=0; j<4; j++ )
 			locmat.element[i][j] /= locmat.element[3][3];
 	/* pmat is used to solve for perspective, but it also provides
 	 * an easy way to test for singularity of the upper 3x3 component.
 	 */
 	pmat = locmat;
 	for ( i=0; i<3; i++ )
 		pmat.element[i][3] = 0;
 	pmat.element[3][3] = 1;

 	if ( det4x4(&pmat) == 0.0 )
 		return 0;

 	/* First, isolate perspective.  This is the messiest. */
 	if ( locmat.element[0][3] != 0 || locmat.element[1][3] != 0 ||
 		locmat.element[2][3] != 0 ) {
 		/* prhs is the right hand side of the equation. */
 		prhs.x = locmat.element[0][3];
 		prhs.y = locmat.element[1][3];
 		prhs.z = locmat.element[2][3];
 		prhs.w = locmat.element[3][3];

 		/* Solve the equation by inverting pmat and multiplying
 		 * prhs by the inverse.  (This is the easiest way, not
 		 * necessarily the best.)
 		 * inverse function (and det4x4, above) from the Matrix
 		 * Inversion gem in the first volume.
 		 */
 		inverse( &pmat, &invpmat );
		TransposeMatrix4( &invpmat, &tinvpmat );
 		V4MulPointByMatrix(&prhs, &tinvpmat, &psol);
 
 		/* Stuff the answer away. */
 		tran[U_PERSPX] = psol.x;
 		tran[U_PERSPY] = psol.y;
 		tran[U_PERSPZ] = psol.z;
 		tran[U_PERSPW] = psol.w;
 		/* Clear the perspective partition. */
 		locmat.element[0][3] = locmat.element[1][3] =
 			locmat.element[2][3] = 0;
 		locmat.element[3][3] = 1;
 	} else		/* No perspective. */
 		tran[U_PERSPX] = tran[U_PERSPY] = tran[U_PERSPZ] =
 			tran[U_PERSPW] = 0;

 	/* Next take care of translation (easy). */
 	for ( i=0; i<3; i++ ) {
 		tran[U_TRANSX + i] = locmat.element[3][i];
 		locmat.element[3][i] = 0;
 	}

 	/* Now get scale and shear. */
 	for ( i=0; i<3; i++ ) {
 		row[i].x = locmat.element[i][0];
 		row[i].y = locmat.element[i][1];
 		row[i].z = locmat.element[i][2];
 	}

 	/* Compute X scale factor and normalize first row. */
 	tran[U_SCALEX] = V3Length(&row[0]);
 	row[0] = *V3Scale(&row[0], 1.0);

 	/* Compute XY shear factor and make 2nd row orthogonal to 1st. */
 	tran[U_SHEARXY] = V3Dot(&row[0], &row[1]);
 	(void)V3Combine(&row[1], &row[0], &row[1], 1.0, -tran[U_SHEARXY]);

 	/* Now, compute Y scale and normalize 2nd row. */
 	tran[U_SCALEY] = V3Length(&row[1]);
 	V3Scale(&row[1], 1.0);
 	tran[U_SHEARXY] /= tran[U_SCALEY];

 	/* Compute XZ and YZ shears, orthogonalize 3rd row. */
 	tran[U_SHEARXZ] = V3Dot(&row[0], &row[2]);
 	(void)V3Combine(&row[2], &row[0], &row[2], 1.0, -tran[U_SHEARXZ]);
 	tran[U_SHEARYZ] = V3Dot(&row[1], &row[2]);
 	(void)V3Combine(&row[2], &row[1], &row[2], 1.0, -tran[U_SHEARYZ]);

 	/* Next, get Z scale and normalize 3rd row. */
 	tran[U_SCALEZ] = V3Length(&row[2]);
 	V3Scale(&row[2], 1.0);
 	tran[U_SHEARXZ] /= tran[U_SCALEZ];
 	tran[U_SHEARYZ] /= tran[U_SCALEZ];
 
 	/* At this point, the matrix (in rows[]) is orthonormal.
 	 * Check for a coordinate system flip.  If the determinant
 	 * is -1, then negate the matrix and the scaling factors.
 	 */
 	if ( V3Dot( &row[0], V3Cross( &row[1], &row[2], &pdum3) ) < 0 )
 		for ( i = 0; i < 3; i++ ) {
 			tran[U_SCALEX+i] *= -1;
 			row[i].x *= -1;
 			row[i].y *= -1;
 			row[i].z *= -1;
 		}
 
 	/* Now, get the rotations out, as described in the gem. */
 	tran[U_ROTATEY] = asin(-row[0].z);
 	if ( cos(tran[U_ROTATEY]) != 0 ) {
 		tran[U_ROTATEX] = atan2(row[1].z, row[2].z);
 		tran[U_ROTATEZ] = atan2(row[0].y, row[0].x);
 	} else {
 		tran[U_ROTATEX] = atan2(-row[2].x, row[1].y);
 		tran[U_ROTATEZ] = 0;
 	}
 	/* All done! */
 	return 1;
}


/* transpose rotation portion of matrix a, return b */
Matrix4 *TransposeMatrix4(a, b)
Matrix4 *a, *b;
{
int i, j;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			b->element[i][j] = a->element[j][i];
	return(b);
}

/* multiply a hom. point by a matrix and return the transformed point */
Vector4 *V4MulPointByMatrix(pin, m, pout)
Vector4 *pin, *pout;
Matrix4 *m;
{
	pout->x = (pin->x * m->element[0][0]) + (pin->y * m->element[1][0]) +
		(pin->z * m->element[2][0]) + (pin->w * m->element[3][0]);
	pout->y = (pin->x * m->element[0][1]) + (pin->y * m->element[1][1]) +
		(pin->z * m->element[2][1]) + (pin->w * m->element[3][1]);
	pout->z = (pin->x * m->element[0][2]) + (pin->y * m->element[1][2]) +
		(pin->z * m->element[2][2]) + (pin->w * m->element[3][2]);
	pout->w = (pin->x * m->element[0][3]) + (pin->y * m->element[1][3]) +
		(pin->z * m->element[2][3]) + (pin->w * m->element[3][3]);
        return(pout);
}
