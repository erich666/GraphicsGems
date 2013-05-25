/* TEST FILE FOR kc.multi.c, NOT FOR BOOK */



#include <stdio.h>



typedef struct {
    double x, y;
} Point2;



int srandom(int seed);

void MultiJitter(Point2 points[], int m, int n);



main(int argc, char *argv[]) {

    Point2 *points;
    int m, n, i, x, y;
    int **counts, *x_bins, *y_bins;

    if (argc != 4) {
	fprintf(stderr, "Usage: multi-jitter <m> <n> <seed>.\n");
	exit(1);
    }

    m = atoi(argv[1]);
    n = atoi(argv[2]);
    srandom(atoi(argv[3]));

    points = (Point2 *) malloc(m*n*sizeof(Point2));
    counts = (int **) malloc(m*sizeof(int *));
    for (i = 0; i < m; i++)
	counts[i] = (int *) malloc(n*sizeof(int));
    x_bins = (int *) malloc(m*n*sizeof(int));
    y_bins = (int *) malloc(m*n*sizeof(int));

    MultiJitter(points, m, n);

    /*
     * Test jittered condition
     */

    for (x = 0; x < m; x++) {
	for (y = 0; y < n; y++) {
	    counts[x][y] = 0;
	}
    }

    for (i = 0; i < m*n; i++) {
	x = points[i].x / (1.0/m);
	y = points[i].y / (1.0/n);
	counts[x][y]++;
    }

    for (x = 0; x < m; x++) {
	for (y = 0; y < n; y++) {
	    if (counts[x][y] != 1) {
		fprintf(stderr, "Jittered condition not satisfied.\n");
		goto done1;
	    }
	}
    }
    done1:;

    /*
     * Test n-rooks condition
     */

    for (x = 0; x < m*n; x++)
	x_bins[x] = 0;
    for (y = 0; y < m*n; y++)
	y_bins[y] = 0;

    for (i = 0; i < m*n; i++) {
	x = points[i].x / (1.0/(m*n));
	y = points[i].y / (1.0/(m*n));
	x_bins[x]++;
	y_bins[y]++;
    }

    for (x = 0; x < m*n; x++) {
	if (x_bins[x] != 1) {
	    fprintf(stderr, "n-rooks condition not satisfied in x.\n");
	    goto done2;
	}
    }
    done2:;

    for (y = 0; y < m*n; y++) {
	if (y_bins[y] != 1) {
	    fprintf(stderr, "n-rooks condition not satisfied in y.\n");
	    goto done3;
	}
    }
    done3:;

    for (i = 0; i < m*n; i++)
	fprintf(stderr, "(%f, %f)\n", points[i].x, points[i].y);
}
