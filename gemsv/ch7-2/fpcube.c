
/*
 *                  FAST POLYGON-CUBE INTERSECTION TESTS
 *                  by Melinda (Daniel) Green
 *                  January 1994
 *
 *
 * The original inspiration for this routine comes from the triangle-cube
 * intersection algorithm in Graphics Gems III by Douglas Voorhies.
 * Aside from the fact that the original code was special-cased for triangles
 * only, it suffered from some bugs.  Don Hatch re-wrote the non-trivial tests
 * using the same basic ideas, fixed the bugs and also made it more
 * general and efficient.  Don's implementation performs just the
 * intersection calculations without any trivial accept or reject tests,
 * and is embodied in the routine polygon_intersects_cube().
 *
 * The function implemented here is simply a wrapper that begins with a
 * slightly more efficient version of Voorhies' original trivial reject tests
 * and only calls polygon_intersects_cube() when those tests fail.
 * The result is a fast and robust polygon-cube tester.
 *
 * Also included here is trivial_vertex_tests() which is used by
 * fast_polygon_intersects_cube().  It can be used to quickly test an entire
 * set of vertices for trivial reject or accept.  This is useful for testing
 * polyhedra or polygon meshes.
 *
 * WARNING: When used to test intersection of a polyhedron with the unit cube,
 * remember that these routines only test *surfaces* and not volumes.
 * If polygon_intersects_cube() reports no intersection with any of the faces
 * of the polyhedron, the caller should be aware that the polyhedron
 * could still contain the whole unit cube which would then need to be checked
 * with a point-within-polyhedron test.  The origin would be a natural point
 * to check in such a test.
 */


#include "pcube.h"

#ifndef __cplusplus
#define inline
#endif


#define TEST_AGAINST_PARALLEL_PLANES(posbit, negbit, value, limit)	\
	if (mask & (posbit|negbit)) {					\
		register real temp = value;				\
		if ((mask & posbit) && temp > limit)			\
			outcode |= posbit;				\
		else if ((mask & negbit) && temp < -limit)		\
			outcode |= negbit;				\
	}								\


/*
 * Tells which of the six face-planes the given point is outside of.
 * Only tests faces not represented in "mask".
 */

static inline unsigned long
face_plane(const real p[3], unsigned long mask)
{
	register unsigned long outcode = 0L;

	TEST_AGAINST_PARALLEL_PLANES(0x001, 0x002, p[0], 0.5)
	TEST_AGAINST_PARALLEL_PLANES(0x004, 0x008, p[1], 0.5)
	TEST_AGAINST_PARALLEL_PLANES(0x010, 0x020, p[2], 0.5)

	return(outcode);
}



/*
 * Tells which of the twelve edge planes the given point is outside of.
 * Only tests faces not represented in "mask".
 */

static inline unsigned long
bevel_2d(const real p[3], unsigned long mask)
{
	register unsigned long outcode = 0L;

	TEST_AGAINST_PARALLEL_PLANES(0x001, 0x002, p[0] + p[1], 1.0)
	TEST_AGAINST_PARALLEL_PLANES(0x004, 0x008, p[0] - p[1], 1.0)
	TEST_AGAINST_PARALLEL_PLANES(0x010, 0x020, p[0] + p[2], 1.0)
	TEST_AGAINST_PARALLEL_PLANES(0x040, 0x080, p[0] - p[2], 1.0)
	TEST_AGAINST_PARALLEL_PLANES(0x100, 0x200, p[1] + p[2], 1.0)
	TEST_AGAINST_PARALLEL_PLANES(0x400, 0x800, p[1] - p[2], 1.0)

	return(outcode);
}



/*
 * Tells which of the eight corner planes the given point is outside of.
 * Only tests faces not represented in "mask".
 */

