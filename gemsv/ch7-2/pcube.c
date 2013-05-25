
/*
 *			polygon_intersects_cube()
 *			by Don Hatch
 *			January 1994
 *
 *   Algorithm:
 *        1. If any edge intersects the cube, return true.
 *           Testing whether a line segment intersects the cube
 *           is equivalent to testing whether the origin is contained
 *           in the rhombic dodecahedron obtained by dragging
 *           a unit cube from (being centered at) one segment endpoint
 *           to the other.
 *        2. If the polygon interior intersects the cube, return true.
 *           Since we know no vertex or edge intersects the cube,
 *           this amounts to testing whether any of the four cube diagonals
 *           intersects the interior of the polygon.  (Same as voorhies's test).
 *        3. Return false.
 */

#include "pcube.h"
#include "vec.h"


#define FOR(i,n) for ((i) = 0; (i) < (n); ++(i))
#define MAXDIM2(v) ((v)[0] > (v)[1] ? 0 : 1)
#define MAXDIM3(v) ((v)[0] > (v)[2] ? MAXDIM2(v) : MAXDIM2((v)+1)+1)
#define ABS(x) ((x)<0 ? -(x) : (x))
#define SQR(x) ((x)*(x))
#define SIGN_NONZERO(x) ((x) < 0 ? -1 : 1)
/* note a and b can be in the reverse order and it still works! */
#define IN_CLOSED_INTERVAL(a,x,b) (((x)-(a)) * ((x)-(b)) <= 0)
#define IN_OPEN_INTERVAL(a,x,b) (((x)-(a)) * ((x)-(b)) < 0)



#define seg_contains_point(a,b,x) (((b)>(x)) - ((a)>(x)))
/*
 *  Tells whether a given polygon with nonzero area
 *  contains a point which is assumed to lie in the plane of the polygon.
 *  Actually returns the multiplicity of containment.
 *  This will always be 1 or 0 for non-self-intersecting planar
 *  polygons with the normal in the standard direction
 *  (towards the eye when looking at the polygon so that it's CCW).
 */
extern int
polygon_contains_point_3d(int nverts, const real verts[/* nverts */][3],
			const real polynormal[3],
			real point[3])
{
    real abspolynormal[3];
    int zaxis, xaxis, yaxis, i, count;
    int xdirection;
    const real *v, *w;

    /*
     * Determine which axis to ignore
     * (the one in which the polygon normal is largest)
     */
    FOR(i,3)
	abspolynormal[i] = ABS(polynormal[i]);
    zaxis = MAXDIM3(abspolynormal);

    if (polynormal[zaxis] < 0) {
	xaxis = (zaxis+2)%3;
	yaxis = (zaxis+1)%3;
    } else {
	xaxis = (zaxis+1)%3;
	yaxis = (zaxis+2)%3;
    }

    count = 0;
    FOR(i,nverts) {
	v = verts[i];
	w = verts[(i+1)%nverts];
	if (xdirection = seg_contains_point(v[xaxis], w[xaxis], point[xaxis])) {
	    if (seg_contains_point(v[yaxis], w[yaxis], point[yaxis])) {
		if (xdirection * (point[xaxis]-v[xaxis])*(w[yaxis]-v[yaxis]) <= 
		    xdirection * (point[yaxis]-v[yaxis])*(w[xaxis]-v[xaxis]))
		    count += xdirection;
	    } else {
		if (v[yaxis] <= point[yaxis])
		    count += xdirection;
	    }
	}
    }
    return count;
}




/*
 *  A segment intersects the unit cube centered at the origin
 *  iff the origin is contained in the solid obtained
 *  by dragging a unit cube from one segment endpoint to the other.
 *  (This solid is a warped rhombic dodecahedron.)
 *  This amounts to 12 sidedness tests.
 *  Also, this test works even if one or both of the segment endpoints is
 *  inside the cube.
 */
