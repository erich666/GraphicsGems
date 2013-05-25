// -*- C++ -*-
//  plane.cc by George Vanecek Jr., June 1994
//

#include "plane.h"

// Computes the plane equation using Newell's averaging algorithm.
Plane::Plane( const Counter nPoints, const Point points[] )
: n(0.0,0.0,0.0), d(0.0), eps(0.0)
{
  assert( nPoints > 2 );
  Vector avrPnt = Point(0,0,0);
  for( Index i = 0; i < nPoints; ++i ) {
    avrPnt += points[i-1];
    n      += Vector(points[i-1]) ^ Vector(points[i]);
    updateEpsilon( points[i] );
  }
  avrPnt += points[nPoints-1];
  n      += Vector(points[nPoints-1]) ^ Vector(points[0]);
  n.normalize();
  d = normal() * ((-1.0 / nPoints) * avrPnt );
}

// Compute the intersection point with the transversal line (p,q).
Point Plane::onPoint( const Point &p, const Point &q ) const
{
  const Vector v(q - p);
  const double c = normal() * v;
  assert( c != 0.0 );
  const double t = -sDistance(p) / c;
  return p + t * v;
}

void Plane::updateEpsilon ( const Point& p )
{
  double d = sDistance(p);
  if( d < 0.0 )
    d = -d;
  if( d > eps)
    eps = d;
}

