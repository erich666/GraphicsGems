// -*- C++ -*-
// dedge.cc by George Vanecek Jr. June 1994
//

#include <stdlib.h>
#include <assert.h>
#include "dedge.h"

DEdge::DEdge( const Point& srcP, DEdge* const last )
: sP(srcP), nxt(NULL), sPW(NOWHERE)
{
  last->nxt = this;
  prv       = last;
}

void DEdge::closeCycle( DEdge* const first, DEdge* const last )
{
  first->prv = last;
  last->nxt  = first;
}

void DEdge::split( const Point& p )
{
  DEdge* const n = next();
  DEdge* const d = new DEdge( p, this );
  closeCycle( n, d );
}
