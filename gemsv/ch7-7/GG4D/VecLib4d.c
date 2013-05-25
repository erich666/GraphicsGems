#include <stdio.h>
#include <stdlib.h>	/* for malloc() definition */
#include <math.h>
#include "GGems.h"
#include "GGems4d.h"

/* 4D VECTOR LIBRARY.
 *
 * Steve Hill, Computing Laboratory, University of Kent, UK
 *
 * Email: S.A.Hill@ukc.ac.uk
 *
 * Included with Graphics Gems V, Academic Press ed. Alan Paeth.
 *
 * Extends the standard Graphics Gems library to four dimensions
 * and provides functions to create and manipulate homogeneous
 * transformations for 4-vectors (including projections).
 */

/* V4SquaredLength()
 *
 * Compute the square of the magnitude of a 4-vector
 */

double
V4SquaredLength(v)
Vector4 *v;
{
	return v->x * v->x +
	       v->y * v->y +
	       v->z * v->z +
	       v->w * v->w;
}

/* V4Length()
 *
 * Compute the magnitude of a 4-vector
 * If square of magnitude is required use V4SquaredLength().
 */

double
V4Length(v)
Vector4	*v;
{
	return sqrt(V4SquaredLength(v));
}

/* V4Negate()
 *
 * Negate vector elementwise.
 */

Vector4 *
V4Negate(v)
Vector4	*v;
{
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
	v->w = -v->w;

	return v;
}

/* V4Normalize()
 *
 * Convert vector to unit magnitude.
 * A zero-length vector is unchanged.
 */

Vector4 *
V4Normalize(v)
Vector4	*v;
{
	double	len = V4Length(v);

	if (len != 0.0)
	{
		v->x /= len;
		v->y /= len;
		v->z /= len;
		v->w /= len;
	}

	return v;
}

/* V4Scale()
 *
 * Scale vector to requested magnitude.
 * Zero length vectors unchanged.
 */

Vector4 *
V4Scale(v, newlen)
Vector4	*v;
double	newlen;
{
	double	len = V4Length(v);

	if (len != 0.0)
	{
		v->x *= newlen/len;
		v->y *= newlen/len;
		v->z *= newlen/len;
		v->w *= newlen/len;
	}

	return v;
}

/* V4Add()
 *
 * Add (elementwise) two 4-vectors
 */

Vector4 *
V4Add(a, b, result)
Vector4	*a, *b, *result;
{
	result->x = a->x + b-> x;
	result->y = a->y + b-> y;
	result->z = a->z + b-> z;
	result->w = a->w + b-> w;

	return result;
}

/* V4Sub()
 *
 * Subtract (elementwise) vector b from vector a.
 */

Vector4 *
V4Sub(a, b, result)
Vector4	*a, *b, *result;
{
	result->x = a->x - b-> x;
	result->y = a->y - b-> y;
	result->z = a->z - b-> z;
	result->w = a->w - b-> w;

	return result;
}

/* V4Dot()
 *
 * Returns the 4-space dot-product of two vectors.
 */

double
V4Dot(a, b)
Vector4	*a, *b;
{
	return	a->x * b->x +
		a->y * b->y +
		a->z * b->z +
		a->w * b->w;
}

/* V4Lerp()
 *
 * Calculates vector that is a linear interpolation between
 * lo and hi, accoreding to alpha.
 * See also GraphicsGems.h (GGems.h)
 */

Vector4 *
V4Lerp(lo, hi, alpha, result)
Vector4	*lo, *hi, *result;
double	alpha;
{
	result->x = LERP(alpha, lo->x, hi->x);
	result->y = LERP(alpha, lo->y, hi->y);
	result->z = LERP(alpha, lo->z, hi->z);
	result->w = LERP(alpha, lo->w, hi->w);

	return result;
}

/* V4Combine()
 *
 * Calculate a linear combination of two vectors.
 */

Vector4 *
V4Combine(a, b, result, ascl, bscl)
Vector4	*a, *b, *result;
double	ascl, bscl;
{
	result->x = ascl * a->x + bscl * b->x;
	result->y = ascl * a->y + bscl * b->y;
	result->z = ascl * a->z + bscl * b->z;
	result->w = ascl * a->w + bscl * b->w;

	return result;
}

/* V4Mul()
 *
 * Multiply (elementwise) two vectors.
 */

