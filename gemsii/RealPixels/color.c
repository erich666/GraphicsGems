/* Copyright (c) 1991 Regents of the University of California */

/*
 *  color.c - routines for color calculations.
 *
 *     10/10/85
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include  "color.h"

#define  MINELEN	8	/* minimum scanline length for encoding */
#define  MINRUN		4	/* minimum run length */


char *
tempbuffer(len)			/* get a temporary buffer */
unsigned int len;
{
	static char  *tempbuf = NULL;
	static int  tempbuflen = 0;

	if ((int)len > tempbuflen) {
		if (tempbuflen > 0)
			tempbuf = realloc(tempbuf, len);
		else
			tempbuf = malloc(len);
		tempbuflen = tempbuf==NULL ? 0 : len;
	}
	return(tempbuf);
}


int fwritecolrs(COLR* scanline, int len, FILE* fp)		/* write out a colr scanline */
{
	register int  i, j, beg, cnt;
	int  c2;
	
	if (len < MINELEN)		/* too small to encode */
		return (int)(fwrite((char *)scanline,sizeof(COLR),len,fp) - len);
	if (len > 32767)		/* too big! */
		return(-1);
	putc(2, fp);			/* put magic header */
	putc(2, fp);
	putc(len>>8, fp);
	putc(len&255, fp);
					/* put components separately */
	for (i = 0; i < 4; i++) {
	    for (j = 0; j < len; j += cnt) {	/* find next run */
		for (beg = j; beg < len; beg += cnt) {
		    for (cnt = 1; cnt < 127 && beg+cnt < len &&
			    scanline[beg+cnt][i] == scanline[beg][i]; cnt++)
			;
		    if (cnt >= MINRUN)
			break;			/* long enough */
		}
		if (beg-j > 1 && beg-j < MINRUN) {
		    c2 = j+1;
		    while (scanline[c2++][i] == scanline[j][i])
			if (c2 == beg) {	/* short run */
			    putc(128+beg-j, fp);
			    putc(scanline[j][i], fp);
			    j = beg;
			    break;
			}
		}
		while (j < beg) {		/* write out non-run */
		    if ((c2 = beg-j) > 128) c2 = 128;
		    putc(c2, fp);
		    while (c2--)
			putc(scanline[j++][i], fp);
		}
		if (cnt >= MINRUN) {		/* write out run */
		    putc(128+cnt, fp);
		    putc(scanline[beg][i], fp);
		} else
		    cnt = 0;
	    }
	}
	return(ferror(fp) ? -1 : 0);
}


int oldreadcolrs(COLR* scanline, int len, FILE* fp)		/* read in an old colr scanline */
{
	int  rshift;
	register int  i;

	rshift = 0;

	while (len > 0) {
		scanline[0][RED] = getc(fp);
		scanline[0][GRN] = getc(fp);
		scanline[0][BLU] = getc(fp);
		scanline[0][EXP] = getc(fp);
		if (feof(fp) || ferror(fp))
			return(-1);
		if (scanline[0][RED] == 1 &&
			scanline[0][GRN] == 1 &&
			scanline[0][BLU] == 1) {
				for (i = scanline[0][EXP] << rshift; i > 0; i--) {
					copycolr(scanline[0], scanline[-1]);
					scanline++;
					len--;
				}
				rshift += 8;
		} else {
			scanline++;
			len--;
			rshift = 0;
		}
	}
	return 0;
}


