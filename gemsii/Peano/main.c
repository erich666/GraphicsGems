/* main.c */

/* copyright Ken Musgrave */
/* June 1986 */

/*
 * creates an 8-bit, 2-D peano curve image file
 * background is 0, curve segments range in value from 1 to 255
 *
 * NOTE: the peano curve can be n-dimensional, 
 * this is just an example of the use of the
 * routines in peano.c
 */


#include <stdio.h>
#include <math.h>
#include "types.h"

char           *image_file="im_file";

unsigned char   fb[FB_SIZE + 1][FB_SIZE + 1];
FILE           *outfile, *fopen();
vector          gcoord, glast_coord;


main(argc, argv)
int             argc;
char           *argv[];

{
	int             i;

		/* get command-line arg */
	if (argc != 2) {
		fprintf(stderr, "usage: %s precision\n", argv[0]);
		exit(0);
	}
	dimensions = 2; 	/* the dimension is fixed in this example */
	precision = atoi(argv[1]);
	if (precision < 0 || precision > MAX_PRECISION - 1) {
		fprintf(stderr, "%s: can't work with %d bits of precision!\n", argv[0], precision);
		exit(-1);
	}
	if ((outfile = fopen(image_file, "w")) == NULL) {
		fprintf(stderr, "Error: cannot open file %s\n", image_file);
		exit(-1);
	}
	printf("%s: filling %d dimensions to %d bits of precision.\n",
			argv[0], dimensions, precision);

		/* begin the mysterious & incomprehensible algorithm... */
	for (i=0; i<dimensions; i++) {
		bitmask[i] = 1 << (dimensions - i - 1);
		bytemask |= 1 << i;
		glast_coord[i] = 0;
	}

		/* run the mysterious & incomprehensible algorithm... */
	recurse(gcoord, glast_coord, (int) pow(2.0, (double) precision),
			dimensions);

		/* inform user that execution of algorithm is complete */
	fprintf(stderr, "Spewing...");

	for (i=FB_SIZE-1; i>=0; i--)
		fwrite(fb[i], 1, FB_SIZE, outfile);

	fprintf(stderr, " done.");

	exit(0);

} /* main() */


/*
 * recursive routine to call "peano()"
 */
recurse(coord, last_coord, iterations, level)
vector          coord, last_coord;
int             iterations, level;
{
	int             i ;
	static int      n=0, scale, offset;

	if (level > 0)
		for (i=0; i<iterations; i++)
			recurse(coord, last_coord, iterations, level - 1);
	else {
			/* get x,y coord of position n on peano curve */
		peano(coord, n++);
		offset = ((int) pow(2.0, (double) precision));
		scale = FB_SIZE / ((int) pow(2.0, (double) precision));
		offset = scale / 2;
			/* draw line between adjacent coords */
		draw_line(scale * coord[1] + offset,
			FB_SIZE - scale * coord[0] - offset,
			scale * last_coord[1] + offset,
			FB_SIZE - scale * last_coord[0] - offset,
			n);

		/*
		 * x = scale*coord[1]+offset; y =
		 * FB_SIZE-scale*coord[0]-offset; index = n%256;
		 */

		for (i=0; i<precision; i++)
			last_coord[i] = coord[i];
	}

} /* recurse() */


/* 
 * draws horizontal and vertical lines into "fb" with color "index"
 */
draw_line(x1, y1, x2, y2, index)
unsigned        x1, y1, x2, y2, index;
{
	int	tmp, i;

	index = index % 256;
	if (index == 0)
		index = 1;

	if (x1 != x2) {		/* have horizontal line */
		if (x1 > x2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		for (i=x1; i<=x2; i++)
			fb[y1][i] = (unsigned char) index;
	} else {		/* vertical line */
		if (y1 > y2) {
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		for (i=y1; i<=y2; i++)
			fb[i][x1] = (unsigned char) index;
	}
} /* draw_line() */

