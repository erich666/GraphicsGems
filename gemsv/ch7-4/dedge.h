// -*- C++ -*-
// dedge.h by George Vanecek Jr, June 1994

#ifndef _DEDGE_H_
#define _DEDGE_H_

#ifndef _POINT_H_
#include "point.h"
#endif

class DEdge {			// Directed Edge
public:
  DEdge ( const Point& srcP )
  : sP(srcP), nxt(this), prv(this), sPW(NOWHERE) { }
  DEdge*           next( ) const { return nxt; }
  DEdge*           prev( ) const { return prv; }
  const Point& srcPoint( ) const { return sP; }
  const Point& dstPoint( ) const { return nxt->sP; }
  Where&       srcWhere( ) { return sPW; }
  Where&       dstWhere( ) { return nxt->sPW; }
  Where           where( ) const { return Where( sPW | nxt->sPW ); }
  double&  distFromRefP( ) { return t; }
  
private:
  DEdge ( const Point& srcP, DEdge* const last );
  static void closeCycle( DEdge* const first, DEdge* const last );
  void        split     ( const Point& );

  DEdge*      nxt;		// Next DEdge on cycle
  DEdge*      prv;		// Previous DEdge on cycle
  const Point sP;		// Source Point
  Where       sPW;		// Where is Source Point?
  double      t;		// Related to sP. Used in complexCut(...)

friend class Polygon;
};

typedef DEdge* DEdgePtr;

#endif
