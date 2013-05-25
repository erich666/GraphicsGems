/* 
Fast Anti-Aliasing Polygon Scan Conversion
by Jack Morrison
from "Graphics Gems", Academic Press, 1990

user provides screenX(), vLerp(), and renderPixel() routines.
*/

/*
* Anti-aliased polygon scan conversion by Jack Morrison
*
* This code renders a polygon, computing subpixel coverage at
* 8 times Y and 16 times X display resolution for anti-aliasing.
* One optimization left out for clarity is the use of incremental
* interpolations. X coordinate interpolation in particular can be
* with integers. See Dan Field's article in ACM Transactions on
* Graphics, January 1985 for a fast incremental interpolator.
*/
#include <math.h>
#include "GraphicsGems.h"

#define	SUBYRES	8		/* subpixel Y resolution per scanline */
#define	SUBXRES	16		/* subpixel X resolution per pixel */
#define	MAX_AREA	(SUBYRES*SUBXRES)
#define	MODRES(y)	((y) & 7)		/*subpixel Y modulo */
#define MAX_X	0x7FFF	/* subpixel X beyond right edge */

typedef struct SurfaceStruct {  /* object shading surface info */
	int	red, green, blue;		   /* color components */
	} Surface;
/*
* In  real life, SurfaceStruct will contain many more parameters as
* required by the shading and rendering programs, such as diffuse
* and specular factors, texture information, transparency, etc.
*/

typedef struct VertexStruct	{	/* polygon vertex */
	Vector3	model, world,		/* geometric information */
		    normal, image;
	int y;					/* subpixel display coordinate */
	} Vertex;

Vertex *Vleft, *VnextLeft;		/* current left edge */
Vertex *Vright, *VnextRight;	/* current right edge */

struct	SubPixel  {			/* subpixel extents for scanline */
	int xLeft, xRight;
	} sp[SUBYRES];

int	xLmin, xLmax;		/* subpixel x extremes for scanline */
int	xRmax, xRmin;		/* (for optimization shortcut) */

/* Compute sub-pixel x coordinate for vertex */
extern int screenX(/* Vertex *v */);

/* Interpolate vertex information */
extern void vLerp(/* double alpha, Vertex *Va, *Vb, *Vout */);

/* Render polygon for one pixel, given coverage area */
/*  and bitmask */
extern void renderPixel(/* int x, y, Vertex *V,
						int area, unsigned mask[], 
						Surface *object */);

/*
 * Render shaded polygon
 */
drawPolygon(polygon, numVertex, object)
	Vertex	polygon[];		/*clockwise clipped vertex list */
	int	numVertex;			/*number of vertices in polygon */

	Surface *object;			/* shading parms for this object */
{
	Vertex *endPoly;			/* end of polygon vertex list */
	Vertex VscanLeft, VscanRight;	/* interpolated vertices */ 								/* at scanline */
	double aLeft, aRight;			/* interpolation ratios */
	struct SubPixel *sp_ptr;		/* current subpixel info */
	int xLeft, xNextLeft;			/* subpixel coordinates for */
	int  xRight, xNextRight;		/* active polygon edges */
	int i,y;						

/* find vertex with minimum y (display coordinate) */
Vleft = polygon;
for  (i=1; i<numVertex; i++)
	if  (polygon[i].y < Vleft->y)
		Vleft = &polygon[i];
endPoly = &polygon[numVertex-1];

/* initialize scanning edges */
Vright = VnextRight = VnextLeft = Vleft;

/* prepare bottom of initial scanline - no coverage by polygon */
for (i=0; i<SUBYRES; i++)
	sp[i].xLeft = sp[i].xRight = -1;
xLmin = xRmin = MAX_X;
xLmax = xRmax = -1;

/* scan convert for each subpixel from bottom to top */
for (y=Vleft->y; ; y++)	{

	while (y == VnextLeft->y)	{	/* reached next left vertex */
		VnextLeft = (Vleft=VnextLeft) + 1; 	/* advance */
		if (VnextLeft > endPoly)			/* (wraparound) */
			VnextLeft = polygon;
		if (VnextLeft == Vright)	/* all y's same?  */
			return;				/* (null polygon) */ 
		xLeft = screenX(Vleft);
		xNextLeft = screenX(VnextLeft);
	}

	while (y == VnextRight->y)  { /*reached next right vertex */
		VnextRight = (Vright=VnextRight) -1;
		if (VnextRight < polygon)			/* (wraparound) */
			VnextRight = endPoly;
		xRight = screenX(Vright);
		xNextRight = screenX(VnextRight);
	}

	if (y>VnextLeft->y || y>VnextRight->y)	{
				/* done, mark uncovered part of last scanline */
		for (; MODRES(y); y++)
			sp[MODRES(y)].xLeft = sp[MODRES(y)].xRight = -1;
		renderScanline(Vleft, Vright, y/SUBYRES, object);
		return;
	}

/*
 * Interpolate sub-pixel x endpoints at this y,
 * and update extremes for pixel coherence optimization
 */
	
	sp_ptr = &sp[MODRES(y)];
	aLeft = (double)(y - Vleft->y) / (VnextLeft->y - Vleft->y);
	sp_ptr->xLeft = LERP(aLeft, xLeft, xNextLeft);
	if (sp_ptr->xLeft < xLmin)
		xLmin = sp_ptr->xLeft;
	if (sp_ptr->xLeft > xLmax)
		xLmax = sp_ptr->xLeft;

	aRight = (double)(y - Vright->y) / (VnextRight->y 
					- Vright->y);
	sp_ptr->xRight = LERP(aRight, xRight, xNextRight);
	if (sp_ptr->xRight < xRmin)
		xRmin = sp_ptr->xRight;
	if (sp_ptr->xRight > xRmax)
		xRmax = sp_ptr->xRight;

	if (MODRES(y) == SUBYRES-1)	{	/* end of scanline */
			/* interpolate edges to this scanline */
		vLerp(aLeft, Vleft, VnextLeft, &VscanLeft);
		vLerp(aRight, Vright, VnextRight, &VscanRight);
		renderScanline(&VscanLeft, &VscanRight, y/SUBYRES, object);
		xLmin = xRmin = MAX_X; 		/* reset extremes */
		xLmax = xRmax = -1;
	}
  }
}

