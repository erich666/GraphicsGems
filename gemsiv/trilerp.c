/*
 * C code from the article
 * "Tri-linear Interpolation"
 * by Steve Hill, sah@ukc.ac.uk
 * in "Graphics Gems IV", Academic Press, 1994
 *
 * compile with "cc -DMAIN ..." to create a test program
 */

#include <math.h>
#include "GraphicsGems.h"

double
trilinear(p, d, xsize, ysize, zsize, def)
Point3	 *p;
double	 *d;
int	 xsize, ysize, zsize;
double	 def;
{
#   define DENS(X, Y, Z) d[(X)+xsize*((Y)+ysize*(Z))]

    int	       x0, y0, z0,
	       x1, y1, z1;
    double     *dp,
	       fx, fy, fz,
	       d000, d001, d010, d011,
	       d100, d101, d110, d111,
	       dx00, dx01, dx10, dx11,
	       dxy0, dxy1, dxyz;

    x0 = floor(p->x); fx = p->x - x0;
    y0 = floor(p->y); fy = p->y - y0;
    z0 = floor(p->z); fz = p->z - z0;

    x1 = x0 + 1;
    y1 = y0 + 1;
    z1 = z0 + 1;

    if (x0 >= 0 && x1 < xsize &&
	y0 >= 0 && y1 < ysize &&
	z0 >= 0 && z1 < zsize)
    {
	dp = &DENS(x0, y0, z0);
	d000 = dp[0];
	d100 = dp[1];
	dp += xsize;
	d010 = dp[0];
	d110 = dp[1];
	dp += xsize*ysize;
	d011 = dp[0];
	d111 = dp[1];
	dp -= xsize;
	d001 = dp[0];
	d101 = dp[1];
    }
    else
    {
#	define INRANGE(X, Y, Z) \
		  ((X) >= 0 && (X) < xsize && \
		   (Y) >= 0 && (Y) < ysize && \
		   (Z) >= 0 && (Z) < zsize)

	d000 = INRANGE(x0, y0, z0) ? DENS(x0, y0, z0) : def;
	d001 = INRANGE(x0, y0, z1) ? DENS(x0, y0, z1) : def;
	d010 = INRANGE(x0, y1, z0) ? DENS(x0, y1, z0) : def;
	d011 = INRANGE(x0, y1, z1) ? DENS(x0, y1, z1) : def;

	d100 = INRANGE(x1, y0, z0) ? DENS(x1, y0, z0) : def;
	d101 = INRANGE(x1, y0, z1) ? DENS(x1, y0, z1) : def;
	d110 = INRANGE(x1, y1, z0) ? DENS(x1, y1, z0) : def;
	d111 = INRANGE(x1, y1, z1) ? DENS(x1, y1, z1) : def;
    }

    dx00 = LERP(fx, d000, d100);
    dx01 = LERP(fx, d001, d101);
    dx10 = LERP(fx, d010, d110);
    dx11 = LERP(fx, d011, d111);

    dxy0 = LERP(fy, dx00, dx10);
    dxy1 = LERP(fy, dx01, dx11);

    dxyz = LERP(fz, dxy0, dxy1);

    return dxyz;
}

#ifdef MAIN

/* test program for trilinear interpolation */

#include <stdio.h>

#define XSIZE	2
#define YSIZE	2
#define ZSIZE	2

main()
{
#define TDENS(X,Y,Z) d[(X)+XSIZE*((Y)+YSIZE*(Z))]

	extern double trilinear();
	double	*d, def;
	int	 x, y, z;
	Point3	p;

	d = (double *) malloc(sizeof(double) * XSIZE * YSIZE * ZSIZE);

	printf("Test for trilinear interpolation\n");
	printf("Enter the densities for the corners of a cube\n");

	for (x = 0; x < XSIZE; x++)
	for (y = 0; y < YSIZE; y++)
	for (z = 0; z < ZSIZE; z++)
	{
		printf("Point (%d, %d, %d) is: ", x, y, z);
		scanf("%lf", &TDENS(x, y, z));
	}

	printf("Enter the default density: ");
	scanf("%lf", &def);

	printf("Enter point of interest: ");
	while (scanf("%lf %lf %lf", &p.x, &p.y, &p.z) == 3)
	{
		double	n;

		n = trilinear(&p, d, XSIZE, YSIZE, ZSIZE, def);
		printf("Density at (%lf, %lf, %lf) is %lf\n", p.x, p.y, p.z, n);
		printf("Enter point of interest: ");
	}
}

#endif
