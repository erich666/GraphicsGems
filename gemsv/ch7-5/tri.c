#include "basic.h"
#include <sys/time.h>


static int initialise(nseg)
     int nseg;
{
  register int i;

  for (i = 1; i <= nseg; i++)
    seg[i].is_inserted = FALSE;
  generate_random_ordering(nseg);
  
  return 0;
}

#ifdef STANDALONE

int main(argc, argv)
     int argc;
     char *argv[];
{
  int n, nmonpoly;
  FILE *infile;
  int op[SEGSIZE][3], i;

  if (argc < 2)
    {
      fprintf(stderr, "usage: triangulate <filename>\n");
      exit(1);
    }
  else
    if ((infile = fopen(argv[1], "r")) == NULL)
      {
	perror(argv[1]);
	exit(1);
      }
  
#ifdef CLOCK
  clock();
#endif
  
  n = read_segments(infile);
  for (i = 0; i < 1; i++)
    {
      initialise(n);
      construct_trapezoids(n, seg);
      nmonpoly = monotonate_trapezoids(n);
      triangulate_monotone_polygons(nmonpoly, op);
    }
  
#ifdef CLOCK
  printf("CPU time used: %ld microseconds\n", clock());
#endif
  
  return 0;
}

#else  /* Not standalone. Use this as an interface routine */

/* The points constituting the polygon are specified in anticlockwise
 * order. If there are n points in the polygon, i/p would be the
 * points p0, p1....p(n) (where p0 and pn are the same point). The
 * output is contained in the array "triangles".
 * Every triangle is output in anticlockwise order and the 3
 * integers are the indices of the points. Thus, the triangle (i, j, k)
 * refers to the triangle formed by the points (pi, pj, pk). Before
 * using this routine, please check-out that you do not conflict with
 * the global variables  defined in basic.h.
 *
 * n:         number of points in polygon (p0 = pn)
 * vertices:  the vertices p0, p1..., p(n) of the polygon
 * triangles: output array containing the triangles 
 */

int triangulate_polygon(n, vertices, triangles)
     int n;
     double vertices[][2];
     int triangles[][3];
{
  register int i;
  int nmonpoly;

  memset((void *)seg, 0, sizeof(seg));
  for (i = 1; i <= n; i++)
    {
      seg[i].is_inserted = FALSE;

      seg[i].v0.x = vertices[i][0]; /* x-cood */
      seg[i].v0.y = vertices[i][1]; /* y-cood */
      if (i == 1)
	{
	  seg[n].v1.x = seg[i].v0.x;
	  seg[n].v1.y = seg[i].v0.y;
	}
      else
	{
	  seg[i - 1].v1.x = seg[i].v0.x;
	  seg[i - 1].v1.y = seg[i].v0.y;
	}	
    }
  
  global.nseg = n;
  initialise(n);
  construct_trapezoids(n, seg);
  nmonpoly = monotonate_trapezoids(n);
  triangulate_monotone_polygons(nmonpoly, triangles);
  
  return 0;
}


/* This function returns TRUE or FALSE depending upon whether the 
 * vertex is inside the polygon or not. The polygon must already have
 * been triangulated before this routine is called.
 * This routine will always detect all the points belonging to the 
 * set (polygon-area - polygon-boundary). The return value for points 
 * on the boundary is not consistent!!!
 */

int is_point_inside_polygon(vertex)
     double vertex[2];
{
  point_t v;
  int trnum, rseg;
  trap_t *t;

  v.x = vertex[0];
  v.y = vertex[1];
  
  trnum = locate_endpoint(v, v, 1);
  t = &tr[trnum];
  
  if (t->state == ST_INVALID)
    return FALSE;
  
  if ((t->lseg <= 0) || (t->rseg <= 0))
    return FALSE;
  rseg = t->rseg;
  return _greater_than_equal_to(&seg[rseg].v1, &seg[rseg].v0);
}

#endif
