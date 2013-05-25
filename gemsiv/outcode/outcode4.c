/* Direct calculation of 4D outcodes, C code fragment */

/* number of bits in a word */
#define NUMBITS	   sizeof(long)*8

/* get the integral form of a floating point number */
#define v(x)	   *(long *) &(x)

/* get the sign bit of an integer as a value 0 or 1 */
#define s(x)	   (((unsigned long)(x)) >> (NUMBITS-1))

/* get the absolute value of a floating point number in integral form */
#define a(x)	   ((x) & ~(1 << (NUMBITS-1)))

	iw = v(w);
	abs_w = a(iw);
	outcode = s(iw);	  /* 0 or 1 per w's sign */

	ix = v(x);
	diff = s(abs_w - a(ix));
	t = s(ix) + 1;
	outcode |= diff << t;	  /* 0, 2, or 4 or'd with outcode */

	iy = v(y);
	diff = s(abs_w - a(iy));
	t = s(iy) + 3;
	outcode |= diff << t;	  /* 0, 8, or 16 or'd with outcode */

	iz = v(z);
	diff = s(abs_w - a(iz));
	t = s(iz) + 5;
	outcode |= diff << t;	  /* 0, 32, or 64 or'd with outcode */
