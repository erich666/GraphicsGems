/* Copyright (c) 1990 Regents of the University of California */

/*
 * Integer operations on COLR scanlines
 */

#include "color.h"

#define MAXGSHIFT	15		/* maximum shift for gamma table */

static BYTE	g_mant[256], g_nexp[256];

static BYTE	g_bval[MAXGSHIFT+1][256];


void setcolrgam(double g)			/* set gamma conversion */
{
	extern double	pow();
	double	mult;
	register int	i, j;
					/* compute colr -> gamb mapping */
	for (i = 0; i <= MAXGSHIFT; i++) {
		mult = pow(0.5, (double)(i+8));
		for (j = 0; j < 256; j++)
			g_bval[i][j] = (BYTE)(256.0 * pow((j+.5)*mult, 1.0/g));
	}
					/* compute gamb -> colr mapping */
	i = 0;
	mult = 256.0;
	for (j = 255; j > 0; j--) {
		while ((g_mant[j] = (BYTE)(mult * pow(j/256.0, g))) < 128) {
			i++;
			mult *= 2.0;
		}
		g_nexp[j] = i;
	}
	g_mant[0] = 0;
	g_nexp[0] = COLXS;
}


void colrs_gambs(COLR* scan, int len)		/* convert scanline of colrs to gamma bytes */
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

void shiftcolrs(COLR* scan, int len, int adjust)	/* shift a scanline of colors by 2^adjust */
{
	while (len-- > 0) {
		scan[0][EXP] += adjust;
		scan++;
	}
}

void normcolrs(scan, len, adjust)	/* normalize a scanline of colrs */
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
