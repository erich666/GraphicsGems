#include	<stdio.h>


#define		OFFS				 4		/*	first & last segment min size	*/
#define		SMAX				20		/*	object min half size			*/

#define		DIRECT				0
#define		INVERSE				1

#define		LEFT				0x00
#define		RIGHT				0x01
#define		BOTTOM				0x02
#define		TOP					0x03

#define		EQSIGN(x1, x2, x3, x4)		( ( x2 - x1 ) * ( x3 - x4 ) > 0 )

#define		CODE(x, y, w, h, px, py, c)	{				\
				c = 0x00;								\
				if ( px < x ) c = 0x01;					\
				else if ( px >= x + w ) c = 0x02;		\
				if ( py < y ) c |= 0x04;				\
				else if ( py >= y + h ) c |= 0x08;		\
			}


typedef	struct	{
	int	direction;
	int	x, y;
}	Pad;

typedef	struct	{
	int	x, y, w, h;
}	Obj;


/*****************************************************************************
Computes & corrects the connection path from an object Out pad to an object In
pad.

Entry:
	startObj - the object to start the connection from
	endObj   - the object to which the connection ends
	outPad   - the Out pad to be connected
	inPad    - the In pad to be connected
Exit:
	nump - the number of connection points
	pts  - the connection point list
*****************************************************************************/

ComputeCon ( startObj, endObj, outPad, inPad, nump, pts )
	Obj	*startObj;
	Obj	*endObj;
	Pad	*outPad;
	Pad *inPad;
	int	nump;
	int	pts[][2];
{
	nump = Connect(outPad->x, outPad->y, outPad->direction, 
				   inPad->x, inPad->y, inPad->direction, pts);
	Correct(startObj->x, startObj->y, startObj->w, startObj->h, nump, pts);
	Correct(endObj->x, endObj->y, endObj->w, endObj->h, nump, pts);
}


/*****************************************************************************
Computes the connection path from an object Out pad to an object In pad using 
the orthogonal path algorithm.

Entry:
	xo   - Out pad x coordinate
	yo   - Out pad y coordinate
	diro - Out pad direction
	xi   - In pad x coordinate
	yi   - In pad y coordinate
	diri - In pad direction
Exit:
	p - the connection point list
Return:
	the number of connection points
*****************************************************************************/

int	Connect ( xo, yo, diro, xi, yi, diri, p )
	int	xo, yo, diro;
	int	xi, yi, diri;
	int	p[][2];
{
	static	int	directs[4][4] = {
					{ TOP,    BOTTOM, LEFT,   RIGHT  }, 
					{ BOTTOM, TOP,    RIGHT,  LEFT   }, 
					{ RIGHT,  LEFT,   TOP,    BOTTOM }, 
					{ LEFT,   RIGHT,  BOTTOM, TOP    }
				};
	int	i, np, txi, tyi;

	Transform(DIRECT, diro, xo, yo, xi, yi, &txi, &tyi);
	switch ( directs[diro][diri] ) {			/* In pad direction	*/
		case LEFT:
			if ( tyi > 0 ) {					/*	T	*/
				if ( txi > 0 ) {				/*	34	*/
					Shape3P(txi, tyi, p);
					np = 3;
				}
				else {							/*	12	*/
					Shape5PB(txi, tyi, -1, p);
					np = 5;
				}
			}
			else {								/*	B	*/
				Shape5PB(txi, tyi, -1, p);
				np = 5;
			}
			break;
		case RIGHT:
			if ( tyi > 0 ) {					/*	T	*/
				if ( txi > 0 ) {				/*	34	*/
					Shape5PB(txi, tyi, 1, p);
					np = 5;
				}
				else {							/*	12	*/
					Shape3P(txi, tyi, p);
					np = 3;
				}
			}
			else {								/*	B	*/
				Shape5PB(txi, tyi, 1, p);
				np = 5;
			}
			break;
		case BOTTOM:
			if ( tyi > 0 ) {					/*	T	*/
				Shape4PD(txi, tyi, p);
				np = 4;
			}
			else {								/*	B	*/
				Shape6P(txi, tyi, -1, p);
				np = 6;
			}
			break;
		case TOP:
			if ( txi < - SMAX || txi > SMAX ) {	/*	14	*/
				Shape4PU(txi, tyi, p);
				np = 4;
			}
			else {								/*	23	*/
				Shape6P(txi, tyi, 1, p);
				np = 6;
			}
			break;
	}
	for ( i = 0 ; i < np ; i++ ) 
		Transform(INVERSE, diro, xo, yo, p[i][0], p[i][1], &p[i][0], &p[i][1]);
	return (np);
}


