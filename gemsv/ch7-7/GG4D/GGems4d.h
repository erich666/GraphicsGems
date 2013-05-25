#ifndef GG4D_H
#define GG4D_H	1

/*********************/
/* 4d geometry types */
/*********************/

/* Point4, Vector4
 *
 * 4-space point and vector types
 */

typedef struct Point4Struct
{
	double	x, y, z, w;
}
Point4;

typedef Point4 Vector4;

/* IntPoint4
 *
 * Integer 4-space point.
 */

typedef struct IntPoint4Struct
{
	int	x, y, z, w;
}
IntPoint4;

/* Matrix5
 *
 * 5x5 matrix for general linear 4-space transformations
 */

typedef struct Matrix5Struct
{
	double	element[5][5];
}
Matrix5;

/* Box4
 *
 * A bounding box for four dimensions.
 */

typedef struct Box4dStruct
{
	Point4	min, max;
}
Box4;

/* See C file for function elaboration.
 */

extern double	V4SquaredLength(), V4Length();
extern double	V4Dot(), V4DistanceBetween2Points();
extern Vector4	*V4Negate(), *V4Normalize(), *V4Scale(), *V4Add(), *V4Sub();
extern Vector4	*V4Lerp(), *V4Combine(), *V4Mul(), *V4Cross();
extern Vector4	*V4New(), *V4Duplicate();
extern Point4	*V4MulPointByMatrix(), *V4MulPointByProjMatrix();
extern Matrix5	*V4MatMul();

extern void	V4MatPrint();
extern Matrix5	*V4MatCopy(), *V4MatMul(), *V4MatMul2();
extern Matrix5	*V4MatZero(), *V4MatIdentity();
extern Matrix5	*V4MatRotationXY(), *V4MatRotationXW(), *V4MatRotationYZ();
extern Matrix5	*V4MatRotationYW(), *V4MatRotationZX(), *V4MatRotationZW();
extern Matrix5	*V4MatTranslation(), *V4MatScaling();
extern Matrix5	*V4MatRotateXY(), *V4MatRotateXW(), *V4MatRotateYZ();
extern Matrix5	*V4MatRotateYW(), *V4MatRotateZX(), *V4MatRotateZW();
extern Matrix5	*V4MatTranslate(), *V4MatScale();

#endif
