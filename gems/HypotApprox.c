/*
A Fast Approximation to the Hypotenuse
by Alan Paeth
from "Graphics Gems", Academic Press, 1990
*/

int idist(x1, y1, x2, y2)
 	int x1, y1, x2, y2;
	{
/*
 * gives approximate distance from (x1,y1) to (x2,y2)
 * with only overestimations, and then never by more
 * than (9/8) + one bit uncertainty.
 */
	if ((x2 -= x1) < 0) x2 = -x2;
	if ((y2 -= y1) < 0) y2 = -y2;
	return (x2 + y2 - (((x2>y2) ? y2 : x2) >> 1) );
	}

int PntOnCirc(xp, yp, xc, yc, r)
    int xp, yp, xc, yc, r;
    {
/* returns true IFF a test point (xp, yp) is to within a
 * pixel of the circle of center (xc, yc) and radius r.
 * "d" is an approximate length to circle's center, with
 * 1.0*r < dist < 1.12*r < (9/8)*r used for coarse testing.
 * The 9/8 ratio suggests the code: (x)<<3 and ((x)<<3)-(x).
 * Variables xp, yp, r and d should be of 32-bit precision.
 *
 * Note: (9/8) forms a very tight, proper inner bound but
 * must be slackened by one pixel for the outside test (#2)
 * to account for the -1/2 pixel absolute error introduced
 * when "idist" halves an odd integer; else rough clipping
 * will trim occasional points on the circle's perimeter.
 */
    int d = idist(xp, yp, xc, yc);
    if (  r >      d)   return(0);    /* far-in  */
    if (9*r < 8*(d-1))  return(0);    /* far-out */
/* full test: r < hypot(xp-xc,yp-yc) < r+1 */
    xp -= xc;
    yp -= yc;
    d = xp*xp + yp*yp;
    if (d < r*r) return(0);          /* near-in */
    r += 1;
    if (d > r*r) return(0);          /* near-out */
    return(1);                       /* WITHIN */
    }
