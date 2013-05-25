#ifndef lint
static char SCCSid[] = "@(#)resolu.c 1.1 9/22/90 LBL";
#endif

/*
 * Read and write image resolutions.
 */

#include <stdio.h>

#include "color.h"


fputresolu(ord, xres, yres, fp)		/* put x and y resolution */
register int  ord;
int  xres, yres;
FILE  *fp;
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


fgetresolu(xrp, yrp, fp)		/* get x and y resolution */
int  *xrp, *yrp;
FILE  *fp;
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
