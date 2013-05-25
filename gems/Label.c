/*
 * Nice Numbers for Graph Labels
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * label.c: demonstrate nice graph labeling
 *
 * Paul Heckbert	2 Dec 88
 */

#include <stdio.h>
#include <math.h>
#include "GraphicsGems.h"

double nicenum();

/* expt(a,n)=a^n for integer n */

#ifdef POW_NOT_TRUSTWORTHY
/* if roundoff errors in pow cause problems, use this: */

double expt(a, n)
double a;
register int n;
{
    double x;

    x = 1.;
    if (n>0) for (; n>0; n--) x *= a;
    else for (; n<0; n++) x /= a;
    return x;
}

#else
#   define expt(a, n) pow(a, (double)(n))
#endif

#define NTICK 5			/* desired number of tick marks */

main(ac, av)
int ac;
char **av;
{
    double min, max;

    if (ac!=3) {
	fprintf(stderr, "Usage: label <min> <max>\n");
	exit(1);
    }
    min = atof(av[1]);
    max = atof(av[2]);

    loose_label(min, max);
}

/*
 * loose_label: demonstrate loose labeling of data range from min to max.
 * (tight method is similar)
 */

loose_label(min, max)
double min, max;
{
    char str[6], temp[20];
    int nfrac;
    double d;				/* tick mark spacing */
    double graphmin, graphmax;		/* graph range min and max */
    double range, x;

    /* we expect min!=max */
    range = nicenum(max-min, 0);
    d = nicenum(range/(NTICK-1), 1);
    graphmin = floor(min/d)*d;
    graphmax = ceil(max/d)*d;
    nfrac = MAX(-floor(log10(d)), 0);	/* # of fractional digits to show */
    sprintf(str, "%%.%df", nfrac);	/* simplest axis labels */

    printf("graphmin=%g graphmax=%g increment=%g\n", graphmin, graphmax, d);
    for (x=graphmin; x<graphmax+.5*d; x+=d) {
	sprintf(temp, str, x);
	printf("(%s)\n", temp);
    }
}

/*
 * nicenum: find a "nice" number approximately equal to x.
 * Round the number if round=1, take ceiling if round=0
 */

double nicenum(x, round)
double x;
int round;
{
    int expv;				/* exponent of x */
    double f;				/* fractional part of x */
    double nf;				/* nice, rounded fraction */

    expv = floor(log10(x));
    f = x/expt(10., expv);		/* between 1 and 10 */
    if (round)
	if (f<1.5) nf = 1.;
	else if (f<3.) nf = 2.;
	else if (f<7.) nf = 5.;
	else nf = 10.;
    else
	if (f<=1.) nf = 1.;
	else if (f<=2.) nf = 2.;
	else if (f<=5.) nf = 5.;
	else nf = 10.;
    return nf*expt(10., expv);
}
