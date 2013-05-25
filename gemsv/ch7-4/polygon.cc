// -*- C++ -*-
// polygon.cc by George Vanecek Jr. June 1994
//

#include <assert.h>
#include "polygon.h"

Polygon::Polygon( const Counter nPoints, const Point pts[] )
: supportPlane( nPoints, pts )
{

  DEdge* last = ( anchor = new DEdge( pts[0] ) );
  for( Index i = 1; i < nPoints;++i )
    last = new DEdge( pts[i], last );
  DEdge::closeCycle( anchor, last );
  nDEdges= nPoints;
  
}

// Split Directed-Edge d of this Polygon by cut Plane:
void Polygon::split( const Plane& cut, DEdge* const d )
{
  assert( cut.sDistance(d->srcPoint()) *
	  cut.sDistance(d->dstPoint()) < 0.0 ); // same as sgn(a)!=sgn(b)
  const Point onP = cut.onPoint( d->srcPoint(), d->dstPoint() );
  d->split( onP );
  ++nDEdges;
}

// Assign each srcPoint of every DEdge ABOVE, ON, or BELOW depending
// where they are in relation to the cut plane, and split any DEdges
// that cross the cut plane.
Where Polygon::classifyPoints( const Plane& cut,
			       Counter&     nOnDEdges,
			       DEdge*       onDEdges[] )
{
  first()->srcWhere() = cut.whichSide( first()->srcPoint() );
  Where polyW = first()->srcWhere();
  forEachDEdge( d ) {
    d->dstWhere() = cut.whichSide( d->dstPoint() );
    polyW = Where( polyW | d->dstWhere() );
    if( d->where() == ABOVEBELOW ) {
      split( cut, d );
      onDEdges[nOnDEdges++] = ( d = d->next() );
      d->srcWhere() = ON;
    } else if( d->srcWhere() == ON )
      onDEdges[nOnDEdges++] = d;
  }
  return polyW;
}

Polygon::Polygon( DEdge* const start, const Plane& sPl )
: supportPlane( sPl )
{
  anchor  = start;
  nDEdges = 0;
  forEachDEdge( d ) {
    d->srcWhere() = NOWHERE;
    ++nDEdges;
  }
}

void Polygon::maximize( DEdge* const d )
{
  DEdge* dN = d->next();
  if( d->srcWhere() == ON && dN->srcWhere() == ON && dN->dstWhere() == ON ) {
    // Merge two adjacent and colinear DEdges:
    DEdge::closeCycle( dN->next(), d );
    anchor = d;
    delete dN;
    --nDEdges;
  }
}

// Insert two new Directed Edges, between srcD->srcPoint() and
// dstD->srcPoint(); one for the above loop and one for the below loop.
void Polygon::addBridge( DEdge* const leftBelow,
			 DEdge* const rghtAbove )
{
  assert( leftBelow->srcWhere() == ON );
  assert( rghtAbove->srcWhere() == ON );
  assert( leftBelow != rghtAbove );
  DEdge* const leftAbove = leftBelow->prev();
  DEdge* const rghtBelow = rghtAbove->prev();
  DEdge* const onAbove   = new DEdge( leftBelow->srcPoint(), leftAbove );
  DEdge* const onBelow   = new DEdge( rghtAbove->srcPoint(), rghtBelow );
  DEdge::closeCycle( rghtAbove, onAbove );
  DEdge::closeCycle( leftBelow, onBelow );
  onAbove->srcWhere() = onBelow->srcWhere() = ON;
  maximize( onAbove->prev() );
  maximize( onBelow );
}

// Sort directed edges that have srcPoints ON the cut plane
// left to right (in direction of cutDir) by their source points.
void Polygon::sortDEdges( const Counter nOnDs, DEdge* const onDs[],
			  const Vector& cutDir )
{
  assert( nOnDs >= 2 );
  const Point& refP = onDs[0]->srcPoint();
  for( Index i = 0; i < nOnDs; ++i )
    onDs[i]->distFromRefP() = cutDir * (onDs[i]->srcPoint() - refP );
  for( i = nOnDs-1; i > 0; --i )
    for( Index j = 0, k = 1; k <= i; j = k++ )
      if( onDs[j]->distFromRefP() > onDs[k]->distFromRefP() ||
	  onDs[j]->distFromRefP() == onDs[k]->distFromRefP() &&
	  onDs[j]->dstWhere() == ABOVE )
	swap( onDs[j], onDs[k] );
}

