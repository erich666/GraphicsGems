/*
 * testlen4.c - len4 exerciser
 *
 * % cc -g testlen4.c len4.c -lm -o testlen4
 * % ./testlen4 2 1 1 1 
 * % ./testlen4 60000000 25000000 19000000 16000000
 */

#include <stdio.h>
#include <stdlib.h>	/* for atoi() */
#include <math.h>

main(argc, argv)
    char *argv[];
    {
    long a, b, c, d, l;
    double l2;
    a = atoi(argv[1]);
    b = atoi(argv[2]);
    c = atoi(argv[3]);
    d = atoi(argv[4]);
    l = len4(a,b,c,d);
    l2 = hypot(hypot((double)(a),(double)(b)),
	       hypot((double)(c),(double)(d)));
    printf("(%d,%d,%d,%d) = %d (%g)\n", a, b, c, d, l, l/l2);
    }
