/* copyright Ken Musgrave */
/* March 1985 */

/* ran_ramp.c */

/***********************************************************************
*
*   Usage: ran_ramp [-g] [-z]
*
*   Performs random continuous changes to the color map of a frame buffer.
*
*   The idea is to use three DDA's with endpoint input from a random
* number generator.  The three DDA's generate random sawtooth waves
* of values for red, green, and blue.  These waves of values are pushed
* through the lookup table from entry 254 down to entry 0.
*
*   NOTE:  Entry 255 remains black, as this was designed for animating
* the Mandelbrot set, whose center should remain black.
*
***********************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#define TRUE  1
#define FALSE 0

#define MAXENTRY	256
#define MAXINDEX	255
#define DELAY		8	/* kludge to modulate speed of animation */
				/* you may want/need to change this value */

/*
 * pseudo-random number generator; period 65536; requires seed between 0 and
 * 65535; returns random numbers between 0 and 65536.
 */
#define MULTIPLIER 25173
#define INCREMENT  13849
#define MODULUS    65535
#define RANDOM(x)  (MULTIPLIER * x + INCREMENT) & MODULUS

int             quit = FALSE;	/* signal variables */
int             stop = FALSE;	/* used in trapping <ctrl-C> and <ctrl-Z> */

int             seed, maxsteps; /* user input variables */

				/* arrays for color lookup table values */
unsigned char   r[MAXENTRY], g[MAXENTRY], b[MAXENTRY];


main(argc, argv)
int             argc;
char           *argv[];
{

	register int    i;
	int             reply, delay;

	int             inter();	 /* signal functions, see below */
	int             suspend();

	signal(SIGINT, inter);		 /* traps <ctrl-C> */
	signal(SIGTSTP, suspend);	 /* traps <ctrl-Z> */

	printf("Please enter seed: ");
	scanf("%d", &seed);

					/* "(10-100)" is just a suggestion... */
	printf("\nPlease enter maximum length of color ramp:  (10-100)");
	scanf("%d", &maxsteps);
	printf("\n");

	fb_init();			/* generic frame buffer init routine */

			/* if specified, set initial map option */
	if (argc == 2) {
			/* "-g" option for gray scale */
		if (argv[1][0] == '-' && argv[1][1] == 'g') {
			for (i=0; i<MAXINDEX; i++)
				r[i] = g[i] = b[i] = i;
			r[MAXINDEX] = g[MAXINDEX] = b[MAXINDEX] = 0;
		}
			/* "-z" option for zebra scale */
		else if (argv[1][0] == '-' && argv[1][1] == 'z') {
			for (i=0; i<MAXINDEX; i+=4) {
				r[i]=r[i+1]=g[i]=g[i+1]=b[i]=b[i+1]=i * 4 / 5;
				r[i+2]=r[i+3]=g[i+2]=g[i+3]=b[i+2]=b[i+3]=
					i * 4/5 + 51;
			}
			r[MAXINDEX] = g[MAXINDEX] = b[MAXINDEX] = 0;
		}
	} else {
			/* initialize the color map to black */
		for (i=0; i<MAXINDEX; i++)
			r[i] = g[i] = b[i] = 0;
	}

			/* generic routine to write frame buffer color map */
	fb_setmap(r, g, b);

			/* loop until <ctrl-C> is trapped */
	while (!quit) {
			/* trapped <crtl-Z> */
		if (stop) {
			fb_done();	/* release frame buffer */
			printf("\nSave lookup table? (y/n) ");
			while (isspace(reply = getchar()));
			if (reply == 'y')
				save_lut();
					/* returns when job running again */
			kill(getpid(), SIGSTOP);
			stop = FALSE;
			fb_init();	/* get back in action */
		}

		/*
		 * main animation loop:
		 *
		 * move each entry in color map arrays down one place
		 */
		for (i=0; i<(MAXINDEX-1); i++) {
			r[i] = r[i+1];
			g[i] = g[i+1];
			b[i] = b[i+1];
		}
		/* 
		 * get new high color map entries 
		 */
		r[MAXINDEX-1] = dda_red();
		g[MAXINDEX-1] = dda_green();
		b[MAXINDEX-1] = dda_blue();
		/*
		 * send new color maps to the frame buffer
		 * NOTE: we may do this several times (the value of
		 * "DELAY") to slow down the animation
		 */
		for (delay=0; delay<DELAY; delay++)
			fb_setmap(0, MAXINDEX, r, g, b);

	}

	fb_done();	/* release the frame buffer */

} /* main() */


