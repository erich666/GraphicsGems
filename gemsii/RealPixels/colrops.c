/* Copyright (c) 1990 Regents of the University of California */

#ifndef lint
static char SCCSid[] = "@(#)colrops.c 1.3 11/9/90 LBL";
#endif

/*
 * Integer operations on COLR scanlines
 */

#include "color.h"

#define MAXGSHIFT	15		/* maximum shift for gamma table */

static BYTE	g_mant[256], g_nexp[256];

static BYTE	g_bval[MAXGSHIFT+1][256];


setcolrgam(g)			/* set gamma conversion */
double	g;
{
	extern double	pow();
	double	mult;
	register int	i, j;
					/* compute colr -> gamb mapping */
	for (i = 0; i <= MAXGSHIFT; i++) {
		mult = pow(0.5, (double)(i+8));
		for (j = 0; j < 256; j++)
			g_bval[i][j] = 256.0 * pow((j+.5)*mult, 1.0/g);
	}
					/* compute gamb -> colr mapping */
	i = 0;
	mult = 256.0;
	for (j = 255; j > 0; j--) {
		while ((g_mant[j] = mult * pow(j/256.0, g)) < 128) {
			i++;
			mult *= 2.0;
		}
		g_nexp[j] = i;
	}
	g_mant[0] = 0;
	g_nexp[0] = COLXS;
}


colrs_gambs(scan, len)		/* convert scanline of colrs to gamma bytes */
register COLR	*scan;
int	len;
{
	register int	i, expo;

	while (len-- > 0) {
		expo = scan[0][EXP] - COLXS;
		if (expo < -MAXGSHIFT) {
			if (expo < -MAXGSHIFT-8) {
				scan[0][RED] =
				scan[0][GRN] =
				scan[0][BLU] = 0;
			} else {
				i = (-MAXGSHIFT-1) - expo;
				scan[0][RED] = 
				g_bval[MAXGSHIFT][((scan[0][RED]>>i)+1)>>1];
				scan[0][GRN] =
				g_bval[MAXGSHIFT][((scan[0][GRN]>>i)+1)>>1];
				scan[0][BLU] =
				g_bval[MAXGSHIFT][((scan[0][BLU]>>i)+1)>>1];
			}
		} else if (expo > 0) {
			if (expo > 8) {
				scan[0][RED] =
				scan[0][GRN] =
				scan[0][BLU] = 255;
			} else {
				i = (scan[0][RED]<<1 | 1) << (expo-1);
				scan[0][RED] = i > 255 ? 255 : g_bval[0][i];
				i = (scan[0][GRN]<<1 | 1) << (expo-1);
				scan[0][GRN] = i > 255 ? 255 : g_bval[0][i];
				i = (scan[0][BLU]<<1 | 1) << (expo-1);
				scan[0][BLU] = i > 255 ? 255 : g_bval[0][i];
			}
		} else {
			scan[0][RED] = g_bval[-expo][scan[0][RED]];
			scan[0][GRN] = g_bval[-expo][scan[0][GRN]];
			scan[0][BLU] = g_bval[-expo][scan[0][BLU]];
		}
		scan[0][EXP] = COLXS;
		scan++;
	}
}


gambs_colrs(scan, len)		/* convert gamma bytes to colr scanline */
register COLR	*scan;
int	len;
{
	register int	nexpo;

	while (len-- > 0) {
		nexpo = g_nexp[scan[0][RED]];
		if (g_nexp[scan[0][GRN]] < nexpo)
			nexpo = g_nexp[scan[0][GRN]];
		if (g_nexp[scan[0][BLU]] < nexpo)
			nexpo = g_nexp[scan[0][BLU]];
		if (nexpo < g_nexp[scan[0][RED]])
			scan[0][RED] = g_mant[scan[0][RED]]
					>> (g_nexp[scan[0][RED]]-nexpo);
		else
			scan[0][RED] = g_mant[scan[0][RED]];
		if (nexpo < g_nexp[scan[0][GRN]])
			scan[0][GRN] = g_mant[scan[0][GRN]]
					>> (g_nexp[scan[0][GRN]]-nexpo);
		else
			scan[0][GRN] = g_mant[scan[0][GRN]];
		if (nexpo < g_nexp[scan[0][BLU]])
			scan[0][BLU] = g_mant[scan[0][BLU]]
					>> (g_nexp[scan[0][BLU]]-nexpo);
		else
			scan[0][BLU] = g_mant[scan[0][BLU]];
		scan[0][EXP] = COLXS - nexpo;
		scan++;
	}
}


shiftcolrs(scan, len, adjust)	/* shift a scanline of colors by 2^adjust */
register COLR	*scan;
register int	len;
register int	adjust;
{
	while (len-- > 0) {
		scan[0][EXP] += adjust;
		scan++;
	}
}


normcolrs(scan, len, adjust)	/* normalize a scanline of colrs */
register COLR  *scan;
int  len;
int  adjust;
{
	register int  c;
	register int  shift;

	while (len-- > 0) {
		shift = scan[0][EXP] + adjust - COLXS;
		if (shift > 0) {
			if (shift > 8) {
				scan[0][RED] =
				scan[0][GRN] =
				scan[0][BLU] = 255;
			} else {
				shift--;
				c = (scan[0][RED]<<1 | 1) << shift;
				scan[0][RED] = c > 255 ? 255 : c;
				c = (scan[0][GRN]<<1 | 1) << shift;
				scan[0][GRN] = c > 255 ? 255 : c;
				c = (scan[0][BLU]<<1 | 1) << shift;
				scan[0][BLU] = c > 255 ? 255 : c;
			}
		} else if (shift < 0) {
			if (shift < -8) {
				scan[0][RED] =
				scan[0][GRN] =
				scan[0][BLU] = 0;
			} else {
				shift = -1-shift;
				scan[0][RED] = ((scan[0][RED]>>shift)+1)>>1;
				scan[0][GRN] = ((scan[0][GRN]>>shift)+1)>>1;
				scan[0][BLU] = ((scan[0][BLU]>>shift)+1)>>1;
			}
		}
		scan[0][EXP] = COLXS - adjust;
		scan++;
	}
}