/*
 * Render one scanline of polygon
 */

renderScanline(Vl, Vr, y, object)
	Vertex *Vl, *Vr; 	/* polygon vertices interpolated */
					/* at scanline */   
	int y;			/* scanline coordinate */
	Surface *object;	/* shading parms for this object */
{
	Vertex Vpixel;	/*object info interpolated at one pixel */
	unsigned mask[SUBYRES];	/*pixel coverage bitmask */
	int x;			/* leftmost subpixel of current pixel */

	for (x=SUBXRES*floor((double)(xLmin/SUBXRES)); x<=xRmax; x+=SUBXRES) {
		vLerp((double)(x-xLmin)/(xRmax-xLmin), Vl, Vr, &Vpixel);
		computePixelMask(x, mask);
		renderPixel(x/SUBXRES, y, &Vpixel,
					/*computePixel*/Coverage(x), mask, object);
	}
}

/*
 * Compute number of subpixels covered by polygon at current pixel
*/
/*computePixel*/Coverage(x)
	int x;			/* left subpixel of pixel */
{
	int  area;			/* total covered area */
	int partialArea;	  /* covered area for current subpixel y */
	int xr = x+SUBXRES-1;	/*right subpixel of pixel */
	int y;

	/* shortcut for common case of fully covered pixel */
	if (x>xLmax && x<xRmin)
		return MAX_AREA;
	
	for (area=y=0; y<SUBYRES; y++) {
		partialArea = MIN(sp[y].xRight, xr)
			 - MAX(sp[y].xLeft, x) + 1;
		if (partialArea > 0)
			area += partialArea;
	}
	return area;
}

/* Compute bitmask indicating which subpixels are covered by 
 * polygon at current pixel. (Not all hidden-surface methods
 * need this mask. )
*/
computePixelMask(x, mask)
	int x;			/* left subpixel of pixel */
	unsigned mask[];	/* output bitmask */
{
	static unsigned leftMaskTable[] =
		{ 0xFFFF, 0x7FFF, 0x3FFF, 0x1FFF, 0x0FFF, 0x07FF, 0x03FF,
		  0x01FF, 0x00FF, 0x007F, 0x003F, 0x001F, 0x000F, 0x0007,
		  0x0003, 0x0001  };
	static unsigned rightMaskTable[] = 
		{ 0x8000, 0xC000, 0xE000, 0xF000, 0xF800, 0xFC00, 
		  0xFE00, 0xFF00, 0xFF80, 0xFFC0, 0xFFE0, 0xFFF0,
		  0xFFF8, 0xFFFC, 0xFFFE, 0xFFFF   };
	unsigned leftMask, rightMask; 		/* partial masks */
	int xr = x+SUBXRES-1;			/* right subpixel of pixel */
	int y;

/* shortcut for common case of fully covered pixel */
	if (x>xLmax && x<xRmin) 	{
		for (y=0; y<SUBYRES; y++)
			mask[y] = 0xFFFF;
	} else 	{
		for (y=0; y<SUBYRES; y++)	{
			if (sp[y].xLeft < x) 	/* completely left of pixel*/
				leftMask = 0xFFFF;
			else if (sp[y].xLeft > xr)  /* completely right */	
				leftMask = 0;
			else
				leftMask = leftMaskTable[sp[y].xLeft -x];

			if (sp[y].xRight > xr)  	/* completely  */
							/* right of pixel*/
				rightMask = 0xFFFF;
			else if (sp[y].xRight < x)	/*completely left */
				rightMask = 0;
			else
				rightMask = rightMaskTable[sp[y].xRight -x];
			mask[y] = leftMask & rightMask;
		}
	}
}
