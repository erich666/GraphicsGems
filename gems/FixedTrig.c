/* 
Fixed-Point Trigonometry with CORDIC Iterations
by Ken Turkowski
from "Graphics Gems", Academic Press, 1990

provided by user:
    frmul(a,b)=(a*b)>>31, high part of 64-bit product
*/

#define COSCALE 0x22c2dd1c /* 0.271572 */
#define QUARTER ((int)(3.141592654 / 2.0 * (1 << 28)))
static long arctantab[32] = {  /* MS 4 integral bits for radians */
	297197971, 210828714, 124459457, 65760959, 33381290, 16755422,
	8385879, 4193963, 2097109, 1048571, 524287, 262144, 131072,
	65536, 32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64,
	32, 16, 8, 4, 2, 1, 0, 0,
};

CordicRotate(px, py, theta)
long *px, *py;
register long theta;	/* Assume that abs(theta) <= pi */
{
	register int i;
	register long x = *px, y = *py, xtemp;
	register long *arctanptr = arctantab;

	/* The -1 may need to be pulled out and done as a left shift */
	for (i = -1; i <= 28; i++) {
		if (theta < 0) {
			xtemp = x + (y >> i);
			y     = y - (x >> i);
			x = xtemp;
			theta += *arctanptr++;
		} else {
			xtemp = x - (y >> i);	
			y     = y + (x >> i);
			x = xtemp;	
			theta -= *arctanptr++;
		}
	}

	*px = frmul(x, COSCALE); /* Compensate for CORDIC enlargement */
	*py = frmul(y, COSCALE); /* frmul(a,b)=(a*b)>>31, high part  */
				 /* of 64-bit product */
}




CordicPolarize(argx, argy)
long *argx, *argy;	/* We assume these are already in the */
					/*  right half plane */
{
	register long theta = 0, yi, i;
	register long x = *argx, y = *argy;
	register long *arctanptr = arctantab;
	for (i = -1; i <= 28; i++) {
		if (y < 0) {		/* Rotate positive */
			yi = y + (x >> i);
			x  = x - (y >> i);
			y  = yi;
			theta -= *arctanptr++;
		} else { 		/* Rotate negative */
			yi = y - (x >> i);
			x  = x + (y >> i);
			y  = yi;
			theta += *arctanptr++;
		}
	}

	*argx = frmul(x, COSCALE);
	*argy = theta;
}