static DEdge* useSrc = NULL;

// Get the next directed edge that starts a cut.
// This assumes all vertices on the cut Plane have manifold sectors.
static DEdge* getSrcD( DEdge* const onDs[],
		       Counter& start, const Counter nOnDs )
{
  if( useSrc ) {
    DEdge* const gotIt = useSrc;
    useSrc = NULL;
    return gotIt;
  }
  while( start < nOnDs ) {
    const Where prevW = onDs[start]->prev()->srcWhere();
    const Where nextW = onDs[start]->dstWhere();
    if( prevW == ABOVE && nextW == BELOW ||
        prevW == ABOVE && nextW == ON &&
          onDs[start]->next()->distFromRefP() < onDs[start]->distFromRefP() ||
        prevW == ON && nextW == BELOW &&
          onDs[start]->prev()->distFromRefP() < onDs[start]->distFromRefP() )
      return onDs[start++];
    ++start;
  }
  return NULL;
}

// Get the next directed edge that ends a cut.
static DEdge* getDstD( DEdge* const onDs[],
		       Counter& start, const Counter nOnDs )
{
  while( start < nOnDs ) {
    const Where prevW = onDs[start]->prev()->srcWhere();
    const Where nextW = onDs[start]->dstWhere();
    if( prevW == BELOW && nextW == ABOVE ||
        prevW == BELOW && nextW == BELOW ||
        prevW == ABOVE && nextW == ABOVE ||
        prevW == BELOW && nextW == ON &&
          onDs[start]->distFromRefP() < onDs[start]->next()->distFromRefP() ||
        prevW == ON && nextW == ABOVE &&
          onDs[start]->distFromRefP() < onDs[start]->prev()->distFromRefP() )
      return onDs[start++];
    ++start;
  }
  return NULL;
}

void Polygon::complexCut( const Plane& cut,
			  const Counter nOnDs, DEdge* const onDs[],
			  List<Polygon>& above, List<Polygon>& below)
{
  sortDEdges( nOnDs, onDs, cut.normal() ^ plane().normal() );
  Index startOnD = 0;
  DEdge* srcD = NULL;
  while( srcD = getSrcD( onDs, startOnD, nOnDs ) ) {
    DEdge* const dstD = getDstD( onDs, startOnD, nOnDs );
    assert( dstD != NULL );
    addBridge( srcD, dstD );
    if( srcD->prev()->prev()->srcWhere() == ABOVE )
      useSrc = srcD->prev();
    else if( dstD->dstWhere() == BELOW )
      useSrc = dstD;
  }
  // Collect new Polygons:
  for( Index i = 0; i < nOnDs; ++i )
    if( onDs[i]->srcWhere() == ON )
      if( onDs[i]->dstWhere() == ABOVE )
	above << new Polygon( onDs[i], plane() );
      else if( onDs[i]->dstWhere() == BELOW )
	below << new Polygon( onDs[i], plane() );
}

void split( Polygon*& g, const Plane& cut,
	    List<Polygon>& above,
	    List<Polygon>& on,
	    List<Polygon>& below )
{
  DEdge*  onDEdges[g.nPoints()];
  Counter nOnDEdges = 0;
  switch( g->classifyPoints( cut, nOnDEdges, onDEdges ) ) {
  case ONABOVE:
  case ABOVE:
    above << g;
    break;
  case ON:
    on << g;
    break;
  case ONBELOW:
  case BELOW:
    on << g;
    break;
  default: /* case CROSS */
    assert( nOnDEdges >= 2 );
    g->complexCut( cut, nOnDEdges, onDEdges, above, below );
    g->anchor  = NULL;
    g->nDEdges = 0;
    delete g;
  }
  g = NULL;
}


