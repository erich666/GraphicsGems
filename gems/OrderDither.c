/*
Ordered Dithering
by Stephen Hawley
from "Graphics Gems", Academic Press, 1990
*/

/* Program to generate dithering matrices.
 * written by Jim Blandy, Oberlin College, jimb@occs.oberlin.edu
 * Gifted to, documented and revised by Stephen Hawley,
 * sdh@flash.bellcore.com
 * 
 * Generates a dithering matrix from the command line arguments.
 * The first argument, size, determines the dimensions of the 
 * matrix: 2^size by 2^size
 * The optional range argument is the range of values to be 
 * dithered over. By default, it is (2^size)^2, or simply the
 * total number of elements in the matrix.
 * The final output is suitable for inclusion in a C program.
 * A typical dithering function is something like this:
 * extern int dm[], size;
 *
 * int
 * dither(x,y, level)
 * register int x,y, level;
 * {
 *	return(level > dm[(x % size) + size * (y % size)];
 * }
 */

main(argc, argv)
int argc;
char **argv;
{
	register int size, range;

	if (argc >= 2) size = atoi(argv[1]);
	else size = 2;

	if (argc == 3) range = atoi(argv[2]);
	else range = (1 << size) * (1 << size);

	printdither (size, range);
}


printdither (size, range)
register int size, range;
{
	register int l = (1 << size), i;
     /*
      * print a dithering matrix.
      * l is the length on a side.
      */
	range = range / (l * l);
	puts("int dm[] = {");
	for (i=0; i < l*l; i++) {
		if (i % l == 0) /* tab in 4 spaces per row */
			printf("    ");
		/* print the dither value for this location
 		 * scaled to the given range
		 */
		printf("%4d", range * dithervalue(i / l, i % l, size));

		/* commas after all but the last */
		if (i + 1 < l * l)
			putchar(',');
		/* newline at the end of the row */
		if ((i + 1) % l == 0)
			putchar('\n');
	}
	puts("\n}; ");
}
dithervalue(x, y, size)
register int x, y, size;
{
	register int d;
	/*
	 * calculate the dither value at a particular
	 * (x, y) over the size of the matrix.
	 */
	d=0;
	while (size-->0)	{
		/* Think of d as the density. At every iteration,
		 * d is shifted left one and a new bit is put in the
		 * low bit based on x and y. If x is odd and y is even,
		 * or x is even and y is odd, a bit is put in. This
		 * generates the checkerboard seen in dithering.
		 * This quantity is shifted left again and the low bit of
		 * y is added in.
		 * This whole thing interleaves a checkerboard bit pattern
		 * and y's bits, which is the value you want.
		 */
		d = (d <<1 | (x&1 ^ y&1))<<1 | y&1;
		x >>= 1;
		y >>= 1;
	}
	return(d);
}

