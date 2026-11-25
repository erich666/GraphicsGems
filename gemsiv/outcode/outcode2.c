/* Direct calculation of 2D outcodes, C code fragment */

#include <stdint.h>

/* number of bits in a float; was sizeof(long)*8, wrong where long is 64 bits */
#define NUMBITS	   32

/* get the integral form of a floating point number */
#define v(x)	   (*(int32_t *) &(x))

/* get the sign bit of an integer as a value 0 or 1 */
#define s(x)	   (((uint32_t)(x)) >> (NUMBITS-1))

/* get the absolute value of a floating point number in integral form */
#define a(x)	   ((x) & 0x7fffffff)

void test() {
	float x = 0.f, y = 0.f, xmin = 0.f, xmax = 0.f, ymin = 0.f, ymax = 0.f;
	long int iy, ixmin, ixmax, iymin, iymax, ix, ax, ay;
	unsigned long outcode;

/* these values typically would be calculated once and cached */
	ixmin = v(xmin);
	ixmax = v(xmax);
	iymin = v(ymin);
	iymax = v(ymax);

	/* get the bits and absolute value */
	ix = v(x); ax = a(ix);
	/* put the sign bit in bit 0 */
	outcode = s(ix | (ax - ixmin));

	/* put the sign bit in bit 1 */
	outcode |= s(~ix & (ixmax - ax)) << 1;

	/* do the same for y */
	iy = v(y);
	ay = a(iy);
	outcode |= s(iy | (ay - iymin))	 << 2;
	outcode |= s(~iy & (iymax - ay)) << 3;
}
