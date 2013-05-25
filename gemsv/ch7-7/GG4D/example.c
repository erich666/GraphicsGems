/* example.c
 *
 * Steve Hill, Computing Laboratory, University of Kent, UK.
 *
 * Email: S.A.Hill@ukc.ac.uk
 *
 * This file contains a short example of using the 4D vector
 * library.  It concentrates on the matrix functions since these
 * are the main extension not present in the original Graphics Gems
 * libraries.
 */


#include <stdio.h>
#include <math.h>
#include "GGems.h"
#include "GGems4d.h"

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

void
Example1()
{
	Vector4	*a, *b, *c, r;
	Vector3 *a1, *b1, r1;

	a = V4New(1.0, 0.0, 0.0, 0.0);
	b = V4New(0.0, 1.0, 0.0, 0.0);
	c = V4New(0.0, 0.0, 1.0, 0.0);

	a1 = V3New(1.0, 0.0, 0.0);
	b1 = V3New(0.0, 1.0, 0.0);

	V3Cross(a1, b1, &r1);
	V4Cross(a, b, c, &r);

	printf("(%lf, %lf, %lf)\n", r1.x, r1.y, r1.z);
	printf("(%lf, %lf, %lf, %lf)\n", r.x, r.y, r.z, r.w);
}

/* Example2
 *
 * Constructing a compound transformation using matrix product.
 * T(-1,-2,-3,-4) o Rxw(PI) o T(1, 2, 3, 4)
 */

void
Example2()
{
	Vector4	v = {-1.0, -2.0, -3.0, -4.0};
	Matrix5	m1, m2;

	V4MatTranslation(&m1, &v);
	V4MatRotationXW(&m2, M_PI);
	V4MatMul2(&m1, &m2, &m1);
	V4Negate(&v);
	V4MatTranslation(&m2, &v);
	V4MatMul2(&m1, &m2, &m1);

	V4MatPrint(stdout, &m1);
}

/* Example3
 *
 * Constructing a compound transformation using affine operations
 * T(-1,-2,-3,-4) o Rxw(PI) o T(1, 2, 3, 4)
 */

void
Example3()
{
	Vector4 v = {-1.0, -2.0, -3.0, -4.0};
	Matrix5	m;

	V4MatIdentity(&m);
	V4MatTranslate(&m, v);
	V4MatRotateXW(&m, M_PI);
	V4Negate(&v);
	V4MatTranslate(&m, v);
	
	V4MatPrint(stdout, &m);
}

/* Example4
 *
 * All transformation functions return a pointer to their
 * result, it is possible therefore to use the result directly in
 * subsequent function call.
 * Here is Example3 recoded in this style.
 * NB
 * Some care has to be exercised since the order of argument evaluation
 * is not defined.
 */

void
Example4()
{
	Matrix5	m;
	Vector4	v = {-1.0, -2.0, -3.0, -4.0};

	V4MatRotateXW(V4MatTranslate(V4MatIdentity(&m), &v), M_PI);
	V4MatTranslate(&m, V4Negate(&v));

	V4MatPrint(stdout, &m);
}

void
main()
{
	Example1();
	putchar('\n');
	Example2();
	putchar('\n');
	Example3();
	putchar('\n');
	Example4();
}