/*
 * function for trapping <ctrl-C>
 * NOTE: the only reason to use this is so that
 * we can call fb_done() before exiting - this 
 * may or may not be necessary
 */
inter()
{
	quit = TRUE;
}

/*
 * function for trapping <ctrl-Z>
 * NOTE: this is used so that the user may stop the
 * animation at any time and dump the color map to 
 * a file
 */
suspend()
{
	stop = TRUE;
}


/*
 * produces linear ramps in intensity for the red portion of the color
 * lookup table
 */

dda_red()
{

	register int    temp;
	static float	ry1, ry2=0., rinc, r_xsteps=0.;
	static int 	r_xcount=0;

	/* if at end of ramp... */
	if (r_xcount >= (int) r_xsteps) {
		/*
		 * make the end of last ramp the beginning of next ramp
		 */
		ry1 = ry2;
			/* define end of next ramp */
		seed = RANDOM(seed);
			/* assign a new (scaled) end point */
		ry2 = MAXINDEX * (seed / 65535.0);
		seed = RANDOM(seed);
			/* get a new ramp length */
		r_xsteps = (maxsteps * (seed / 65535.0));
			/* find the intensity increment per step */
		if (r_xsteps != 0)
			rinc = (ry2 - ry1) / r_xsteps;
		else
			rinc = 0;
		r_xcount = 0;
	}
	temp = (int) ry1;
	ry1 += rinc;
	r_xcount++;
	return temp;
} /* dda_red() */

/*
 * produces linear ramps in intensity for the green portion of the color
 * lookup table
 */

dda_green()
{

	register int    temp;
	static float	gy1, gy2=0., ginc, g_xsteps=0.;
	static int 	g_xcount=0;

	if (g_xcount >= (int) g_xsteps) {
		/*
		 * make the end of last ramp the beginning of next ramp
		 */
		gy1 = gy2;
			/* define end of next ramp */
		seed = RANDOM(seed);
		gy2 = MAXINDEX * (seed / 65535.0);
		seed = RANDOM(seed);
		g_xsteps = (maxsteps * (seed / 65535.0));
			/* find the intensity increment per step */
		if (g_xsteps != 0)
			ginc = (gy2 - gy1) / g_xsteps;
		else
			ginc = 0;
		g_xcount = 0;
	}
	temp = (int) gy1;
	gy1 += ginc;
	g_xcount++;
	return temp;
}  /* dda_green() */

/*
 * produces linear ramps in intensity for the blue portion of the color
 * lookup table
 */

dda_blue()
{

	register int    temp;
	static float	by1, by2=0., binc, b_xsteps=0.;
	static int 	b_xcount=0;

	if (b_xcount >= (int) b_xsteps) {
		/*
		 * make the end of last ramp the beginning of next ramp
		 */
		by1 = by2;
			/* define end of next ramp */
		seed = RANDOM(seed);
		by2 = MAXINDEX * (seed / 65535.0);
		seed = RANDOM(seed);
		b_xsteps = (maxsteps * (seed / 65535.0));
			/* find the intensity increment per step */
		if (b_xsteps != 0)
			binc = (by2 - by1) / b_xsteps;
		else
			binc = 0;
		b_xcount = 0;
	}
	temp = (int) by1;
	by1 += binc;
	b_xcount++;
	return temp;
}  /* dda_blue() */


/* 
 * save the lookup table to a file
 */

save_lut()
{
	FILE           *fp, *fopen();
	char            filename[40];
	int             i;

	getchar();		/* read leading newline char */
	printf("Enter filename for lookup table:  ");
	gets(filename);
	fp = fopen(filename, "w");

	for (i=0; i<MAXINDEX; i++)
		fprintf(fp, "%3d %3d %3d\n", r[i], g[i], b[i]);

	fclose(fp);

} /* save_lut() */







