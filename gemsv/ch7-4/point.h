// -*- C++ -*-
// point.h by George Vanecek Jr. June 1994

#ifndef _POINT_H_
#define _POINT_H_

#ifndef _BASIC_H_
  #include "basic.h"
#endif

class Point
{
public:
  Point ( const double x, const double y, const double z)
    : _x(x), _y(y), _z(z) { }

  double x() const { return _x; }
  double y() const { return _y; }
  double z() const { return _z; }
  Point&  operator +=( const Point& p);

protected:
  double  _x, _y, _z;		// Point Coordinates
  double& x() { return _x; }
  double& y() { return _y; }
  double& z() { return _z; }
};

inline Point& Point::operator +=( const Point& p )
{ x() += p.x(), y() += p.y(), z() += p.z(); return *this; }

inline ostream& operator << ( ostream& outs, const Point& p )
{
  outs << '(' << p.x() << ' ' << p.y() << ' ' << p.z() << ')';
  return outs;
}
     

#endif


