/*
Fast Bitmap Stretching
Tomas Mšller
*/













/* user provided routines */

short ReadPixel(long x,long y);/* returns color of (x,y) in source bitmap*/
void SetColor(short Col); /* set the current writing color to Col */
void WritePixel(long x,long y); /* write a pixel at (x,y) in destination bitmap with the current writing color */

#define sign(x) ((x)>0 ? 1:-1)

void RectStretch(long xs1,long ys1,long xs2,long ys2,long xd1,long yd1,long xd2,long yd2) ;
void Stretch(long x1,long x2,long y1,long y2,long yr,long yw) ;
void CircleStretch(long SBMINX,long SBMAXX,long xc,long yc,long r) ;
void Stretch2Lines(long x1,long x2,long y1,long y2,long yr1,long yw1,long yr2,long yw2) ;

/**********************************************************
 RectStretch enlarges or diminishes a source rectangle of
 a bitmap to a destination rectangle. The source
 rectangle is selected by the two points (xs1,ys1) and
 (xs2,ys2), and the destination rectangle by (xd1,yd1) and
 (xd2,yd2). Since readability of source-code is wanted,
 some optimizations have been left out for the reader:
 It«s possible to read one line at a time, by first
 stretching in x-direction and then stretching that bitmap
 in y-direction.
 Entry:
	xs1,ys1 - first point of source rectangle
	xs2,ys2 - second point of source rectangle
	xd1,yd1 - first point of destination rectangle
	xd2,yd2 - second point of destination rectangle
**********************************************************/
void RectStretch(long xs1,long ys1,long xs2,long ys2,long xd1,long yd1,long xd2,long yd2)
{
	long dx,dy,e,d,dx2;
	short sx,sy;
	dx=abs((int)(yd2-yd1));
	dy=abs((int)(ys2-ys1));
	sx=sign(yd2-yd1);
	sy=sign(ys2-ys1);
	e=(dy<<1)-dx;
	dx2=dx<<1;
	dy<<=1;
	for(d=0;d<=dx;d++)
	{
		Stretch(xd1,xd2,xs1,xs2,ys1,yd1);
		while(e>=0)
		{
			ys1+=sy;
			e-=dx2;
		}
		yd1+=sx;
		e+=dy;
	}
}

/**********************************************************
 Stretches a horizontal source line onto a horizontal
 destination line. Used by RectStretch.
 Entry:
	x1,x2 - x-coordinates of the destination line
	y1,y2 - x-coordinates of the source line
	yr    - y-coordinate of source line
	yw    - y-coordinate of destination line
**********************************************************/
void Stretch(long x1,long x2,long y1,long y2,long yr,long yw)
{
	long dx,dy,e,d,dx2;
	short sx,sy,color;
	dx=abs((int)(x2-x1));
	dy=abs((int)(y2-y1));
	sx=sign(x2-x1);
	sy=sign(y2-y1);
	e=(dy<<1)-dx;
	dx2=dx<<1;
	dy<<=1;
	for(d=0;d<=dx;d++)
	{
		color=ReadPixel(y1,yr); 
		SetColor(color);
		WritePixel(x1,yw);
		while(e>=0)
		{
			y1+=sy;
			e-=dx2;
		}
		x1+=sx;
		e+=dy;
	}
}

/**********************************************************
 CircleStretch stretches a source rectangle, selected by
 the two points (SBMINX,0) and (SBMAXX,2*r-1), onto a
 Bresenham circle at (xc,yc) with radius=r.
 Instead of writing pixels on the circle, horizontal lines
 of the source rectangle are being stretched onto all
 horizontal lines of the circle. 
 Entry:
	SBMINX - min x of source rectangle
	SBMAXX - max x of source rectangle 
	xc,yc  - center of the circle
	r      - radius of circle
**********************************************************/
void CircleStretch(long SBMINX,long SBMAXX,long xc,long yc,long r)
{
	long p=3-(r<<1),x=0,y=r;
	while(x<y)
	{
		/* stretch lines in first octant */
		Stretch2Lines(xc-y,xc+y,SBMINX,SBMAXX,r-x,yc-x,r+x,yc+x);
		if(p<0) p=p+(x<<2)+6;
		else
		{
			/* stretch lines in second octant */
			Stretch2Lines(xc-x,xc+x,SBMINX,SBMAXX,r-y,yc-y,r+y,yc+y);
			p=p+((x-y)<<2)+10;
			y--;
		}
		x++;
	}
	if(x==y) Stretch2Lines(xc-x,xc+x,SBMINX,SBMAXX,r-y,yc-y,r+y,yc+y);
}

/**********************************************************
 Stretch2Lines stretches two source lines with same length
 and different y-coordinates onto two destination lines
 with same length and different y-coordinates. Used by
 CircleStretch.
 Entry:
	x1,x2 - x-coordinates of the destination line
	y1,y2 - x-coordinates of the source line
	yr1   - y-coordinate of source line # 1
	yw1   - y-coordinate of destination line # 1
	yr2   - y-coordinate of source line # 2
	yw2   - y-coordinate of destination line # 2
**********************************************************/
void Stretch2Lines(long x1,long x2,long y1,long y2,long yr1,long yw1,long yr2,long yw2)
{
	long dx,dy,e,d,dx2;
	short sx,sy,color;
	dx=abs((int)(x2-x1));
	dy=abs((int)(y2-y1));
	sx=sign(x2-x1);
	sy=sign(y2-y1);
	e=(dy<<1)-dx;
	dx2=dx<<1;
	dy<<=1;
	for(d=0;d<=dx;d++)
	{
		color=ReadPixel(y1,yr1);
		SetColor(color);
		WritePixel(x1,yw1);
		color=ReadPixel(y1,yr2);
		SetColor(color);
		WritePixel(x1,yw2);
 
		while(e>=0)
		{
			y1+=sy;
			e-=dx2;
		}
		x1+=sx;
		e+=dy;
	}
}
