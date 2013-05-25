// gmVector2.h - 2 element vector class
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#ifndef GMVECTOR2_H
#define GMVECTOR2_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gmUtils.h>

class gmVector2 {

protected:
  double v_[2];

public:
  gmVector2();
  gmVector2(const gmVector2&);
  gmVector2(double, double);

  // array access

  double& operator [](int);
  const double& operator [](int) const;

  // assignment

  gmVector2& assign(double, double);
  gmVector2& operator =(const gmVector2&);

  // math operators

  gmVector2& operator +=(const gmVector2&);
  gmVector2& operator -=(const gmVector2&);
  gmVector2& operator *=(double);
  gmVector2& operator /=(double);

  gmVector2 operator +(const gmVector2&) const;
  gmVector2 operator -(const gmVector2&) const;
  gmVector2 operator -() const;
  gmVector2 operator *(double) const;
  gmVector2 operator /(double) const;

friend gmVector2 operator *(double, const gmVector2&);

  bool operator ==(const gmVector2&) const;
  bool operator !=(const gmVector2&) const;

  // operations

  gmVector2& clamp(double, double);
  double length() const;
  double lengthSquared() const;
  gmVector2& normalize();

  void copyTo(float [2]) const;
  void copyTo(double [2]) const;

friend double distance(const gmVector2&, const gmVector2&);
friend double distanceSquared(const gmVector2&, const gmVector2&);
friend double dot(const gmVector2&, const gmVector2&);
friend gmVector2 lerp(double, const gmVector2&, const gmVector2&);

  // output

friend ostream & operator << ( ostream &, const gmVector2 & );
};

// CONSTRUCTORS

inline gmVector2::gmVector2()
{
  v_[0] = v_[1] = 0;
}

inline gmVector2::gmVector2(const gmVector2& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1];
}

inline gmVector2::gmVector2(double x, double y)
{
  v_[0] = x; v_[1] = y;
}

// ARRAY ACCESS

inline double& gmVector2::operator [](int i) 
{
  assert(i == 0 || i == 1);
  return v_[i];
}

inline const double& gmVector2::operator [](int i) const
{
  assert(i == 0 || i == 1);
  return v_[i];
}

// ASSIGNMENT

inline gmVector2& gmVector2::assign(double x, double y)
{
  v_[0] = x; v_[1] = y;
  return *this;
}

inline gmVector2& gmVector2::operator =(const gmVector2& v)
{
  v_[0] = v[0]; v_[1] = v[1];
  return *this;
}

// MATH OPERATORS

inline gmVector2& gmVector2::operator +=(const gmVector2& v)
{
  v_[0] += v[0]; v_[1] += v[1];
  return *this;
}

inline gmVector2& gmVector2::operator -=(const gmVector2& v)
{
  v_[0] -= v[0]; v_[1] -= v[1];
  return *this;
}

inline gmVector2& gmVector2::operator *=(double c)
{
  v_[0] *= c; v_[1] *= c;
  return *this;
}

inline gmVector2& gmVector2::operator /=(double c)
{
  assert(!gmIsZero(c));
  v_[0] /= c; v_[1] /= c;
  return *this;
}

inline gmVector2 gmVector2::operator +(const gmVector2& v) const
{
  return gmVector2(v_[0] + v[0], v_[1] + v[1]);
}

inline gmVector2 gmVector2::operator -(const gmVector2& v) const
{
  return gmVector2(v_[0] - v[0], v_[1] - v[1]);
}

inline gmVector2 gmVector2::operator -() const
{
  return gmVector2(-v_[0], -v_[1]);
}

inline gmVector2 gmVector2::operator *(double c) const
{
  return gmVector2(v_[0] * c, v_[1] * c);
}

inline gmVector2 gmVector2::operator /(double c) const
{
  assert(!gmIsZero(c));
  return gmVector2(v_[0] / c, v_[1] / c);
}

inline gmVector2 operator *(double c, const gmVector2& v)
{
  return gmVector2(c * v[0], c * v[1]);
}

inline bool gmVector2::operator ==(const gmVector2& v) const
{
  return (gmFuzEQ(v_[0], v[0]) && gmFuzEQ(v_[1], v[1]));
}

inline bool gmVector2::operator !=(const gmVector2& v) const
{
  return (!(*this == v));
}

// OPERATIONS

inline gmVector2& gmVector2::clamp(double lo, double hi)
{
  gmClamp(v_[0], lo, hi); gmClamp(v_[1], lo, hi);
  return *this;
}

inline double gmVector2::length() const
{
  return sqrt(gmSqr(v_[0]) + gmSqr(v_[1]));
}

inline double gmVector2::lengthSquared() const
{
  return gmSqr(v_[0]) + gmSqr(v_[1]);
}

inline gmVector2& gmVector2::normalize()
{
  double len = length();
  assert(!gmIsZero(len));
  *this /= len;
  return *this;
}

inline void gmVector2::copyTo(float f[2]) const
{
  f[0] = v_[0]; f[1] = v_[1];
}

inline void gmVector2::copyTo(double f[2]) const
{
  f[0] = v_[0]; f[1] = v_[1];
}

inline double distance(const gmVector2& v1, const gmVector2& v2)
{
  return sqrt(gmSqr(v1[0] - v2[0]) + gmSqr(v1[1] - v2[1]));
}

inline double distanceSquared(const gmVector2& v1, const gmVector2& v2)
{
  return gmSqr(v1[0] - v2[0]) + gmSqr(v1[1] - v2[1]);
}

inline double dot(const gmVector2& v1, const gmVector2& v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1];
}

inline gmVector2 lerp(double f, const gmVector2& v1, const gmVector2& v2)
{
  return v1 + ((v2 - v1) * f);
}

// OUTPUT

inline ostream & operator << ( ostream& os, const gmVector2& v)
{
  os << "< " << v[0] << " " << v[1] << " >";
  return os;
}

#endif


