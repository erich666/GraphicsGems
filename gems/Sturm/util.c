
/*
 * util.c
 *
 *	some utlity functions for root polishing and evaluating
 * polynomials.
 */
#include <math.h>
#include <stdio.h>
#include "solve.h"

/*
 * evalpoly
 *
 *	evaluate polynomial defined in coef returning its value.
 */
double
evalpoly (ord, coef, x)
	int		ord;
	double	*coef, x;
{
	double	*fp, f;

	fp = &coef[ord];
	f = *fp;

	for (fp--; fp >= coef; fp--)
	f = x * f + *fp;

	return(f);
}


/*
 * modrf
 *
 *	uses the modified regula-falsi method to evaluate the root
 * in interval [a,b] of the polynomial described in coef. The
 * root is returned is returned in *val. The routine returns zero
 * if it can't converge.
 */
int
modrf(ord, coef, a, b, val)
	int		ord;
	double	*coef;
	double	a, b, *val;
{
	int		its;
	double	fa, fb, x, fx, lfx;
	double	*fp, *scoef, *ecoef;

	scoef = coef;
	ecoef = &coef[ord];

	fb = fa = *ecoef;
	for (fp = ecoef - 1; fp >= scoef; fp--) {
		fa = a * fa + *fp;
		fb = b * fb + *fp;
	}

	/*
	 * if there is no sign difference the method won't work
	 */
	if (fa * fb > 0.0)
		return(0);

	if (fabs(fa) < RELERROR) {
		*val = a;
		return(1);
	}

	if (fabs(fb) < RELERROR) {
		*val = b;
		return(1);
	}

	lfx = fa;


	for (its = 0; its < MAXIT; its++) {

		x = (fb * a - fa * b) / (fb - fa);

		fx = *ecoef;
		for (fp = ecoef - 1; fp >= scoef; fp--)
				fx = x * fx + *fp;

		if (fabs(x) > RELERROR) {
				if (fabs(fx / x) < RELERROR) {
					*val = x;
					return(1);
				}
		} else if (fabs(fx) < RELERROR) {
				*val = x;
				return(1);
		}

		if ((fa * fx) < 0) {
				b = x;
				fb = fx;
				if ((lfx * fx) > 0)
					fa /= 2;
		} else {
				a = x;
				fa = fx;
				if ((lfx * fx) > 0)
					fb /= 2;
		}

		lfx = fx;
	}

	fprintf(stderr, "modrf overflow %f %f %f\n", a, b, fx);

	return(0);
}
	


