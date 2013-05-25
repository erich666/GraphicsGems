/*****************************************************************
Plot a series of points along a PI/2-radian arc of an ellipse.
The arc is specified in terms of a control polygon (a triangle)
with vertices P, Q and K.  The arc begins at P, ends at Q, and is
completely contained within the control polygon.  The draw_point
function plots a single pixel at display coordinates (x,y).

Entry:
  xP, yP, xQ, yQ, xK, yK -- coordinates of P, Q and K.  These
    are 32-bit fixed-point values with 16 bits of fraction.
  m -- nonnegative integer that controls spacing between points.
    The angular increment between points is 1/2^m radians.
Exit:
  The number of points plotted is 1 + floor((PI/2)*2^m).
*****************************************************************/

#define PIV2  102944     /* fixed point PI/2 */
#define TWOPI 411775     /* fixed point 2*PI */
#define HALF  32768      /* fixed point 1/2 */
typedef long FIX;        /* 32-bit fixed point, 16-bit fraction */

qtr_elips(xP, yP, xQ, yQ, xK, yK, m)
FIX xP, yP, xQ, yQ, xK, yK;
int m;
{
    int i, x, y;
    FIX vx, ux, vy, uy, w, xJ, yJ;

    vx = xK - xQ;                 /* displacements from center */
    ux = xK - xP;
    vy = yK - yQ;
    uy = yK - yP;
    xJ = xP - vx + HALF;          /* center of ellipse J */
    yJ = yP - vy + HALF;
    ux -= (w = ux >> (2*m + 3));  /* cancel 2nd-order error */
    ux -= (w >>= (2*m + 4));      /* cancel 4th-order error */
    ux -= w >> (2*m + 3);         /* cancel 6th-order error */
    ux += vx >> (m + 1);          /* cancel 1st-order error */
    uy -= (w = uy >> (2*m + 3));  /* cancel 2nd-order error */
    uy -= (w >>= (2*m + 4));      /* cancel 4th-order error */
    uy -= w >> (2*m + 3);         /* cancel 6th-order error */
    uy += vy >> (m + 1);          /* cancel 1st-order error */
    for (i = (PIV2 << m) >> 16; i >= 0; --i) {
        x = (xJ + vx) >> 16;
        y = (yJ + vy) >> 16;
        draw_point(x, y);
        ux -= vx >> m;
        vx += ux >> m;
        uy -= vy >> m;
        vy += uy >> m;
    }
}
