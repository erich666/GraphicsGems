/*
 * Recording Animation in Binary Order for Progressive Temporal Refinement
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * binrec.c: demonstrate binary recording order
 *
 * Paul Heckbert	Jan 90
 */

#include <stdio.h>

main(ac, av)
int ac;
char **av;
{
    int nframes, i, start_frame, repeat_count;
    if (ac!=2) {
	fprintf(stderr, "Usage: binrec <nframes>\n");
	exit(1);
    }
    nframes = atoi(av[1]);

    printf("step startframe repeatcount\n");
    for (i=0; i<nframes; i++) {
	inside_out(nframes, i, &start_frame, &repeat_count);
	printf(" %2d     %2d          %2d\n", i, start_frame, repeat_count);
    }
}

/*
 * inside_out: turn a number "inside-out": a generalization of bit-reversal.
 * For n = power of two, this is equivalent to bit-reversal.
 *
 * Turn the number a inside-out, yielding b.  If 0<=a<n then 0<=b<n.
 * Also return r = min(n-b, largest power of 2 dividing b)
 */

inside_out(n, a, b, r)
int n, a, *b, *r;
{
    int k, m;

    *r = m = n;
    for (*b=0, k=1; k<n; k<<=1)
	if (a<<1>=m) {
	    if (*b==0) *r = k;
	    *b += k;
	    a -= (m+1)>>1;
	    m >>= 1;
	}
	else m = (m+1)>>1;
    if (*r>n-*b) *r = n-*b;
}
