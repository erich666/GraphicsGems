// -*- C++ -*-
//
// plane.h by George Vanecek Jr. June 1994

#ifndef _PLANE_H_
#define _PLANE_H_

#ifndef _VECTOR_H_
#include "vector.h"
#endif

// Provide a minimal 3D plane support sufficine for the Gem.
//
// Any point p that is known topologically to lie on a plane pN+d~0
// is included in the plane by enlarging the epsilon, so the
// equation |pN+d| <= eps holds.  The point/plane relationship must
// be established by the application code.

class Plane
{
public:
  Plane ( const Counter nPoints, const Point[] );
  Plane ( const Vector& v, double x ) : n(v), d(x), eps(0.0) { }
  Plane ( const Plane& pl ) : n(pl.n), d(pl.d), eps(pl.eps) { }

  const Vector& normal( ) const { return n; }

  // The point is topologically known to lie in the plane, so check
  // and update epsilon accordingly:
  void   updateEpsilon( const Point& );
  
  // Signed distance from the point to the plane.
  double      sDistance( const Point& p ) const { return Vector(p) * n + d; } 

  // Intersection point between this Plane and transversal line.
  Point        onPoint( const Point&, const Point& ) const;

  // Which side of the plane is this point on?
  Where      whichSide( const Point& ) const;

private:
  Vector n;			// unit normal vector
  double d;			// shortest distance from origin
  double eps;			// point/plane distance epsilon
};

inline Where Plane::whichSide( const Point& p ) const
{ const double d = sDistance( p );
  return d < -eps ? BELOW : (d > eps ? ABOVE : ON);
}

#endif
