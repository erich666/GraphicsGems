/*
 * A High Speed, Low Precision Square Root
 * by Paul Lalonde and Robert Dawson
 * from "Graphics Gems", Academic Press, 1990
 */


/*
 * SPARC implementation of a fast square root by table 
 * lookup.
 * SPARC floating point format is as follows:
 *
 * BIT 31 	30 	23 	22 	0
 *     sign	exponent	mantissa
 */

#include <math.h>

static short sqrttab[0x100];	/* declare table of square roots */

void
build_table()
{
	unsigned short i;
	float f;
	unsigned int *fi = (unsigned int *)&f;
				/* to access the bits of a float in  */
				/* C quickly we must misuse pointers */

	for(i=0; i<= 0x7f; i++) {
		*fi = 0;

		/*
		 * Build a float with the bit pattern i as mantissa
		 * and an exponent of 0, stored as 127
  		 */

		*fi = (i << 16) | (127 << 23);
		f = sqrt(f);

		/*
		 * Take the square root then strip the first 7 bits of
		 * the mantissa into the table
		 */

		sqrttab[i] = (*fi & 0x7fffff) >> 16;

		/*
		 * Repeat the process, this time with an exponent of
		 * 1, stored as 128
		 */

		*fi = 0;
		*fi = (i << 16) | (128 << 23);
		f = sqrt(f);
		sqrttab[i+0x80] = (*fi & 0x7fffff) >> 16;
	}
}

/*
 * fsqrt - fast square root by table lookup
 */
float
fsqrt(float n)
{
	unsigned int *num = (unsigned int *)&n;
				/* to access the bits of a float in C
				 * we must misuse pointers */
							
	short e;		/* the exponent */
	if (n == 0) return (0);	/* check for square root of 0 */
	e = (*num >> 23) - 127;	/* get the exponent - on a SPARC the */
				/* exponent is stored with 127 added */
	*num &= 0x7fffff;	/* leave only the mantissa */
	if (e & 0x01) *num |= 0x800000;
				/* the exponent is odd so we have to */
				/* look it up in the second half of  */
				/* the lookup table, so we set the   */
				/* high bit				   */
	e >>= 1;		/* divide the exponent by two */
				/* note that in C the shift */
				/* operators are sign preserving */
				/* for signed operands */
	/* Do the table lookup, based on the quaternary mantissa,
	 * then reconstruct the result back into a float
	 */
	*num = ((sqrttab[*num >> 16]) << 16) | ((e + 127) << 23);
	return(n);
}
