/*
 * unmatrix.h - Definitions for using unmatrix
 *
 * Author:	Spencer W. Thomas
 *		University of Michigan
 */

/* The unmatrix subroutine fills in a vector of floating point
 * values.  These symbols make it easier to get the data back out.
 */

enum unmatrix_indices {
 	U_SCALEX,
 	U_SCALEY,
 	U_SCALEZ,
 	U_SHEARXY,
 	U_SHEARXZ,
 	U_SHEARYZ,
 	U_ROTATEX,
 	U_ROTATEY,
 	U_ROTATEZ,
 	U_TRANSX,
 	U_TRANSY,
 	U_TRANSZ,
 	U_PERSPX,
 	U_PERSPY,
 	U_PERSPZ,
 	U_PERSPW
};

typedef struct {
	double x,y,z,w;
} Vector4;

Matrix4 *TransposeMatrix4();
Vector4 *V4MulPointByMatrix();
