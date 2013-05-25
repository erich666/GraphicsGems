/*
 * ANSI C code from the article
 * "Centroid of a Polygon"
 * by Gerard Bashein and Paul R. Detmer,
	(gb@locke.hs.washington.edu, pdetmer@u.washington.edu)
 * in "Graphics Gems IV", Academic Press, 1994
 */

/*********************************************************************
polyCentroid: Calculates the centroid (xCentroid, yCentroid) and area
of a polygon, given its vertices (x[0], y[0]) ... (x[n-1], y[n-1]). It
is assumed that the contour is closed, i.e., that the vertex following
(x[n-1], y[n-1]) is (x[0], y[0]).  The algebraic sign of the area is
positive for counterclockwise ordering of vertices in x-y plane;
otherwise negative.

Returned values:  0 for normal execution;  1 if the polygon is
degenerate (number of vertices < 3);  and 2 if area = 0 (and the
centroid is undefined).
**********************************************************************/
int polyCentroid(double x[], double y[], int n,
		 double *xCentroid, double *yCentroid, double *area)
     {
     register int i, j;
     double ai, atmp = 0, xtmp = 0, ytmp = 0;
     if (n < 3) return 1;
     for (i = n-1, j = 0; j < n; i = j, j++)
	  {
	  ai = x[i] * y[j] - x[j] * y[i];
	  atmp += ai;
	  xtmp += (x[j] + x[i]) * ai;
	  ytmp += (y[j] + y[i]) * ai;
	  }
     *area = atmp / 2;
     if (atmp != 0)
	  {
	  *xCentroid =	xtmp / (3 * atmp);
	  *yCentroid =	ytmp / (3 * atmp);
	  return 0;
	  }
     return 2;
     }
