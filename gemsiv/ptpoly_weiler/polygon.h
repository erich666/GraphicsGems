#include <stdio.h>

    /* polygon vertex definition */
typedef struct vertex_struct {
  double x,y;			/* coordinate values */
  struct vertex_struct	*next;	/* circular singly linked list from poly */
  } vtx, *vtx_ptr;

    /* polygon definition */
typedef struct polygon_struct {
  vtx_ptr last;		/* pointer to end of circular vertex list */
  } polygon, *polygon_ptr;

    /* return next vertex in polygon list of vertices */
#define polygon_get_vertex(poly, vertex)		    \
    ((vertex == NULL) ? poly->last->next : vertex->next)

    /* create and insert new polygon vertex */
#define polygon_new_vertex(vertex, poly, xx, yy, new_vertex) {	\
    new_vertex	= (vtx_ptr) malloc (sizeof(vtx));	    \
    new_vertex->x = xx;					    \
    new_vertex->y = yy;					    \
    if (poly->last != NULL) {				    \
      new_vertex->next = poly->last->next;		    \
      poly->last->next = new_vertex;			    \
      }							    \
    else {						    \
      new_vertex->next = new_vertex;			    \
      }							    \
    poly->last = new_vertex;				    \
    }

    /* create new polygon */
#define polygon_create(poly)				    \
    poly = (polygon_ptr) malloc(sizeof (polygon));	    \
    poly->last = NULL;
