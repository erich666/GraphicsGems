
/*
 * sturm.c
 *
 *	the functions to build and evaluate the Sturm sequence
 */
#include <math.h>
#include <stdio.h>
#include "solve.h"

/*
 * modp
 *
 *	calculates the modulus of u(x) / v(x) leaving it in r, it
 *  returns 0 if r(x) is a constant.
 *  note: this function assumes the leading coefficient of v 
 *	is 1 or -1
 */
static int
modp(u, v, r)
	poly	*u, *v, *r;
{
	int		k, j;
	double	*nr, *end, *uc;

	nr = r->coef;
	end = &u->coef[u->ord];

	uc = u->coef;
	while (uc <= end)
			*nr++ = *uc++;

	if (v->coef[v->ord] < 0.0) {


			for (k = u->ord - v->ord - 1; k >= 0; k -= 2)
				r->coef[k] = -r->coef[k];

			for (k = u->ord - v->ord; k >= 0; k--)
				for (j = v->ord + k - 1; j >= k; j--)
					r->coef[j] = -r->coef[j] - r->coef[v->ord + k]
					* v->coef[j - k];
	} else {
			for (k = u->ord - v->ord; k >= 0; k--)
				for (j = v->ord + k - 1; j >= k; j--)
				r->coef[j] -= r->coef[v->ord + k] * v->coef[j - k];
	}

	k = v->ord - 1;
	while (k >= 0 && fabs(r->coef[k]) < SMALL_ENOUGH) {
		r->coef[k] = 0.0;
		k--;
	}

	r->ord = (k < 0) ? 0 : k;

	return(r->ord);
}

/*
 * buildsturm
 *
 *	build up a sturm sequence for a polynomial in smat, returning
 * the number of polynomials in the sequence
 */
int
buildsturm(ord, sseq)
	int		ord;
	poly	*sseq;
{
	int		i;
	double	f, *fp, *fc;
	poly	*sp;

	sseq[0].ord = ord;
	sseq[1].ord = ord - 1;


	/*
	 * calculate the derivative and normalise the leading
	 * coefficient.
	 */
	f = fabs(sseq[0].coef[ord] * ord);
	fp = sseq[1].coef;
	fc = sseq[0].coef + 1;
	for (i = 1; i <= ord; i++)
			*fp++ = *fc++ * i / f;

	/*
	 * construct the rest of the Sturm sequence
	 */
	for (sp = sseq + 2; modp(sp - 2, sp - 1, sp); sp++) {

		/*
		 * reverse the sign and normalise
		 */
		f = -fabs(sp->coef[sp->ord]);
		for (fp = &sp->coef[sp->ord]; fp >= sp->coef; fp--)
				*fp /= f;
	}

	sp->coef[0] = -sp->coef[0];	/* reverse the sign */

	return(sp - sseq);
}

/*
 * numroots
 *
 *	return the number of distinct real roots of the polynomial
 * described in sseq.
 */
int
numroots(np, sseq, atneg, atpos)
		int		np;
		poly	*sseq;
		int		*atneg, *atpos;
{
		int		atposinf, atneginf;
		poly	*s;
		double	f, lf;

		atposinf = atneginf = 0;


	/*
	 * changes at positive infinity
	 */
	lf = sseq[0].coef[sseq[0].ord];

	for (s = sseq + 1; s <= sseq + np; s++) {
			f = s->coef[s->ord];
			if (lf == 0.0 || lf * f < 0)
				atposinf++;
		lf = f;
	}

	/*
	 * changes at negative infinity
	 */
	if (sseq[0].ord & 1)
			lf = -sseq[0].coef[sseq[0].ord];
	else
			lf = sseq[0].coef[sseq[0].ord];

	for (s = sseq + 1; s <= sseq + np; s++) {
			if (s->ord & 1)
				f = -s->coef[s->ord];
			else
				f = s->coef[s->ord];
			if (lf == 0.0 || lf * f < 0)
				atneginf++;
			lf = f;
	}

	*atneg = atneginf;
	*atpos = atposinf;

	return(atneginf - atposinf);
}

/*
 * numchanges
 *
 *	return the number of sign changes in the Sturm sequence in
 * sseq at the value a.
 */
int
numchanges(np, sseq, a)
	int		np;
	poly	*sseq;
	double	a;

{
	int		changes;
	double	f, lf;
	poly	*s;

	changes = 0;

	lf = evalpoly(sseq[0].ord, sseq[0].coef, a);

	for (s = sseq + 1; s <= sseq + np; s++) {
			f = evalpoly(s->ord, s->coef, a);
			if (lf == 0.0 || lf * f < 0)
				changes++;
			lf = f;
	}

	return(changes);
}

/*
 * sbisect
 *
 *	uses a bisection based on the sturm sequence for the polynomial
 * described in sseq to isolate intervals in which roots occur,
 * the roots are returned in the roots array in order of magnitude.
 */
sbisect(np, sseq, min, max, atmin, atmax, roots)
	int		np;
	poly	*sseq;
	double	min, max;
	int		atmin, atmax;
	double	*roots;
{
	double	mid;
	int		n1 = 0, n2 = 0, its, atmid, nroot;

	if ((nroot = atmin - atmax) == 1) {

		/*
		 * first try a less expensive technique.
		 */
		if (modrf(sseq->ord, sseq->coef, min, max, &roots[0]))
			return;


		/*
		 * if we get here we have to evaluate the root the hard
		 * way by using the Sturm sequence.
		 */
		for (its = 0; its < MAXIT; its++) {
				mid = (min + max) / 2;

				atmid = numchanges(np, sseq, mid);

				if (fabs(mid) > RELERROR) {
					if (fabs((max - min) / mid) < RELERROR) {
						roots[0] = mid;
						return;
					}
				} else if (fabs(max - min) < RELERROR) {
					roots[0] = mid;
					return;
				}

				if ((atmin - atmid) == 0)
					min = mid;
				else
					max = mid;
			}

		if (its == MAXIT) {
				fprintf(stderr, "sbisect: overflow min %f max %f\
					diff %e nroot %d n1 %d n2 %d\n",
					min, max, max - min, nroot, n1, n2);
			roots[0] = mid;
		}

		return;
	}

	/*
	 * more than one root in the interval, we have to bisect...
	 */
	for (its = 0; its < MAXIT; its++) {

			mid = (min + max) / 2;

			atmid = numchanges(np, sseq, mid);

			n1 = atmin - atmid;
			n2 = atmid - atmax;


			if (n1 != 0 && n2 != 0) {
				sbisect(np, sseq, min, mid, atmin, atmid, roots);
				sbisect(np, sseq, mid, max, atmid, atmax, &roots[n1]);
				break;
			}

			if (n1 == 0)
				min = mid;
			else
				max = mid;
	}

	if (its == MAXIT) {
			fprintf(stderr, "sbisect: roots too close together\n");
			fprintf(stderr, "sbisect: overflow min %f max %f diff %e\
				nroot %d n1 %d n2 %d\n",
				min, max, max - min, nroot, n1, n2);
			for (n1 = atmax; n1 < atmin; n1++)
			roots[n1 - atmax] = mid;
	}
}
