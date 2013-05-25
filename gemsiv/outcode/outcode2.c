/* Direct calculation of 2D outcodes, C code fragment */

/* number of bits in a word */
#define NUMBITS	   sizeof(long)*8

/* get the integral form of a floating point number */
#define v(x)	   *(long *) &(x)

/* get the sign bit of an integer as a value 0 or 1 */
#define s(x)	   (((unsigned long)(x)) >> (NUMBITS-1))

/* get the absolute value of a floating point number in integral form */
#define a(x)	   ((x) & ~(1 << (NUMBITS-1)))

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