/*****************************************************************************
Translates & rotates a point according to the Out pad direction.

Entry:
	mode - transformation mode (DIRECT or INVERSE)
	dout - Out pad direction
	xout - Out pad x coordinate
	yout - Out pad y coordinate
	x, y - point to be transformed
Exit:
	tx, ty - transformed point
*****************************************************************************/

Transform ( mode, dout, xout, yout, x, y, tx, ty )
	int	mode;
	int	dout, xout, yout;
	int	x, y;
	int	*tx, *ty;
{
	int	ox, oy;

	ox = x;
	oy = y;
	switch ( mode ) {
		case DIRECT:
			switch ( dout ) {
				case LEFT:
					*tx = oy - yout;
					*ty = xout - ox;
					break;
				case RIGHT:
					*tx = yout - oy;
					*ty = ox - xout;
					break;
				case BOTTOM:
					*tx = xout - ox;
					*ty = yout - oy;
					break;
				case TOP:
					*tx = ox - xout;
					*ty = oy - yout;
					break;
			}
			break;
		case INVERSE:
			switch ( dout ) {
				case LEFT:
					*tx = xout - oy;
					*ty = yout + ox;
					break;
				case RIGHT:
					*tx = xout + oy;
					*ty = yout - ox;
					break;
				case BOTTOM:
					*tx = xout - ox;
					*ty = yout - oy;
					break;
				case TOP:
					*tx = xout + ox;
					*ty = yout + oy;
					break;
			}
			break;
	}
}


/*****************************************************************************
Corrects the connection path moving the overcrossing segments out of the
crossed object.

Entry:
	x, y - object lower left corner
	w, h - object width & height
	np   - number of connection point
	p    - original path points list
Exit:
	p - eventually corrected path points list
*****************************************************************************/

Correct ( x, y, w, h, np, p )
	int	x, y, w, h;
	int	np;
	int	p[][2];
{
	int	i, j, c, op[2], od[2];

	op[0] = x;
	op[1] = y;
	od[0] = w;
	od[1] = h;
	for ( j = 1, i = 2 ; i < np - 1 ; i++, j++ ) {
		if ( Overcross(x, y, w, h, p[j][0], p[j][1], p[i][0], p[i][1]) ) {
			c = ( p[j][0] == p[i][0] ) ? 0 : 1;
			if ( EQSIGN(p[j-1][c], p[j][c], p[i][c], p[i+1][c]) ) {
				if ( p[j][c] < p[j-1][c] ) 
					p[j][c] = p[i][c] = op[c] - OFFS + 2;
				else p[j][c] = p[i][c] = op[c] + od[c] - 1 + OFFS - 2;
			}
			else {
				if ( p[i][c] < p[i+1][c] ) 
					p[j][c] = p[i][c] = op[c] - OFFS + 2;
				else p[j][c] = p[i][c] = op[c] + od[c] - 1 + OFFS - 2;
			}
		}
	}
}


/*****************************************************************************
Implementation of line-rectangle intersection test as the first test in 
Cohen-Sutherland clipping algorithm.

Entry:
	x, y   - rectangle lower left corner
	w, h   - rectangle width & height
	x1, y1 - line start point
	x2, y2 - line end point
Return:
	TRUE if the line overcrosses the rectangle, FALSE otherwise
*****************************************************************************/