int freadcolrs(COLR* scanline, int len, FILE* fp)		/* read in an encoded colr scanline */
{
	register int  i, j;
	int  code;
					/* determine scanline type */
	if (len < MINELEN)
		return(oldreadcolrs(scanline, len, fp));
	if ((i = getc(fp)) == EOF)
		return(-1);
	if (i != 2) {
		ungetc(i, fp);
		return(oldreadcolrs(scanline, len, fp));
	}
	scanline[0][GRN] = getc(fp);
	scanline[0][BLU] = getc(fp);
	if ((i = getc(fp)) == EOF)
		return(-1);
	if (scanline[0][GRN] != 2 || scanline[0][BLU] & 128) {
		scanline[0][RED] = 2;
		scanline[0][EXP] = i;
		return(oldreadcolrs(scanline+1, len-1, fp));
	}
	if ((scanline[0][BLU]<<8 | i) != len)
		return(-1);		/* length mismatch! */
					/* read each component */
	for (i = 0; i < 4; i++)
	    for (j = 0; j < len; ) {
		if ((code = getc(fp)) == EOF)
		    return(-1);
		if (code > 128) {	/* run */
		    scanline[j++][i] = getc(fp);
		    for (code &= 127; --code; j++)
			scanline[j][i] = scanline[j-1][i];
		} else			/* non-run */
		    while (code--)
			scanline[j++][i] = getc(fp);
	    }
	return(feof(fp) ? -1 : 0);
}


void setcolr(clr, r, g, b)		/* assign a short color value */
	register COLR  clr;
double  r, g, b;
{
	double  frexp();
	double  d;
	int  e;

	d = r > g ? r : g;
	if (b > d) d = b;

	if (d <= 1e-32) {
		clr[RED] = clr[GRN] = clr[BLU] = 0;
		clr[EXP] = 0;
		return;
	}

	d = frexp(d, &e) * 256.0 / d;

	clr[RED] = (BYTE)(r * d);
	clr[GRN] = (BYTE)(g * d);
	clr[BLU] = (BYTE)(b * d);
	clr[EXP] = e + COLXS;
}


size_t fwritescan(COLOR* scanline, int len, FILE* fp)		/* write out a scanline */
{
	COLR  *clrscan;
	int  n;
	register COLR  *sp;
					/* get scanline buffer */
	if ((sp = (COLR *)tempbuffer(len*sizeof(COLR))) == NULL)
		return(-1);
	clrscan = sp;
					/* convert scanline */
	n = len;
	while (n-- > 0) {
		setcolr(sp[0], scanline[0][RED],
				  scanline[0][GRN],
				  scanline[0][BLU]);
		scanline++;
		sp++;
	}
	return(fwritecolrs(clrscan, len, fp));
}


void colr_color(COLOR col, COLR clr)		/* convert short to float color */
{
	double  f;

	if (clr[EXP] == 0)
		col[RED] = col[GRN] = col[BLU] = 0.0;
	else {
		f = ldexp(1.0, (int)clr[EXP]-(COLXS+8));
		col[RED] = (BYTE)((clr[RED] + 0.5)*f);
		col[GRN] = (BYTE)((clr[GRN] + 0.5)*f);
		col[BLU] = (BYTE)((clr[BLU] + 0.5)*f);
	}
}


int freadscan(COLOR* scanline, int len, FILE* fp)		/* read in a scanline */
{
	COLR  *clrscan;

	if ((clrscan = (COLR *)tempbuffer(len*sizeof(COLR))) == NULL)
		return -1;
	if (freadcolrs(clrscan, len, fp) < 0)
		return -1 ;
					/* convert scanline */
	colr_color(scanline[0], clrscan[0]);
	while (--len > 0) {
		scanline++; clrscan++;
		if (clrscan[0][RED] == clrscan[-1][RED] &&
			    clrscan[0][GRN] == clrscan[-1][GRN] &&
			    clrscan[0][BLU] == clrscan[-1][BLU] &&
			    clrscan[0][EXP] == clrscan[-1][EXP])
			copycolor(scanline[0], scanline[-1]);
		else
			colr_color(scanline[0], clrscan[0]);
	}
	return 0;
}


int bigdiff(COLOR c1, COLOR c2, double md)			/* c1 delta c2 > md? */
{
	register int  i;

	for (i = 0; i < 3; i++)
		if (colval(c1,i)-colval(c2,i) > md*colval(c2,i) ||
			colval(c2,i)-colval(c1,i) > md*colval(c1,i))
			return(1);
	return(0);
}
