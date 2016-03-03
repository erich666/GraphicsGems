/*
 * Read and write image resolutions.
 */

#include <stdlib.h>
#include <stdio.h>

#include "color.h"


void fputresolu(int ord, int xres, int yres, FILE* fp)		/* put x and y resolution */
{
	if (ord&YMAJOR)
		fprintf(fp, "%cY %d %cX %d\n",
				ord&YDECR ? '-' : '+', yres,
				ord&XDECR ? '-' : '+', xres);
	else
		fprintf(fp, "%cX %d %cY %d\n",
				ord&XDECR ? '-' : '+', xres,
				ord&YDECR ? '-' : '+', yres);
}


int fgetresolu(int* xrp, int* yrp, FILE* fp)		/* get x and y resolution */
{
	char  buf[64], *xndx, *yndx;
	register char  *cp;
	register int  ord;

	if (fgets(buf, sizeof(buf), fp) == NULL)
		return(-1);
	xndx = yndx = NULL;
	for (cp = buf+1; *cp; cp++)
		if (*cp == 'X')
			xndx = cp;
		else if (*cp == 'Y')
			yndx = cp;
	if (xndx == NULL || yndx == NULL)
		return(-1);
	ord = 0;
	if (xndx > yndx) ord |= YMAJOR;
	if (xndx[-1] == '-') ord |= XDECR;
	if (yndx[-1] == '-') ord |= YDECR;
	if ((*xrp = atoi(xndx+1)) <= 0)
		return(-1);
	if ((*yrp = atoi(yndx+1)) <= 0)
		return(-1);
	return(ord);
}