static inline unsigned long
bevel_3d(const real p[3], unsigned long mask)
{
	register unsigned long outcode = 0L;

	TEST_AGAINST_PARALLEL_PLANES(0x001, 0x002, p[0] + p[1] + p[2], 1.5)
	TEST_AGAINST_PARALLEL_PLANES(0x004, 0x008, p[0] + p[1] - p[2], 1.5)
	TEST_AGAINST_PARALLEL_PLANES(0x010, 0x020, p[0] - p[1] + p[2], 1.5)
	TEST_AGAINST_PARALLEL_PLANES(0x040, 0x080, p[0] - p[1] - p[2], 1.5)

	return(outcode);
}



/*
 * Returns 1 if any of the vertices are inside the cube of edge length 1
 * centered at the origin (trivial accept), 0 if all vertices are outside
 * of any testing plane (trivial reject), -1 otherwise (couldn't help).
 */

extern int
trivial_vertex_tests(int nverts, const real verts[][3],
			int already_know_verts_are_outside_cube)
{
	register unsigned long cum_and;  /* cumulative logical ANDs */
	register int i;

	/*
	 * Compare the vertices with all six face-planes.
	 * If it's known that no vertices are inside the unit cube
	 * we can exit the loop early if we run out of bounding
	 * planes that all vertices might be outside of.  That simply means
	 * that this test failed and we can go on to the next one.
	 * If we must test for vertices within the cube, the loop is slightly
	 * different in that we can trivially accept if we ever do find a
	 * vertex within the cube, but we can't break the loop early if we run
	 * out of planes to reject against because subsequent vertices might
	 * still be within the cube.
	 */
	cum_and = ~0L;  /* Set to all "1" bits */
	if(already_know_verts_are_outside_cube) {
		for(i=0; i<nverts; i++)
			if(0L == (cum_and = face_plane(verts[i], cum_and)))
				break; /* No planes left to trivially reject */
	}
	else {
		for(i=0; i<nverts; i++) {
			/* Note the ~0L mask below to always test all planes */
			unsigned long face_bits = face_plane(verts[i], ~0L);
			if(0L == face_bits)  /* vertex is inside the cube */
				return 1; /* trivial accept */
			cum_and &= face_bits;
		}
	}
	if(cum_and != 0L)  /* All vertices outside some face plane. */
		return 0;  /* Trivial reject */

	/*
	 * Now do the just the trivial reject test against the 12 edge planes.
	 */
	cum_and = ~0L;  /* Set to all "1" bits */
	for(i=0; i<nverts; i++)
		if(0L == (cum_and = bevel_2d(verts[i], cum_and)))
			break; /* No planes left that might trivially reject */
	if(cum_and != 0L)  /* All vertices outside some edge plane. */
		return 0;  /* Trivial reject */

	/*
	 * Now do the trivial reject test against the 8 corner planes.
	 */
	cum_and = ~0L;  /* Set to all "1" bits */
	for(i=0; i<nverts; i++)
		if(0L == (cum_and = bevel_3d(verts[i], cum_and)))
			break; /* No planes left that might trivially reject */
	if(cum_and != 0L)  /* All vertices outside some corner plane. */
		return 0;  /* Trivial reject */

	/*
	 * By now we know that the polygon is not to the outside of any of the
	 * test planes and can't be trivially accepted *or* rejected.
	 */
	return -1;
}



/*
 * This is a version of the same polygon-cube intersection that first calls
 * trivial_vertex_tests() to hopefully skip the more expensive definitive test.
 * It simply calls polygon_intersects_cube() when that fails.
 * Note that after the trivial tests we at least know that all vertices are
 * outside the cube and can therefore pass a true flag to
 * polygon_intersects_cube().
 */
extern int
fast_polygon_intersects_cube(int nverts, const real verts[][3],
                        const real polynormal[3],
			int already_know_verts_are_outside_cube,
			int already_know_edges_are_outside_cube)
{
	int quick_test = trivial_vertex_tests(nverts, verts,
				already_know_verts_are_outside_cube);
	if(-1 == quick_test)
		return polygon_intersects_cube(nverts, verts, polynormal, 1,
				already_know_edges_are_outside_cube);
	else
		return quick_test;
}