Vector4 *
V4Mul(a, b, result)
Vector4	*a, *b, *result;
{
	result->x = a->x * b->x;
	result->y = a->y * b->y;
	result->z = a->z * b->z;
	result->w = a->w * b->w;

	return result;
}

/* V4DistanceBetween2Points()
 *
 * Calculates the distance between two 4-space points.
 */

double
V4DistanceBetween2Points(a, b)
Point4	*a, *b;
{
	double	dx = a->x - b->x,
		dy = a->y - b->y,
		dz = a->z - b->z,
		dw = a->w - b->w;

	return sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}

/* V4Cross()
 *
 * Calculates the cross product of three 4-space vectors.
 */

Vector4 *
V4Cross(a, b, c, result)
Vector4	*a, *b, *c, *result;
{
	double	d1, d2, d3, d4, d5, d6;

	d1 = (b->z * c->w) - (b->w * c->z);
	d2 = (b->y * c->w) - (b->w * c->y);
	d3 = (b->y * c->z) - (b->z * c->y);
	d4 = (b->x * c->w) - (b->w * c->x);
	d5 = (b->x * c->z) - (b->z * c->x);
	d6 = (b->x * c->y) - (b->y * c->x);

	result->x = - a->y * d1 + a->z * d2 - a->w * d3;
	result->y =   a->x * d1 - a->z * d4 + a->w * d5;
	result->z = - a->x * d2 + a->y * d4 - a->w * d6;
	result->w =   a->x * d3 - a->y * d5 + a->z * d6;

	return result;
}

/* V4New()
 *
 * Allocate and initialise a new 4-vector
 */

Vector4 *
V4New(x, y, z, w)
double	x, y, z, w;
{
	Vector4	*v = NEWTYPE(Vector4);

	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;

	return v;
}

/* V4Duplicate()
 *
 * Create a copy of a 4-vector.
 */

Vector4 *
V4Duplicate(a)
Vector4	*a;
{
	Vector4	*v = NEWTYPE(Vector4);

	v->x = a->x;
	v->y = a->y;
	v->z = a->z;
	v->w = a->w;

	return v;
}

/* V4MatPrint()
 *
 * Diagnostic function to print out a 5x5 matrix
 *
 */

void
V4MatPrint(file, mat)
FILE	*file;
Matrix5	*mat;
{
	int	i, j;

	for (i = 0; i < 5; i += 1)
	{
		for (j = 0; j < 5; j += 1)
			fprintf(file, "%lf ", mat->element[i][j]);
		putc('\n', file);
	}
}

/* V4MatCopy()
 *
 * Copy a 5x5 matrix.
 */

Matrix5 *
V4MatCopy(from, to)
Matrix5	*from, *to;
{
	int	i, j;

	for (i = 0; i < 5; i += 1)
	for (j = 0; j < 5; j += 1)
		to->element[i][j] = from->element[i][j];

	return to;
}

/* V4MulPointByMatrix()
 *
 * Apply 4x4 transformation matrix to 4-space point.
 *
 * In common with the standard Graphics Gems library, points/vectors
 * are considered to be row vectors, hence multiplication is
 * post-multiplication.  The transformation T o S o R represents
 * a translation followed by a scale followed by a rotation.
 * Where o is matrix multiplication (see V4MatMul() and V4MatMul2() below).
 */

Point4 *
V4MulPointByMatrix(pin, m, pout)
Point4	*pin, *pout;
Matrix4	*m;
{
	pout->x = pin->x * m->element[0][0] +
		  pin->y * m->element[1][0] +
		  pin->z * m->element[2][0] +
		  pin->w * m->element[3][0];
	pout->y = pin->x * m->element[0][1] +
		  pin->y * m->element[1][1] +
		  pin->z * m->element[2][1] +
		  pin->w * m->element[3][1];
	pout->z = pin->x * m->element[0][2] +
		  pin->y * m->element[1][2] +
		  pin->z * m->element[2][2] +
		  pin->w * m->element[3][2];
	pout->w = pin->x * m->element[0][3] +
		  pin->y * m->element[1][3] +
		  pin->z * m->element[2][3] +
		  pin->w * m->element[3][3];

	return pout;
}

