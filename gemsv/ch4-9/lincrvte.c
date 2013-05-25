/*** lincrvtest.c ***/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "lincrv.h"

#define TRUE 1
#define FALSE 0
#define BIG (1.0e12)

static Vect work[4];
static Vect ctlPts[] = { {0,0}, {1,1}, {1,0}, {0,0}, };
static float *knots;
static float bezKts[] = {0, 0, 0, 1, 1, 1, BIG};
static float lagKts[] = {0.00, 0.25, 0.75, 1.00, BIG};
static float catKts[] = {-1, 0, 1, 2, BIG};
static float bspKts[] = {-2, -1, 0, 1, 2, 3, BIG};
static int m = MAXDIM;
static int n = 3;
static int Cn = 1;
static Bool interp = FALSE;
static Vect val = {0.84375, 0.0};
static float t = 0;
static int eh = 0;

enum Flavor{PLY, LAG, BEZ, CAT, BSP, NFLAVORS};
char fnames[][4] = {"PLY", "LAG", "BEZ", "CAT", "BSP"};

void main(void)
{
    int i;
    int flavor = PLY;
    
    for (flavor=0; flavor<NFLAVORS; flavor++) {
        switch (flavor) {
        case PLY:   knots = lagKts; interp = TRUE;  Cn = 0; break;
        case LAG:   knots = lagKts; interp = TRUE;  Cn = 2; break;
        case BEZ:   knots = bezKts; interp = FALSE; Cn = 2; break;
        case CAT:   knots = catKts; interp = TRUE;  Cn = 1; break;
        case BSP:   knots = bspKts; interp = FALSE; Cn = 2; break;
        default:    knots = bspKts; interp = FALSE; Cn = 0; break;
        }
        printf("Flavor %s: interp=%d, Cn=%d\n", fnames[flavor],interp,Cn);
        for (t=0.0; t<=1.0; t+=0.125) {
            eh = DialASpline(t, knots, ctlPts, m, n, work, Cn, interp, val);
            printf("(%6.3f) ", t);
            for (i=0; i<MAXDIM; i++) printf("%9.6f ",val[i]); printf("\n");
        }
    }
}
