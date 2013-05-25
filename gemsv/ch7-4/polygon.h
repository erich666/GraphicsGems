// -*- C++ -*-
// polygon.h by George Vanecek Jr, June 1994

#ifndef _POLYGON_H_
#define _POLYGON_H_

#ifndef _LIST_H_
#include "list.h"
#endif

#ifndef _PLANE_H_
#include "plane.h"
#endif

#ifndef _DEDGE_H_
#include "dedge.h"
#endif

class Polygon {
public:
  Polygon( const Counter nPoints, const Point [] );
  Counter    nPoints( ) const { return nDEdges; }
  const Plane& plane( ) const { return supportPlane; }
  DEdge*       first( ) const { return anchor; }

private:
  Polygon( DEdge* const, const Plane& );
  Where   classifyPoints( const Plane&, Counter&, DEdge*[] );
  void    addBridge     ( DEdge* const, DEdge* const );
  void    complexCut    ( const Plane&, const Counter, DEdge* const [],
			  List<Polygon>&, List<Polygon>& );
  static void sortDEdges( const Counter, DEdge* const [], const Vector& );
  void    maximize      ( DEdge* const );
  void    split         ( const Plane&, DEdge* const );

  const Plane supportPlane;
  Counter     nDEdges;		// Number of DEdges in loop...
  DEdge*      anchor;		// Edge Loop

friend void split( Polygon*&, const Plane& cut,
		   List<Polygon>& above,
		   List<Polygon>& on,
		   List<Polygon>& below );
};

// Juxtapose two strings to form a single identifier:
#define name2(a,b) a##b

// Iterate over all Directed Edges within a Polygon member function:
#define forEachDEdge(d) \
       for( DEdgePtr d = first(), name2(last,d) = NULL;\
	    d != first() || name2(last,d) == NULL;\
	    name2(last,d) = d, d = d->next() )

// Iterate over all Directed Edges of a Polygon *g:
#define forEachDEdgeOfPoly(d,g) \
       for( DEdgePtr d = g->first(), name2(last,d) = NULL;\
	    d != g->first() || name2(last,d) == NULL;\
	    name2(last,d) = d, d = d->next() )
#endif