/* V4MulPointByProjMatrix()
 *
 * Apply 5x5 (projection) matrix to 4-space point
 *
 * For example, the projection matrix:
 *
 *   [  1  0  0  0    0    ]
 *   [  0  1  0  0    0    ]
 *   [  0  0  0  0   1/dz  ]
 *   [  0  0  0  0   1/dw  ]
 *   [  0  0  0  0    1    ]
 *
 * represents a combined perspective/parallel projection along
 * the Z and W axes.  As dz or dw tend to infinity the projections
 * along those axes becomes parallel.
 *
 * 5x5 matrices may also be used to perform translations.
 *
 *   [  1  0  0  0  0 ]
 *   [  0  1  0  0  0 ]
 *   [  0  0  1  0  0 ]
 *   [  0  0  0  1  0 ]
 *   [  x  y  z  w  1 ]
 */

Point4 *
V4MulPointByProjMatrix(pin, m, pout)
Point4	*pin, *pout;
Matrix5	*m;
{
	double	v;

	pout->x = pin->x * m->element[0][0] +
		  pin->y * m->element[1][0] +
		  pin->z * m->element[2][0] +
		  pin->w * m->element[3][0] +
		  m->element[4][0];
	pout->y = pin->x * m->element[0][1] +
		  pin->y * m->element[1][1] +
		  pin->z * m->element[2][1] +
		  pin->w * m->element[3][1] +
		  m->element[4][1];
	pout->z = pin->x * m->element[0][2] +
		  pin->y * m->element[1][2] +
		  pin->z * m->element[2][2] +
		  pin->w * m->element[3][2] +
		  m->element[4][2];
	pout->w = pin->x * m->element[0][3] +
		  pin->y * m->element[1][3] +
		  pin->z * m->element[2][3] +
		  pin->w * m->element[3][3] +
		  m->element[4][3];

	v = pin->x * m->element[0][4] +
	    pin->y * m->element[1][4] +
	    pin->z * m->element[2][4] +
	    pin->w * m->element[3][4] +
	    m->element[4][4];

	if (v != 0.0)
	{
		pout->x /= v;
		pout->y /= v;
		pout->z /= v;
		pout->w /= v;
	}

	return pout;
}

/* V4MatMul()
 *
 * Multiply two 5x5 matrices.  Result matrix must be distinct
 * from argument matrices.
 */

Matrix5 *
V4MatMul(a, b, result)
Matrix5	*a, *b, *result;
{
	int	i, j, k;

	for (i = 0; i < 5; i++)
	for (j = 0; j < 5; j++)
	{
		double t = 0.0;

		for (k = 0; k < 5; k++)
			t += a->element[i][k] * b->element[k][j];

		result->element[i][j] = t;
	}

	return result;
}

/* V4MatMul2
 *
 * Variation of V4MatMul() where result matrix may be the same
 * as matrices a or b.
 */

Matrix5 *
V4MatMul2(a, b, result)
Matrix5 *a, *b, *result;
{
	Matrix5	tmp;

	V4MatMul(a, b, &tmp);
	V4MatCopy(&tmp, result);
	return result;
}

/* 4D TRANSFORMATIONS
 *
 * The following functions provide various methods
 * to apply and manipulate transformations.
 */

/* V4MatZero()
 *
 * Clear a 5x5 matrix.
 */

Matrix5 *
V4MatZero(mat)
Matrix5	*mat;
{
	int	i, j;

	for (i = 0; i < 5; i += 1)
	for (j = 0; j < 5; j += 1)
		mat->element[i][j] = 0.0;
}

/* STANDARD TRANSFORMATIONS
 *
 * The following functions initialise the standard affine transformations
 * ie. Identity, Rotation (6 kinds), Translation and Scaling.
 */

/* V4MatIdentity()
 *
 * Initialises to identity matrix.
 */

Matrix5 *
V4MatIdentity(mat)
Matrix5	*mat;
{
	int	i, j;

	for (i = 0; i < 5; i += 1)
	for (j = 0; j < 5; j += 1)
		mat->element[i][j] = i == j ? 1.0 : 0.0;
}

/* V4MatRotationXY() etc.
 *
 * Initialises a matrix to a rotation by theta about the indicated hyperplane.
 */

Matrix5 *
V4MatRotationXY(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	s, c;

	s = sin(theta);
	c = cos(theta);

	V4MatIdentity(mat);
	mat->element[0][0] = c;
	mat->element[0][1] = s;
	mat->element[1][0] = -s;
	mat->element[1][1] = c;

	return mat;
}

