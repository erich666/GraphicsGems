/*
Using Sturm Sequences to Bracket Real Roots of Polynomial Equations
by D.G. Hook and P.R. McAree
from "Graphics Gems", Academic Press, 1990
*/

/*
 * main.c
 *
 *	a sample driver program.
 */
#include <stdio.h>
#include <math.h>
#include "solve.h"

/*
 * a driver program for a root solver.
 */
main()
{
	poly	sseq[MAX_ORDER];
	double 	min, max, roots[MAX_ORDER];
	int		i, j, order, nroots, nchanges, np, atmin, atmax;

	/*
	 * get the details...
	 */

	printf("Please enter order of polynomial: ");
	scanf("%d", &order);

	printf("\n");

	for (i = order; i >= 0; i--) {
			printf("Please enter coefficient number %d: ", i);
			scanf("%lf", &sseq[0].coef[i]);
	}

	printf("\n");

	/*
	 * build the Sturm sequence
	 */
	np = buildsturm(order, sseq);

	printf("Sturm sequence for:\n");

	for (i = order; i >= 0; i--)
			printf("%lf ", sseq[0].coef[i]);

	printf("\n\n");

	for (i = 0; i <= np; i++) {
			for (j = sseq[i].ord; j >= 0; j--)
				printf("%lf ", sseq[i].coef[j]);
			printf("\n");
	}

	printf("\n");


	/* 
	 * get the number of real roots
	 */
	nroots = numroots(np, sseq, &atmin, &atmax);

	if (nroots == 0) {
			printf("solve: no real roots\n");
			exit(0);
	}

	/*
	 * calculate the bracket that the roots live in
	 */
	min = -1.0;
	nchanges = numchanges(np, sseq, min);
	for (i = 0; nchanges != atmin && i != MAXPOW; i++) { 
			min *= 10.0;
			nchanges = numchanges(np, sseq, min);
	}

	if (nchanges != atmin) {
			printf("solve: unable to bracket all negative roots\n");
			atmin = nchanges;
	}

	max = 1.0;
	nchanges = numchanges(np, sseq, max);
	for (i = 0; nchanges != atmax && i != MAXPOW; i++) { 
			max *= 10.0;
			nchanges = numchanges(np, sseq, max);
	}

	if (nchanges != atmax) {
			printf("solve: unable to bracket all positive roots\n");
			atmax = nchanges;
	}

	nroots = atmin - atmax;

	/*
	 * perform the bisection.
	 */
	sbisect(np, sseq, min, max, atmin, atmax, roots);


	/*
	 * write out the roots...
	 */
	if (nroots == 1) {
			printf("\n1 distinct real root at x = %f\n", roots[0]);
	} else {
			printf("\n%d distinct real roots for x: ", nroots);

			for (i = 0; i != nroots; i++)
				printf("%f ", roots[i]);
			printf("\n");
	}
}
