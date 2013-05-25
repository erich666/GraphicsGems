/* Copyright (c) 1991 Regents of the University of California */

#ifndef lint
static char SCCSid[] = "@(#)ra_pr24.c 1.8 8/15/91 LBL";
#endif

/*
 *  program to convert between RADIANCE and 24-bit rasterfiles.
 */

#include  <stdio.h>

#include  "rasterfile.h"

#include  "color.h"

extern double  atof(), pow();

double	gamma = 2.0;			/* gamma correction */

int  bradj = 0;				/* brightness adjustment */

char  *progname;

int  xmax, ymax;


main(argc, argv)
int  argc;
char  *argv[];
{
	struct rasterfile  head;
	int  reverse = 0;
	int  i;
	
	progname = argv[0];

	for (i = 1; i < argc; i++)
		if (argv[i][0] == '-')
			switch (argv[i][1]) {
			case 'g':
				gamma = atof(argv[++i]);
				break;
			case 'e':
				if (argv[i+1][0] != '+' && argv[i+1][0] != '-')
					goto userr;
				bradj = atoi(argv[++i]);
				break;
			case 'r':
				reverse = !reverse;
				break;
			default:
				goto userr;
			}
		else
			break;

	if (i < argc-2)
		goto userr;
	if (i <= argc-1 && freopen(argv[i], "r", stdin) == NULL) {
		fprintf(stderr, "%s: can't open input \"%s\"\n",
				progname, argv[i]);
		exit(1);
	}
	if (i == argc-2 && freopen(argv[i+1], "w", stdout) == NULL) {
		fprintf(stderr, "can't open output \"%s\"\n",
				progname, argv[i+1]);
		exit(1);
	}
	setcolrgam(gamma);
	if (reverse) {
					/* get header */
		if (fread((char *)&head, sizeof(head), 1, stdin) != 1)
			quiterr("missing header");
		if (head.ras_magic != RAS_MAGIC)
			quiterr("bad raster format");
		xmax = head.ras_width;
		ymax = head.ras_height;
		if ((head.ras_type != RT_STANDARD
					&& head.ras_type != RT_FORMAT_RGB)
				|| head.ras_maptype != RMT_NONE
				|| head.ras_depth != 24)
			quiterr("incompatible format");
					/* put header */
		printargs(i, argv, stdout);
		fputformat(COLRFMT, stdout);
		putchar('\n');
		fputresolu(YMAJOR|YDECR, xmax, ymax, stdout);
					/* convert file */
		pr2ra(head.ras_type);
	} else {
					/* get header info. */
		if (checkheader(stdin, COLRFMT, NULL) < 0 ||
			fgetresolu(&xmax, &ymax, stdin) != (YMAJOR|YDECR))
			quiterr("bad picture format");
					/* write rasterfile header */
		head.ras_magic = RAS_MAGIC;
		head.ras_width = xmax;
		head.ras_height = ymax;
		head.ras_depth = 24;
		head.ras_length = xmax*ymax*3;
		head.ras_type = RT_STANDARD;
		head.ras_maptype = RMT_NONE;
		head.ras_maplength = 0;
		fwrite((char *)&head, sizeof(head), 1, stdout);
					/* convert file */
		ra2pr();
	}
	exit(0);
userr:
	fprintf(stderr, "Usage: %s [-r][-g gamma][-e +/-stops] [input [output]]\n",
			progname);
	exit(1);
}


quiterr(err)		/* print message and exit */
char  *err;
{
	if (err != NULL) {
		fprintf(stderr, "%s: %s\n", progname, err);
		exit(1);
	}
	exit(0);
}


pr2ra(rf)		/* convert 24-bit scanlines to Radiance picture */
int	rf;
{
	COLR	*scanout;
	register int	x;
	int	y;
						/* allocate scanline */
	scanout = (COLR *)malloc(xmax*sizeof(COLR));
	if (scanout == NULL)
		quiterr("out of memory in pr2ra");
						/* convert image */
	for (y = ymax-1; y >= 0; y--) {
		for (x = 0; x < xmax; x++)
			if (rf == RT_FORMAT_RGB) {
				scanout[x][RED] = getc(stdin);
				scanout[x][GRN] = getc(stdin);
				scanout[x][BLU] = getc(stdin);
			} else {
				scanout[x][BLU] = getc(stdin);
				scanout[x][GRN] = getc(stdin);
				scanout[x][RED] = getc(stdin);
			}
		if (feof(stdin) || ferror(stdin))
			quiterr("error reading rasterfile");
		gambs_colrs(scanout, xmax);
		if (bradj)
			shiftcolrs(scanout, xmax, bradj);
		if (fwritecolrs(scanout, xmax, stdout) < 0)
			quiterr("error writing Radiance picture");
	}
						/* free scanline */
	free((char *)scanout);
}


ra2pr()			/* convert Radiance scanlines to 24-bit rasterfile */
{
	COLR	*scanin;
	register int	x;
	int	y;
						/* allocate scanline */
	scanin = (COLR *)malloc(xmax*sizeof(COLR));
	if (scanin == NULL)
		quiterr("out of memory in ra2pr");
						/* convert image */
	for (y = ymax-1; y >= 0; y--) {
		if (freadcolrs(scanin, xmax, stdin) < 0)
			quiterr("error reading Radiance picture");
		if (bradj)
			shiftcolrs(scanin, xmax, bradj);
		colrs_gambs(scanin, xmax);
		for (x = 0; x < xmax; x++) {
			putc(scanin[x][BLU], stdout);
			putc(scanin[x][GRN], stdout);
			putc(scanin[x][RED], stdout);
		}
		if (ferror(stdout))
			quiterr("error writing rasterfile");
	}
						/* free scanline */
	free((char *)scanin);
}