Matrix5 *
V4MatRotationXW(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	s, c;

	s = sin(theta);
	c = cos(theta);

	V4MatIdentity(mat);
	mat->element[0][0] = c;
	mat->element[0][3] = s;
	mat->element[3][0] = -s;
	mat->element[3][3] = c;

	return mat;
}

Matrix5 *
V4MatRotationYZ(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	s, c;

	s = sin(theta);
	c = cos(theta);

	V4MatIdentity(mat);
	mat->element[1][1] = c;
	mat->element[1][2] = s;
	mat->element[2][1] = -s;
	mat->element[2][2] = c;

	return mat;
}

Matrix5 *
V4MatRotationYW(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	s, c;

	s = sin(theta);
	c = cos(theta);

	V4MatIdentity(mat);
	mat->element[1][1] = c;
	mat->element[1][3] = -s;
	mat->element[3][1] = s;
	mat->element[3][3] = c;

	return mat;
}

Matrix5 *
V4MatRotationZX(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	s, c;

	s = sin(theta);
	c = cos(theta);

	V4MatIdentity(mat);
	mat->element[0][0] = c;
	mat->element[0][2] = -s;
	mat->element[2][0] = s;
	mat->element[2][2] = c;

	return mat;
}

Matrix5 *
V4MatRotationZW(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	s, c;

	s = sin(theta);
	c = cos(theta);

	V4MatIdentity(mat);
	mat->element[2][2] = c;
	mat->element[2][3] = -s;
	mat->element[3][2] = s;
	mat->element[3][3] = c;

	return mat;
}

/* V4MatTranslation()
 *
 * Initialise a matrix to the translation v
 */

Matrix5 *
V4MatTranslation(mat, v)
Matrix5	*mat;
Vector4	*v;
{
	V4MatIdentity(mat);
	mat->element[4][0] = v->x;
	mat->element[4][1] = v->y;
	mat->element[4][2] = v->z;
	mat->element[4][3] = v->w;

	return mat;
}

/* V4MatScaling()
 *
 * Initialise a matrix to the scaling v
 */

Matrix5 *
V4MatScaling(mat, v)
Matrix5	*mat;
Vector4	*v;
{
	V4MatIdentity(mat);
	mat->element[0][0] = v->x;
	mat->element[1][1] = v->y;
	mat->element[2][2] = v->z;
	mat->element[3][3] = v->w;

	return mat;
}

/* MATRIX OPERATIONS
 *
 * The following functions are provided for a fast compact method
 * to create compound transformations.  Matrix multiplication has
 * been folded into the functions thus eliminating the need for
 * many redundant calculations.  For sparse matrices containing
 * mainly just ones and zeros this represents a considerable saving.
 *
 * Each function takes the form F(M, args) and has the effect
 * M := M o F1(args) where F1 is the transformation corresponding
 * to F.
 */

Matrix5	*
V4MatRotateXY(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	a, b, f, g, k, l, p, q, u, v;
	double	st, ct;

	st = sin(theta);
	ct = cos(theta);

	a = mat->element[0][0]; b = mat->element[0][1];
	f = mat->element[1][0]; g = mat->element[1][1];
	k = mat->element[2][0]; l = mat->element[2][1];
	p = mat->element[3][0]; q = mat->element[3][1];
	u = mat->element[4][0]; v = mat->element[4][1];

	mat->element[0][0] = a * ct - b * st;
	mat->element[0][1] = a * st + b * ct;
	mat->element[1][0] = f * ct - g * st;
	mat->element[1][1] = f * st + g * ct;
	mat->element[2][0] = k * ct - l * st;
	mat->element[2][1] = k * st + l * ct;
	mat->element[3][0] = p * ct - q * st;
	mat->element[3][1] = p * st + q * ct;
	mat->element[4][0] = u * ct - v * st;
	mat->element[4][1] = u * st + v * ct;

	return mat;
}

