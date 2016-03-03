/*
	file:		utah.c
	description:	interface to Utah RLE toolkit
	author:		A. T. Campbell
	date:		October 27, 1989
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef sequent
#include <strings.h>
#else
#include <string.h>
#endif
#include "utah.h"

/******************************************************************************/

int utah_read_close(UTAH_FILE *ufp)
{
	return 0;
}

/******************************************************************************/

UTAH_FILE *
utah_read_init(fname, ht, wd)

char	*fname;
int	*ht, *wd;
{
	FILE		*fp;
	UTAH_FILE	*ufp;

	/* open output stream */
	if (!strcmp(fname, ""))
		fp = stdin;
	else {
		if ((fp = fopen(fname, "r")) == NULL)
		return(NULL);
	}

 	/* change the default rle_dflt_hdr struct to match what we need */
	ufp = (UTAH_FILE *) malloc(sizeof(UTAH_FILE));
	*ufp = rle_dflt_hdr;
	ufp->rle_file = fp;

	/* read the header in the input file */
  	if (rle_get_setup(ufp) != 0)
		return(NULL);

	/* get image size */
	*wd = ufp->xmax - ufp->xmin + 1;
	*ht = ufp->ymax - ufp->ymin + 1;

	/* normal termination */
	return(ufp);
}

/******************************************************************************/

int utah_read_pixels(UTAH_FILE *ufp, unsigned char pixels[][3])
{
	static unsigned	n = 0;
	static unsigned char	*r = NULL, *g = NULL, *b = NULL;
	int		i, width;

	/* allocate storage */
	width = ufp->xmax + 1;
	if (width > (int)n) {
		if (n > 0) {
			free((char *)r);
			free((char *)g);
			free((char *)b);
		}
		n = width;
		r = (unsigned char *) malloc(n * sizeof(unsigned char));
		g = (unsigned char *) malloc(n * sizeof(unsigned char));
		b = (unsigned char *) malloc(n * sizeof(unsigned char));
	}

	/* read this row */
	utah_read_rgb(ufp, r, g, b);

	/* convert to pixels */
	for (i = 0; i < width; i++) {
		pixels[i][0] = r[i];
		pixels[i][1] = g[i];
		pixels[i][2] = b[i];
	}

	return(0);
}

/******************************************************************************/

int utah_read_rgb(UTAH_FILE *ufp, unsigned char	r[], unsigned char g[], unsigned char b[])
{
	rle_pixel	*rows[3];

	/* set color channels */
	rows[0] = r;
	rows[1] = g;
	rows[2] = b;

	/* read this row */
	rle_getrow(ufp, rows);
	return(0);
}

/******************************************************************************/

int utah_write_close(UTAH_FILE	*ufp)
{
	if (!ufp) return(-1);
	rle_puteof(ufp);
	return(0);
}

/******************************************************************************/

UTAH_FILE *
utah_write_init(fname, ht, wd)

char	*fname;
int	ht, wd;
{
	FILE		*fp;
	UTAH_FILE	*ufp;

	/* open output stream */
	if (!strcmp(fname, ""))
		fp = stdout;
	else {
		if ((fp = fopen(fname, "w")) == NULL)
		return(NULL);
	}

 	/* change the default rle_dflt_hdr struct to match what we need */
	ufp = (UTAH_FILE *) malloc(sizeof(UTAH_FILE));
	*ufp = rle_dflt_hdr;
	ufp->rle_file = fp;
	ufp->xmax = wd - 1;
	ufp->ymax = ht - 1;
	ufp->alpha = 0;	/* No coverage (alpha) */

	/* create the header in the output file */
  	rle_put_setup(ufp);

	/* normal termination */
	return ufp;
}

/******************************************************************************/

int utah_write_pixels(UTAH_FILE *ufp, unsigned char	pixels[][3])
{
	static unsigned	n = 0;
	static unsigned char	*r = NULL, *g = NULL, *b = NULL;
	int		i, width;

	/* allocate storage */
	width = ufp->xmax + 1;
	if (width > (int)n) {
		if (n > 0) {
			free((char *)r);
			free((char *)g);
			free((char *)b);
		}
		n = width;
		r = (unsigned char *) malloc(n * sizeof(unsigned char));
		g = (unsigned char *) malloc(n * sizeof(unsigned char));
		b = (unsigned char *) malloc(n * sizeof(unsigned char));
	}

	/* convert to color channels */
	for (i = 0; i < width; i++) {
		r[i] = pixels[i][0];
		g[i] = pixels[i][1];
		b[i] = pixels[i][2];
	}

	/* write this row */
	utah_write_rgb(ufp, r, g, b);
	return 0;
}

/******************************************************************************/

int utah_write_rgb(UTAH_FILE *ufp, unsigned char r[], unsigned char g[], unsigned char b[])
{
	rle_pixel	*rows[3];
	int		width;

	/* set color channels */
	rows[0] = r;
	rows[1] = g;
	rows[2] = b;

	/* write this row */
	width = ufp->xmax - ufp->xmin + 1;
	rle_putrow(rows, width, ufp);
	return(0);
}

/******************************************************************************/
