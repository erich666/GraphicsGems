#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vec.h"
#include "pcube.h"


/*
 *  Calculate a vector perpendicular to a planar polygon.
 *  If the polygon is non-planar, a "best fit" plane will be used.
 *  The polygon may be concave or even self-intersecting,
 *  but it should have nonzero area or the result will be a zero vector
 *  (e.g. the "bowtie" quad).
 *  The length of vector will be twice the area of the polygon.
 *  NOTE:  This algorithm gives the same answer as Newell's method
 *  (see Graphics Gems III) but is slightly more efficient than Newell's
 *  for triangles and quads (slightly less efficient for higher polygons).
 */
static real *
get_polygon_normal(real normal[3],
		   int nverts, const real verts[/* nverts */][3])
{
    int i;
    real tothis[3], toprev[3], cross[3];

    /*
     * Triangulate the polygon and sum up the nverts-2 triangle normals.
     */
    ZEROVEC3(normal);
    VMV3(toprev, verts[1], verts[0]);  	/* 3 subtracts */
    for (i = 2; i <= nverts-1; ++i) {   /* n-2 times... */
	VMV3(tothis, verts[i], verts[0]);    /* 3 subtracts */
	VXV3(cross, toprev, tothis);         /* 3 subtracts, 6 multiplies */
	VPV3(normal, normal, cross);         /* 3 adds */
	SET3(toprev, tothis);
    }
    return normal;
}


main(int argc, char *argv[])
{
	int i, j, k, ntris, ins=0;
	real tri[3][3], normal[3];
	long fast, full;

	if(argc != 2)
		exit(printf("usage: %s <ntris>\n", argv[0]));
	ntris = atoi(argv[1]);
	for(i=0; i<ntris; i++) {
		for(j=0; j<3; j++)
			for(k=0; k<3; k++)
				tri[j][k] = (drand48() - .5) * 5.0;
		get_polygon_normal(normal, 3, tri);
		fast = fast_polygon_intersects_cube(3, tri, normal, 0, 0);
		full = polygon_intersects_cube(3, tri, normal, 0, 0);
		if(fast != full)
		{
			printf("fast = %d, full = %d\n", fast, full);
			printf("\t(%f,%f,%f)\n\t,(%f,%f,%f)\n\t,(%f,%f,%f)\n",
				tri[0][0], tri[0][1], tri[0][2],
				tri[1][0], tri[1][1], tri[1][2],
				tri[2][0], tri[2][1], tri[2][2]);
		}
		if(fast)
			ins++;
	}
	printf("%f percent intersected\n", (float)ins / ntris * 100);
}
