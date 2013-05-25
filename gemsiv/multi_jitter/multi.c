#include <math.h>


#define RAN_DOUBLE(l, h)    (((double) random()/0x80000000U)*((h) - (l)) + (l))
#define RAN_INT(l, h)	    ((int) (RAN_DOUBLE(0, (h)-(l)+1) + (l)))


typedef struct {
    double x, y;
} Point2;


unsigned long random();	    /* expected to return a random int in [0, 2^31-1] */


/*
 * MultiJitter() takes an array of Point2's and the dimension, and fills the
 * the array with the generated sample points.
 *
 *    p[] must have length m*n.
 *    m is the number of columns of cells.
 *    n is the number of rows of cells.
 */
void
MultiJitter(Point2 p[], int m, int n) {

    double subcell_width;
    int i, j;

    subcell_width = 1.0/(m*n);

    /* Initialize points to the "canonical" multi-jittered pattern. */
    for (i = 0; i < m; i++) {
	for (j = 0; j < n; j++) {
	    p[i*n + j].x = i*n*subcell_width + j*subcell_width
	     + RAN_DOUBLE(0, subcell_width);
	    p[i*n + j].y = j*m*subcell_width + i*subcell_width
	     + RAN_DOUBLE(0, subcell_width);
	}
    }

    /* Shuffle x coordinates within each column of cells. */
    for (i = 0; i < m; i++) {
	for (j = 0; j < n; j++) {

	    double t;
	    int k;

	    k = RAN_INT(j, n - 1);
	    t = p[i*n + j].x;
	    p[i*n + j].x = p[i*n + k].x;
	    p[i*n + k].x = t;
	}
    }

    /* Shuffle y coordinates within each row of cells. */
    for (i = 0; i < n; i++) {
	for (j = 0; j < m; j++) {

	    double t;
	    int k;

	    k = RAN_INT(j, m - 1);
	    t = p[j*n + i].y;
	    p[j*n + i].y = p[k*n + i].y;
	    p[k*n + i].y = t;
	}
    }
}
