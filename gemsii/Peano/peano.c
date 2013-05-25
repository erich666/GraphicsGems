/* peano.c */

/* copyright Ken Musgrave */
/* June 1986 */

/*
 * space-filling peano curve algorithm.
 * fills n-space with a 1-D peano curve.
 * 
 * the algorithm is utterly incomprehensible,
 * so expect a paucity of sensible comments in this code.
 * it consists largely of bit-wise logical operations,
 * and is therefore quite impenetrable.
 * but it works.
 */


#include "types.h"



/*
 * determine the n-space coordinate of "point" on the peano curve
 */
peano(coord, point)
vector          coord;
int             point;
{
	int             i;

	zero(sigma);		/* initialize necessary arrays */
	zero(tilde_sigma);
	zero(tilde_tau);

	build_rho(point);
	for (i=0; i<precision; i++)
		J[i] = principal_pos(rho[i]);
	build_sigma();
	build_tau();
	build_tilde_sigma();
	build_tilde_tau();
	build_omega();
	build_alpha();

	v_convert(alpha, coord);

} /* peano() */


/*
 * build "rho" array
 */
build_rho(point)
int             point;
{
	int	i, mask=bytemask;

	for (i=0; i<precision; i++) {
		rho[precision - i - 1] = (point & mask) >> (i * dimensions);
		mask <<= dimensions;
	}

} /* build_rho() */


/*
 * find principal position of "a_byte" 
 */
principal_pos(a_byte)
byte            a_byte;
{
	int             nth_bit, i=1;

	nth_bit = a_byte & 0x01;
	for (i=1; i<dimensions; i++) {
		if (((a_byte & bitmask[dimensions - i - 1]) >> i) != nth_bit)
			return (dimensions - i);
	}

	return (dimensions);	/* all bits are the same */

} /* principal_pos() */


/*
 * build "sigma" array
 */
build_sigma()
{
	int             i, bit;

	for (i=0; i<precision; i++) {
		sigma[i] |= rho[i] & bitmask[0];
		for (bit=1; bit<dimensions; bit++) {
			sigma[i] |= (rho[i] & bitmask[bit])
				^ ((rho[i] & bitmask[bit - 1]) >> 1);
		}
	}

} /* build_sigma() */


/*
 * build "tau" array 
 */
build_tau()
{
	int             parity, bit, i, j;
	byte            temp_byte;

	for (i=0; i<precision; i++) {
		parity = 0;
						/* complement nth bit */
		if (sigma[i] & bitmask[dimensions - 1])
			tau[i] = sigma[i] - 1;	/* nth bit was 1 */
		else
			tau[i] = sigma[i] + 1;	/* nth bit was 0 */

		for (j=0; j<dimensions; j++)	/* find parity */
			if (tau[i] & bitmask[j])
				parity++;

		if (ODD(parity)) {	/* complement principal bit */
			bit = J[i] - 1;	/* get index of principal bit */
					/* get bit in question */
			temp_byte = tau[i] & bitmask[bit];
			tau[i] |= bitmask[bit];	/* set the bit to 1 */
			tau[i] &= ~temp_byte;	/* assign complement */
		}
	}

} /* build_tau() */


/*
 * build "tilde_sigma" array 
 */
build_tilde_sigma()
{
	int             i, shift=0;

	tilde_sigma[0] = sigma[0];
	for (i=1; i<precision; i++) {
		shift += J[i - 1] - 1;
		shift %= dimensions;
		tilde_sigma[i]= RT_CSHFT(sigma[i], shift, dimensions, bytemask);
	}

} /* build_tilde_sigma() */


/*
 * build "tilde_tau" array 
 */
build_tilde_tau()
{
	int             i, shift=0;

	tilde_tau[0] = tau[0];
	for (i=1; i<precision; i++) {
		shift += J[i - 1] - 1;
		shift %= dimensions;
		tilde_tau[i] = RT_CSHFT(tau[i], shift, dimensions, bytemask);
	}

} /* build_tilde_tau() */


/*
 * build "omega" array 
 */
build_omega()
{
	int             i;

	omega[0] = 0;
	for (i=1; i<precision; i++)
		omega[i] = omega[i - 1] ^ tilde_tau[i - 1];

} /* build_omega() */


/*
 * build "alpha" array 
 */
build_alpha()
{
	int             i;

	for (i=0; i<precision; i++)
		alpha[i] = omega[i] ^ tilde_sigma[i];

} /* build_alpha() */


/*
 * initialize "array" to zeros 
 */
zero(array)

	r_array         array;

{
	int             i;

	for (i=0; i<precision; i++)
		array[i] = 0;

} /* zero() */


/*
 * convert "alpha" array into n_space coordinate vector 
 */
v_convert(alph, coord)
r_array         alph;
vector          coord;
{
	int             i, j, bit, a_bitmask=1;

	for (i=0; i<dimensions; i++) {
		coord[i] = 0;
		bit = precision;
		for (j=0; j<precision; j++)	/* extract each bit of coord
						 * i */
			coord[i] |= ((alph[j] & a_bitmask) << --bit) >> i;
		a_bitmask <<= 1;
	}

} /* v_convert() */