Matrix5	*
V4MatRotateXW(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	a, d, f, i, k, n, p, s, u, x;
	double	st, ct;

	st = sin(theta);
	ct = cos(theta);

	a = mat->element[0][0]; d = mat->element[0][3];
	f = mat->element[1][0]; i = mat->element[1][3];
	k = mat->element[2][0]; n = mat->element[2][3];
	p = mat->element[3][0]; s = mat->element[3][3];
	u = mat->element[4][0]; x = mat->element[4][3];

	mat->element[0][0] = a * ct - d * st;
	mat->element[0][3] = a * st + d * ct;
	mat->element[1][0] = f * ct - i * st;
	mat->element[1][3] = f * st + i * ct;
	mat->element[2][0] = k * ct - n * st;
	mat->element[2][3] = k * st + n * ct;
	mat->element[3][0] = p * ct - s * st;
	mat->element[3][3] = p * st + s * ct;
	mat->element[4][0] = u * ct - x * st;
	mat->element[4][3] = u * st + x * ct;

	return mat;
}

Matrix5	*
V4MatRotateYZ(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	b, c, g, h, l, m, q, r, v, w;
	double st, ct;

	st = sin(theta);
	ct = cos(theta);

	b = mat->element[0][1]; c = mat->element[0][2];
	g = mat->element[1][1]; h = mat->element[1][2];
	l = mat->element[2][1]; m = mat->element[2][2];
	q = mat->element[3][1]; r = mat->element[3][2];
	v = mat->element[4][1]; w = mat->element[4][2];

	mat->element[0][1] = b * ct - c * st;
	mat->element[0][2] = b * st + c * ct;
	mat->element[1][1] = g * ct - h * st;
	mat->element[1][2] = g * st + h * ct;
	mat->element[2][1] = l * ct - m * st;
	mat->element[2][2] = l * st + m * ct;
	mat->element[3][1] = q * ct - r * st;
	mat->element[3][2] = q * st + r * ct;
	mat->element[4][1] = v * ct - w * st;
	mat->element[4][2] = v * st + w * ct;

	return mat;
}

Matrix5	*
V4MatRotateYW(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	b, d, g, i, l, n, q, s, v, x;
	double	st, ct;

	st = sin(theta);
	ct = cos(theta);

	b = mat->element[0][1]; d = mat->element[0][3];
	g = mat->element[1][1]; i = mat->element[1][3];
	l = mat->element[2][1]; n = mat->element[2][3];
	q = mat->element[3][1]; s = mat->element[3][3];
	v = mat->element[4][1]; x = mat->element[4][3];

	mat->element[0][1] =   b * ct + d * st;
	mat->element[0][3] = - b * st + d * ct;
	mat->element[1][1] =   g * ct + i * st;
	mat->element[1][3] = - g * st + i * ct;
	mat->element[2][1] =   l * ct + n * st;
	mat->element[2][3] = - l * st + n * ct;
	mat->element[3][1] =   q * ct + s * st;
	mat->element[3][3] = - q * st + s * ct;
	mat->element[4][1] =   v * ct + x * st;
	mat->element[4][3] = - v * st + x * ct;

	return mat;
}

Matrix5	*
V4MatRotateZX(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	a, c, f, h, k, m, p, r, u, w;
	double	st, ct;

	st = sin(theta);
	ct = cos(theta);

	a = mat->element[0][0]; c = mat->element[0][2];
	f = mat->element[1][0]; h = mat->element[1][2];
	k = mat->element[2][0]; m = mat->element[2][2];
	p = mat->element[3][0]; r = mat->element[3][2];
	u = mat->element[4][0]; w = mat->element[4][2];

	mat->element[0][0] =   a * ct + c * st;
	mat->element[0][2] = - a * st + c * ct;
	mat->element[1][0] =   f * ct + h * st;
	mat->element[1][2] = - f * st + h * ct;
	mat->element[2][0] =   k * ct + m * st;
	mat->element[2][2] = - k * st + m * ct;
	mat->element[3][0] =   p * ct + r * st;
	mat->element[3][2] = - p * st + r * ct;
	mat->element[4][0] =   u * ct + w * st;
	mat->element[4][2] = - u * st + w * ct;

	return mat;
}


Matrix5	*
V4MatRotateZW(mat, theta)
Matrix5	*mat;
double	theta;
{
	double	c, d, h, i, m, n, r, s, w, x;
	double	st, ct;

	st = sin(theta);
	ct = cos(theta);

	c = mat->element[0][2]; d = mat->element[0][3];
	h = mat->element[1][2]; i = mat->element[1][3];
	m = mat->element[2][2]; n = mat->element[2][3];
	r = mat->element[3][2]; s = mat->element[3][3];
	w = mat->element[4][2]; x = mat->element[4][3];

	mat->element[0][2] =   c * ct + d * st;
	mat->element[0][3] = - c * st + d * ct;
	mat->element[1][2] =   h * ct + i * st;
	mat->element[1][3] = - h * st + i * ct;
	mat->element[2][2] =   m * ct + n * st;
	mat->element[2][3] = - m * st + n * ct;
	mat->element[3][2] =   r * ct + s * st;
	mat->element[3][3] = - r * st + s * ct;
	mat->element[4][2] =   w * ct + x * st;
	mat->element[4][3] = - w * st + x * ct;

	return mat;
}

