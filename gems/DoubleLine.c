/*
Symmetric Double Step Line Algorithm
by Brian Wyvill
from "Graphics Gems", Academic Press, 1990

user provides "setpixel()" function for output.
*/

#define swap(a,b)           {a^=b; b^=a; a^=b;}
#define absolute(i,j,k)     ( (i-j)*(k = ( (i-j)<0 ? -1 : 1)))
int
symwuline(a1, b1, a2, b2) int a1, b1, a2, b2;
{
	int           dx, dy, incr1, incr2, D, x, y, xend, c, pixels_left;
	int           x1, y1;
	int           sign_x, sign_y, step, reverse, i;

	dx = absolute(a2, a1, sign_x);
	dy = absolute(b2, b1, sign_y);
	/* decide increment sign by the slope sign */
	if (sign_x == sign_y)
		step = 1;
	else
		step = -1;

	if (dy > dx) {		/* chooses axis of greatest movement (make
				 		 * dx) */
		swap(a1, b1);
		swap(a2, b2);
		swap(dx, dy);
		reverse = 1;
	} else
		reverse = 0;
	/* note error check for dx==0 should be included here */
	if (a1 > a2) {		/* start from the smaller coordinate */
		x = a2;
		y = b2;
		x1 = a1;
		y1 = b1;
	} else {
		x = a1;
		y = b1;
		x1 = a2;
		y1 = b2;
	}


	/* Note dx=n implies 0 - n or (dx+1) pixels to be set */
	/* Go round loop dx/4 times then plot last 0,1,2 or 3 pixels */
	/* In fact (dx-1)/4 as 2 pixels are already plotted */
	xend = (dx - 1) / 4;
	pixels_left = (dx - 1) % 4;	/* number of pixels left over at the
					 			 * end */
	plot(x, y, reverse);
	if ( pixels_left < 0 ) return ;	/* plot only one pixel for zero
							* length vectors */
	plot(x1, y1, reverse);	/* plot first two points */
	incr2 = 4 * dy - 2 * dx;
	if (incr2 < 0) {	/* slope less than 1/2 */
		c = 2 * dy;
		incr1 = 2 * c;
		D = incr1 - dx;

		for (i = 0; i < xend; i++) {	/* plotting loop */
			++x;
			--x1;
			if (D < 0) {
                  			/* pattern 1 forwards */
				plot(x, y, reverse);
				plot(++x, y, reverse);
                                /* pattern 1 backwards */
				plot(x1, y1, reverse);
				plot(--x1, y1, reverse);
				D += incr1;
			} else {
				if (D < c) {
					/* pattern 2 forwards */
					plot(x, y, reverse);
					plot(++x, y += step, reverse);
					/* pattern 2 backwards */
					plot(x1, y1, reverse);
					plot(--x1, y1 -= step, reverse);	
				} else {
				        /* pattern 3 forwards */
					plot(x, y += step, reverse);
					plot(++x, y, reverse);
					/* pattern 3 backwards */
					plot(x1, y1 -= step, reverse);
					plot(--x1, y1, reverse);
				}
				D += incr2;
			}
		}		/* end for */

		/* plot last pattern */
		if (pixels_left) {
			if (D < 0) {
				plot(++x, y, reverse);	/* pattern 1 */
				if (pixels_left > 1)
					plot(++x, y, reverse);
				if (pixels_left > 2)
					plot(--x1, y1, reverse);
			} else {
				if (D < c) {
					plot(++x, y, reverse);	/* pattern 2  */
					if (pixels_left > 1)
						plot(++x, y += step, reverse);
					if (pixels_left > 2)
						plot(--x1, y1, reverse);
				} else {
				  /* pattern 3 */
					plot(++x, y += step, reverse);
					if (pixels_left > 1)
						plot(++x, y, reverse);
					if (pixels_left > 2)
						plot(--x1, y1 -= step, reverse);
				}
			}
		}		/* end if pixels_left */
	}
	/* end slope < 1/2 */
	else {			/* slope greater than 1/2 */
		c = 2 * (dy - dx);
		incr1 = 2 * c;
		D = incr1 + dx;
		for (i = 0; i < xend; i++) {
			++x;
			--x1;
			if (D > 0) {
			  /* pattern 4 forwards */
				plot(x, y += step, reverse);
				plot(++x, y += step, reverse);
			  /* pattern 4 backwards */
				plot(x1, y1 -= step, reverse);
				plot(--x1, y1 -= step, reverse);
				D += incr1;
			} else {
				if (D < c) {
				  /* pattern 2 forwards */
					plot(x, y, reverse);
					plot(++x, y += step, reverse);

 				  /* pattern 2 backwards */
					plot(x1, y1, reverse);
					plot(--x1, y1 -= step, reverse);
				} else {
				  /* pattern 3 forwards */
					plot(x, y += step, reverse);
					plot(++x, y, reverse);
				  /* pattern 3 backwards */
					plot(x1, y1 -= step, reverse);
					plot(--x1, y1, reverse);
				}
				D += incr2;
			}
		}		/* end for */
		/* plot last pattern */
		if (pixels_left) {
			if (D > 0) {
				plot(++x, y += step, reverse);	/* pattern 4 */
				if (pixels_left > 1)
					plot(++x, y += step, reverse);
				if (pixels_left > 2)
					plot(--x1, y1 -= step, reverse);
			} else {
				if (D < c) {
					plot(++x, y, reverse);	/* pattern 2  */
					if (pixels_left > 1)
						plot(++x, y += step, reverse);
					if (pixels_left > 2)
						plot(--x1, y1, reverse);
				} else {
				  /* pattern 3 */
					plot(++x, y += step, reverse);
					if (pixels_left > 1)
						plot(++x, y, reverse);
					if (pixels_left > 2) {
						if (D > c) /* step 3 */
						   plot(--x1, y1 -= step, reverse);
						else /* step 2 */
							plot(--x1, y1, reverse);
                         		}
				}
			}
		}
	}
}
/* non-zero flag indicates the pixels needing swap back. */
plot(x, y, flag) int x, y, flag;
{
	if (flag)
		setpixel(y, x);
	else
		setpixel(x, y);
}

