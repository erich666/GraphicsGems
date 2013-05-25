#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "nurbs.h"
#include "drawing.h"

extern void MakeWindow( void );	    /* External system routine to create a window */

void (*DrawTriangle)();		    /* Pointer to triangle drawing function */

/* This generates the NURB surface for a torus, centered about the origin */

static NurbSurface *
generateTorus(double majorRadius, double minorRadius)
{
    /* These define the shape of a unit torus centered about the origin. */
    double xvalues[] = { 0.0, -1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0 };
    double yvalues[] = { 1.0, 1.0, 0.0, -1.0, -1.0, -1.0, 0.0, 1.0, 1.0 };
    double zvalues[] = { 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, -1.0, 0.0 };
    double offsets[] = { -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0 };

    /* Piecewise Bezier knot vector for a quadratic curve with four segments */
    long knots[] = { 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4 };

    long i, j;

    double r2over2 = sqrt( 2.0 ) / 2.0;
    double weight;

    NurbSurface * torus = (NurbSurface *) malloc( sizeof(NurbSurface) );
    CHECK( torus );

    /* Set up the dimension and orders of the surface */

    torus->numU = 9;	    /* A circle is formed from nine points */
    torus->numV = 9;
    torus->orderU = 3;	    /* Quadratic in both directions */
    torus->orderV = 3;

    /* After the dimension and orders are set, AllocNurb creates the dynamic
     * storage for the control points and the knot vectors */

    AllocNurb( torus, NULL, NULL );

    for (i = 0; i < 9; i++)
    {
	for (j = 0; j < 9; j++)
	{
	    weight = ((j & 1) ? r2over2 : 1.0) * ((i & 1) ? r2over2 : 1.0);
	    /* Notice how the weights are pre-multiplied with the x, y and z values */
	    torus->points[i][j].x = xvalues[j]
				    * (majorRadius + offsets[i] * minorRadius) * weight;
	    torus->points[i][j].y = yvalues[j]
				    * (majorRadius + offsets[i] * minorRadius) * weight;
	    torus->points[i][j].z = (zvalues[i] * minorRadius) * weight;
	    torus->points[i][j].w = weight;
	}
    }

    /* The knot vectors define piecewise Bezier segments (the same in both U and V). */

    for (i = 0; i < torus->numU + torus->orderU; i++)
	torus->kvU[i] = torus->kvV[i] = (double) knots[i];

    return torus;
}

/* These drawing routines assume a window of around 400x400 pixels */

void
ScreenProject( Point4 * worldPt, Point3 * screenPt )
{
    screenPt->x = worldPt->x / worldPt->w * 100 + 200;
    screenPt->y = worldPt->y / worldPt->w * 100 + 200;
    screenPt->z = worldPt->z / worldPt->w * 100 + 200;
}

static void
LineTriangle( SurfSample * v0, SurfSample * v1, SurfSample * v2 )
{
    MoveTo( (short) (v0->point.x * 100 + 200), (short) (v0->point.y * 100 + 200) );
    LineTo( (short) (v1->point.x * 100 + 200), (short) (v1->point.y * 100 + 200) );
    LineTo( (short) (v2->point.x * 100 + 200), (short) (v2->point.y * 100 + 200) );
    LineTo( (short) (v0->point.x * 100 + 200), (short) (v0->point.y * 100 + 200) );
}

main()
{
    NurbSurface * torus;

    MakeWindow();	    /* Create a window on the screen */

    /* Set up the subdivision tolerance (facets span about two pixels) */
    SubdivTolerance = 2.0;

    DrawTriangle = LineTriangle;

    torus = generateTorus( 1.3, 0.3 );

    DrawSubdivision( torus );
/*  DrawEvaluation( torus );   */   /* Alternate drawing method */
}