Matrix5	*
V4MatTranslate(mat, v)
Matrix5	*mat;
Vector4	*v;
{
	double	e, j, o, t, y;

	e = mat->element[0][4];
	j = mat->element[1][4];
	o = mat->element[2][4];
	t = mat->element[3][4];
	y = mat->element[4][4];

	if (e != 0.0)
	{
		mat->element[0][0] += e * v->x;
		mat->element[0][1] += e * v->y;
		mat->element[0][2] += e * v->z;
		mat->element[0][3] += e * v->w;
	}
	if (j != 0.0)
	{
		mat->element[1][0] += j * v->x;
		mat->element[1][1] += j * v->y;
		mat->element[1][2] += j * v->z;
		mat->element[1][3] += j * v->w;
	}
	if (o != 0.0)
	{
		mat->element[2][0] += o * v->x;
		mat->element[2][1] += o * v->y;
		mat->element[2][2] += o * v->z;
		mat->element[2][3] += o * v->w;
	}
	if (t != 0.0)
	{
		mat->element[3][0] += t * v->x;
		mat->element[3][1] += t * v->y;
		mat->element[3][2] += t * v->z;
		mat->element[3][3] += t * v->w;
	}
	if (y != 0.0)
	{
		mat->element[4][0] += y * v->x;
		mat->element[4][1] += y * v->y;
		mat->element[4][2] += y * v->z;
		mat->element[4][3] += y * v->w;
	}

	return mat;
}

Matrix5 *
V4MatScale(mat, v)
Matrix5	*mat;
Vector4	*v;
{
	if (v->x != 1.0)
	{
		mat->element[0][0] *= v->x;
		mat->element[1][0] *= v->x;
		mat->element[2][0] *= v->x;
		mat->element[3][0] *= v->x;
		mat->element[4][0] *= v->x;
	}
	if (v->y != 1.0)
	{
		mat->element[0][1] *= v->y;
		mat->element[1][1] *= v->y;
		mat->element[2][1] *= v->y;
		mat->element[3][1] *= v->y;
		mat->element[4][1] *= v->y;
	}
	if (v->z != 1.0)
	{
		mat->element[0][2] *= v->z;
		mat->element[1][2] *= v->z;
		mat->element[2][2] *= v->z;
		mat->element[3][2] *= v->z;
		mat->element[4][2] *= v->z;
	}
	if (v->w != 1.0)
	{
		mat->element[0][3] *= v->w;
		mat->element[1][3] *= v->w;
		mat->element[2][3] *= v->w;
		mat->element[3][3] *= v->w;
		mat->element[4][3] *= v->w;
	}

	return mat;
}

/* Postscript
 *
 * The elements of the 5x5 matrix are labelled in the above functions
 * as follows:
 *
 * a = mat->element[0][0];
 * b = mat->element[0][1];
 * c = mat->element[0][2];
 * d = mat->element[0][3];
 * e = mat->element[0][4];
 * f = mat->element[1][0];
 * g = mat->element[1][1];
 * h = mat->element[1][2];
 * i = mat->element[1][3];
 * j = mat->element[1][4];
 * k = mat->element[2][0];
 * l = mat->element[2][1];
 * m = mat->element[2][2];
 * n = mat->element[2][3];
 * o = mat->element[2][4];
 * p = mat->element[3][0];
 * q = mat->element[3][1];
 * r = mat->element[3][2];
 * s = mat->element[3][3];
 * t = mat->element[3][4];
 * u = mat->element[4][0];
 * v = mat->element[4][1];
 * w = mat->element[4][2];
 * x = mat->element[4][3];
 * y = mat->element[4][4];
 *
 * ie.
 * [ a b c d e ]
 * [ f g h i j ]
 * [ k l m n o ]
 * [ p q r s t ]
 * [ u v w x y ]
 *
 */