extern int
segment_intersects_cube(const real v0[3], const real v1[3])
{
    int i, iplus1, iplus2, edgevec_signs[3];
    real edgevec[3];

    VMV3(edgevec, v1, v0);

    FOR(i,3)
	edgevec_signs[i] = SIGN_NONZERO(edgevec[i]);

    /*
     * Test the three cube faces on the v1-ward side of the cube--
     * if v0 is outside any of their planes then there is no intersection.
     * Also test the three cube faces on the v0-ward side of the cube--
     * if v1 is outside any of their planes then there is no intersection.
     */

    FOR(i,3) {
	if (v0[i] * edgevec_signs[i] >  .5) return 0;
	if (v1[i] * edgevec_signs[i] < -.5) return 0;
    }

    /*
     * Okay, that's the six easy faces of the rhombic dodecahedron
     * out of the way.  Six more to go.
     * The remaining six planes bound an infinite hexagonal prism
     * joining the petrie polygons (skew hexagons) of the two cubes
     * centered at the endpoints.
     */

    FOR(i,3) {
	real rhomb_normal_dot_v0, rhomb_normal_dot_cubedge;

	iplus1 = (i+1)%3;
	iplus2 = (i+2)%3;

#ifdef THE_EASY_TO_UNDERSTAND_WAY

	{
	real rhomb_normal[3], cubedge_midpoint[3];

	/*
	 * rhomb_normal = VXV3(edgevec, unit vector in direction i),
	 * being cavalier about which direction it's facing
	 */
	rhomb_normal[i] = 0;
	rhomb_normal[iplus1] = edgevec[iplus2];
	rhomb_normal[iplus2] = -edgevec[iplus1];

	/*
	 *  We now are describing a plane parallel to
	 *  both segment and the cube edge in question.
	 *  if |DOT3(rhomb_normal, an arbitrary point on the segment)| >
	 *  |DOT3(rhomb_normal, an arbitrary point on the cube edge in question|
	 *  then the origin is outside this pair of opposite faces.
	 *  (This is equivalent to saying that the line
	 *  containing the segment is "outside" (i.e. further away from the
	 *  origin than) the line containing the cube edge.
	 */

	cubedge_midpoint[i] = 0;
	cubedge_midpoint[iplus1] = edgevec_signs[iplus1]*.5;
	cubedge_midpoint[iplus2] = -edgevec_signs[iplus2]*.5;

	rhomb_normal_dot_v0 = DOT3(rhomb_normal, v0);
	rhomb_normal_dot_cubedge = DOT3(rhomb_normal,cubedge_midpoint);
	}

#else /* the efficient way */

	rhomb_normal_dot_v0 = edgevec[iplus2] * v0[iplus1]
			    - edgevec[iplus1] * v0[iplus2];

	rhomb_normal_dot_cubedge = .5 *
				(edgevec[iplus2] * edgevec_signs[iplus1] +
				 edgevec[iplus1] * edgevec_signs[iplus2]);

#endif /* the efficient way */

	if (SQR(rhomb_normal_dot_v0) > SQR(rhomb_normal_dot_cubedge))
	    return 0;	/* origin is outside this pair of opposite planes */
    }
    return 1;
}





/*
 * Tells whether a given polygon intersects the cube of edge length 1
 * centered at the origin.
 * Always returns 1 if a polygon edge intersects the cube;
 * returns the multiplicity of containment otherwise.
 * (See explanation of polygon_contains_point_3d() above).
 */
extern int
polygon_intersects_cube(int nverts, const real verts[/* nverts */][3],
			const real polynormal[3],
			int already_know_vertices_are_outside_cube, /*unused*/
			int already_know_edges_are_outside_cube)
{
    int i, best_diagonal[3];
    real p[3], t;

    /*
     * If any edge intersects the cube, return 1.
     */
    if (!already_know_edges_are_outside_cube)
	FOR(i,nverts)
	    if (segment_intersects_cube(verts[i], verts[(i+1)%nverts]))
		return 1;

    /*
     * If the polygon normal is zero and none of its edges intersect the
     * cube, then it doesn't intersect the cube
     */
    if (ISZEROVEC3(polynormal))
	return 0;

    /*
     * Now that we know that none of the polygon's edges intersects the cube,
     * deciding whether the polygon intersects the cube amounts
     * to testing whether any of the four cube diagonals intersects
     * the interior of the polygon.
     *
     * Notice that we only need to consider the cube diagonal that comes
     * closest to being perpendicular to the plane of the polygon.
     * If the polygon intersects any of the cube diagonals,
     * it will intersect that one.
     */

    FOR(i,3)
	best_diagonal[i] = SIGN_NONZERO(polynormal[i]);

    /*
     * Okay, we have the diagonal of interest.
     * The plane containing the polygon is the set of all points p satisfying
     *      DOT3(polynormal, p) == DOT3(polynormal, verts[0])
     * So find the point p on the cube diagonal of interest
     * that satisfies this equation.
     * The line containing the cube diagonal is described parametrically by
     *      t * best_diagonal
     * so plug this into the previous equation and solve for t.
     *      DOT3(polynormal, t * best_diagonal) == DOT3(polynormal, verts[0])
     * i.e.
     *      t = DOT3(polynormal, verts[0]) / DOT3(polynormal, best_diagonal)
     *
     * (Note that the denominator is guaranteed to be nonzero, since
     * polynormal is nonzero and best_diagonal was chosen to have the largest
     * magnitude dot-product with polynormal)
     */
    t = DOT3(polynormal, verts[0])
      / DOT3(polynormal, best_diagonal);

    if (!IN_CLOSED_INTERVAL(-.5, t, .5))
	return 0;  /* intersection point is not in cube */

    SXV3(p, t, best_diagonal);    /* p = t * best_diagonal */

    return polygon_contains_point_3d(nverts, verts, polynormal, p);
}
