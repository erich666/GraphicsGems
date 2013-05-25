/* mapply.c */
/*
 * maps an 8-bit image into a 24-bit image,
 * using color map in a user-specified file
 *
 * color maps may be generated using ran_ramp
 */

/* Ken Musgrave */
/* June 1986 */



#include <stdio.h>
#include <math.h>
#include "types.h"

char           *image_file = "img";

int             r, g, b;
unsigned char   eight_bit[FB_SIZE], twenty_four_bit[FB_SIZE][3], cmap[256][3];

FILE           *imfile, *cmapfile, *outfile, *fopen();


main(argc, argv)
int             argc;
char           *argv[];
{
	int             i, x, y;

		/* get command-line args */
	if (argc != 3) {
		fprintf(stderr, "usage: %s infile cmap_file\n", argv[0]);
		exit(0);
	}
	if ((imfile = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Error: cannot open file %s\n", argv[1]);
		exit(-1);
	}
	if ((cmapfile = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "Error: cannot open file %s\n", argv[2]);
		exit(-1);
	}
	if ((outfile = fopen(image_file, "w")) == NULL) {
		fprintf(stderr, "Error: cannot open file %s\n", image_file);
		exit(-1);
	}
	printf("%s: mapping \"%s\" to \"%s\" using map in \"%s\"\n", 
			argv[0], argv[1], image_file, argv[2]);

	fscanf(cmapfile, "%d %d\n", &r, &g);
	for (i = 0; i < 256; i++) {
		fscanf(cmapfile, "%d %d %d\n", &r, &g, &b);
		cmap[i][0] = (unsigned char) r;
		cmap[i][1] = (unsigned char) g;
		cmap[i][2] = (unsigned char) b;
	}
	cmap[0][0] = cmap[1][0];
	cmap[0][1] = cmap[1][1];
	cmap[0][2] = cmap[1][2];
	cmap[255][0] = cmap[254][0];
	cmap[255][1] = cmap[254][1];
	cmap[255][2] = cmap[254][2];

	for (y = 0; y < FB_SIZE; y++) {
		fread(eight_bit, 1, FB_SIZE, imfile);
		for (x = 0; x < FB_SIZE; x++) {
			if (eight_bit[x]) {
				twenty_four_bit[x][0] = cmap[eight_bit[x]][0];
				twenty_four_bit[x][1] = cmap[eight_bit[x]][1];
				twenty_four_bit[x][2] = cmap[eight_bit[x]][2];
			} else {
				twenty_four_bit[x][0] = 0;
				twenty_four_bit[x][1] = 0;
				twenty_four_bit[x][2] = 0;
			}
		}
		fwrite(twenty_four_bit, 3, FB_SIZE, outfile);
	}

} /* main() */

