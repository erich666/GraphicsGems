// -*- C++ -*-
// vector.h by George Vanecek Jr. June 1994

#ifndef _VECTOR_H_
#define _VECTOR_H_

#ifndef _POINT_H_
#include "point.h"
#endif

class Vector : public Point {
public:
  Vector( const Point&  p ) : Point(p) {}
  Vector( double a, double b, double c ) : Point(a, b, c) {}
  inline double operator * ( const Vector& v ) const;
  inline Vector operator ^ ( const Vector& v ) const;
  void          normalize  ( );
};

inline Vector operator + ( const Point& p, const Point& q )
{ return Point(p.x()+q.x(), p.y()+q.y(), p.z()+q.z()); }

inline Vector operator - ( const Point& p, const Point& q )
{ return Point(p.x()-q.x(), p.y()-q.y(), p.z()-q.z()); }

inline Vector operator * ( const double s, const Vector& v )
{ return Vector( s*v.x(), s*v.y(), s*v.z() ); }

// Vector Dot Product.
inline double Vector::operator * (  const Vector& v ) const
{ return x() * v.x() + y() * v.y() + z() * v.z(); }

// Vector Cross product.
inline Vector Vector::operator ^ ( const Vector& v ) const
{ return Vector(y() * v.z() - z() * v.y(),
		z() * v.x() - x() * v.z(),
		x() * v.y() - y() * v.x());
}

inline void Vector::normalize()
{ 
  const double n = *this * *this;
  assert( n != 0.0 );
  x() /= n;
  y() /= n;
  z() /= n;
}

#endif
