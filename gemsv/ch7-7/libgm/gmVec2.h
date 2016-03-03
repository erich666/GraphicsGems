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
#include "gmUtils.h"

class gmVector2 {

protected:
  float v_[2];

public:
  gmVector2();
  gmVector2(const gmVector2&);
  gmVector2(float, float);

  // array access

  float& operator [](int);
  const float& operator [](int) const;

  // assignment

  gmVector2& assign(float, float);
  gmVector2& operator =(const gmVector2&);

  // math operators

  gmVector2& operator +=(const gmVector2&);
  gmVector2& operator -=(const gmVector2&);
  gmVector2& operator *=(float);
  gmVector2& operator /=(float);

  gmVector2 operator +(const gmVector2&) const;
  gmVector2 operator -(const gmVector2&) const;
  gmVector2 operator -() const;
  gmVector2 operator *(float) const;
  gmVector2 operator /(float) const;

friend gmVector2 operator *(float, const gmVector2&);

  bool operator ==(const gmVector2&) const;
  bool operator !=(const gmVector2&) const;

  // operations

  gmVector2& clamp(float, float);
  float length() const;
  float lengthSquared() const;
  gmVector2& normalize();

  void copyTo(float [2]) const;

friend float distance(const gmVector2&, const gmVector2&);
friend float distanceSquared(const gmVector2&, const gmVector2&);
friend float dot(const gmVector2&, const gmVector2&);
friend gmVector2 lerp(float, const gmVector2&, const gmVector2&);

  // output

friend std::ostream & operator << ( std::ostream &, const gmVector2 & );
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

inline gmVector2::gmVector2(float x, float y)
{
  v_[0] = x; v_[1] = y;
}

// ARRAY ACCESS

inline float& gmVector2::operator [](int i) 
{
  assert(i == 0 || i == 1);
  return v_[i];
}

inline const float& gmVector2::operator [](int i) const
{
  assert(i == 0 || i == 1);
  return v_[i];
}

// ASSIGNMENT

inline gmVector2& gmVector2::assign(float x, float y)
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

inline gmVector2& gmVector2::operator *=(float c)
{
  v_[0] *= c; v_[1] *= c;
  return *this;
}

inline gmVector2& gmVector2::operator /=(float c)
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

inline gmVector2 gmVector2::operator *(float c) const
{
  return gmVector2(v_[0] * c, v_[1] * c);
}

inline gmVector2 gmVector2::operator /(float c) const
{
  assert(!gmIsZero(c));
  return gmVector2(v_[0] / c, v_[1] / c);
}

inline gmVector2 operator *(float c, const gmVector2& v)
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

inline gmVector2& gmVector2::clamp(float lo, float hi)
{
  gmClamp(v_[0], lo, hi); gmClamp(v_[1], lo, hi);
  return *this;
}

inline float gmVector2::length() const
{
  return sqrt(gmSqr(v_[0]) + gmSqr(v_[1]));
}

inline float gmVector2::lengthSquared() const
{
  return gmSqr(v_[0]) + gmSqr(v_[1]);
}

inline gmVector2& gmVector2::normalize()
{
  float len = length();
  assert(!gmIsZero(len));
  *this /= len;
  return *this;
}

inline void gmVector2::copyTo(float f[2]) const
{
  f[0] = v_[0]; f[1] = v_[1];
}

inline float distance(const gmVector2& v1, const gmVector2& v2)
{
  return sqrt(gmSqr(v1[0] - v2[0]) + gmSqr(v1[1] - v2[1]));
}

inline float distanceSquared(const gmVector2& v1, const gmVector2& v2)
{
  return gmSqr(v1[0] - v2[0]) + gmSqr(v1[1] - v2[1]);
}

inline float dot(const gmVector2& v1, const gmVector2& v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1];
}

inline gmVector2 lerp(float f, const gmVector2& v1, const gmVector2& v2)
{
  return v1 + ((v2 - v1) * f);
}

// OUTPUT

inline std::ostream & operator << ( std::ostream& os, const gmVector2& v)
{
  os << "< " << v[0] << " " << v[1] << " >";
  return os;
}

#endif


