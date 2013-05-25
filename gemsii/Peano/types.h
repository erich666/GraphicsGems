/* types.h */
/* types and #define's for peano curve algorithm */

/* copyright Ken Musgrave */
/* June 1986 */


#define FB_SIZE		1024		/* frame buffer size */

#define MAX_DIMENSIONS	5		/* dimensionality of space */
#define MAX_PRECISION	11		/* number of bits/dimension -1 */

#define ODD(x)		( ( (x) & 0x1 )  ?  1 : 0 )

					/* right circular shift */
#define RT_CSHFT( byte, shift, dimensions, bytemask ) \
	((((byte) >> (shift)) | ((byte) << dimensions - (shift))) & bytemask)


typedef char	byte;			/* size must be >= MAX_PRECISION */

typedef int	vector[MAX_DIMENSIONS];	/* n-space vector */

typedef byte	r_array[MAX_PRECISION];	/* vector of type "r" in algo */


/* global variable section */

int	J[MAX_PRECISION]; 		/* storage for principal positions */

					/* global arrays */
r_array	rho, sigma, tau, tilde_sigma, tilde_tau, omega, alpha;

byte	bitmask[MAX_DIMENSIONS];	/* to be filled with bit masks */

int	dimensions;			/* number of dimensions being filled */
int	precision;			/* number of bits of precision used */
byte	bytemask; 			/* masks "dimensions" bits */