int	Overcross ( x, y, w, h, x1, y1, x2, y2 )
	int	x, y, w, h;
	int	x1, y1, x2, y2;
{
	int	c1, c2;

	CODE(x, y, w, h, x1, y1, c1);
	CODE(x, y, w, h, x2, y2, c2);
	return(! ( c1 & c2));
}


/*****************************************************************************
Shape functions.

Entry:
	xi, yi - In pad coordinate
	[sign] - sign flag
Exit:
	xy - the computed path points list
*****************************************************************************/

Shape3P ( xi, yi, xy )
	int	xi, yi;
	int	xy[][2];
{
	xy[0][0] = 0;
	xy[0][1] = 0;
	xy[1][0] = 0;
	xy[1][1] = yi;
	xy[2][0] = xi;
	xy[2][1] = yi; 
}


Shape4PD ( xi, yi, xy )
	int	xi, yi;
	int	xy[][2];
{
	xy[0][0] = 0;
	xy[0][1] = 0;
	xy[1][0] = 0;
	xy[1][1] = OFFS;
	xy[2][0] = xi;
	xy[2][1] = xy[1][1]; 
	xy[3][0] = xi;
	xy[3][1] = yi; 
}


Shape4PU ( xi, yi, xy )
	int	xi, yi;
	int	xy[][2];
{
	xy[0][0] = 0;
	xy[0][1] = 0;
	xy[1][0] = 0;
	xy[1][1] = ( ( yi <= 0 ) ? 0 : yi ) + OFFS;
	xy[2][0] = xi;
	xy[2][1] = xy[1][1]; 
	xy[3][0] = xi;
	xy[3][1] = yi; 
}


Shape5PB ( xi, yi, sign, xy )
	int	xi, yi;
	int	sign;
	int	xy[][2];
{
	xy[0][0] = 0;
	xy[0][1] = 0;
	xy[1][0] = 0;
	xy[1][1] = OFFS;
	if ( sign * xi < - SMAX ) xy[2][0] = xi + sign * OFFS;
	else xy[2][0] = ( ( sign * xi < 0 ) ? 0 : xi ) + sign * OFFS;
	xy[2][1] = xy[1][1]; 
	xy[3][0] = xy[2][0];
	xy[3][1] = yi; 
	xy[4][0] = xi;
	xy[4][1] = yi; 
}


Shape5PT ( xi, yi, sign, xy )
	int	xi, yi;
	int	sign;
	int	xy[][2];
{
	xy[0][0] = 0;
	xy[0][1] = 0;
	xy[1][0] = 0;
	xy[1][1] = OFFS;
	xy[2][0] = xi + sign * OFFS;
	xy[2][1] = xy[1][1]; 
	xy[3][0] = xy[2][0];
	xy[3][1] = yi; 
	xy[4][0] = xi;
	xy[4][1] = yi; 
}


Shape6P ( xi, yi, sign, xy )
	int	xi, yi;
	int	sign;
	int	xy[][2];
{
	int	xm;

	xm       = xi / 2;
	xy[0][0] = 0;
	xy[0][1] = 0;
	xy[1][0] = 0;
	xy[1][1] = OFFS;
	if ( yi < 0 ) {
		if ( xi > 0 ) xy[2][0] = ( xm > SMAX ) ? xm : SMAX;
		else xy[2][0] = ( xm > SMAX ) ? xm : - SMAX;
	}
	else {
		if ( xi > 0 ) xy[2][0] = ( xm > SMAX ) ? xm : - SMAX;
		else xy[2][0] = ( xm > SMAX ) ? xm : SMAX;
	}
	xy[2][1] = xy[1][1]; 
	xy[3][0] = xy[2][0];
	xy[3][1] = yi + sign * OFFS;
	xy[4][0] = xi;
	xy[4][1] = xy[3][1]; 
	xy[5][0] = xi;
	xy[5][1] = yi; 
}

