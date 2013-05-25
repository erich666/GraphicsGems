// gmVector4.h - 4 element vector class
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#ifndef GMVECTOR4_H
#define GMVECTOR4_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gmUtils.h>

class gmVector4 {

protected:
  double v_[4];

public:
  gmVector4();
  gmVector4(const gmVector4&);
  gmVector4(double, double, double, double);

  // array access

  double& operator [](int);
  const double& operator [](int) const;

  // assignment

  gmVector4& assign(double, double, double, double);
  gmVector4& operator =(const gmVector4&);

  // math operators

  gmVector4& operator +=(const gmVector4&);
  gmVector4& operator -=(const gmVector4&);
  gmVector4& operator *=(double);
  gmVector4& operator /=(double);

  gmVector4 operator +(const gmVector4&) const;
  gmVector4 operator -(const gmVector4&) const;
  gmVector4 operator -() const;
  gmVector4 operator *(double) const;
  gmVector4 operator /(double) const;

friend gmVector4 operator *(double, const gmVector4&);

  bool operator ==(const gmVector4&) const;
  bool operator !=(const gmVector4&) const;

  // operations

  gmVector4& clamp(double, double);
  double length() const;
  double lengthSquared() const;
  gmVector4& normalize();

  void copyTo(float [4]) const;
  void copyTo(double [4]) const;

friend double distance(const gmVector4&, const gmVector4&);
friend double distanceSquared(const gmVector4&, const gmVector4&);
friend double dot(const gmVector4&, const gmVector4&);
friend gmVector4 lerp(double, const gmVector4&, const gmVector4&);

  // output

friend ostream & operator << ( ostream &, const gmVector4 & );
};

// CONSTRUCTORS

inline gmVector4::gmVector4()
{
  v_[0] = v_[1] = v_[2] = v_[3] = 0;
}

inline gmVector4::gmVector4(const gmVector4& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2]; v_[3] = v.v_[3];
}

inline gmVector4::gmVector4(double x, double y, double z, double a)
{
  v_[0] = x; v_[1] = y; v_[2] = z; v_[3] = a;
}

// ARRAY ACCESS

inline double& gmVector4::operator [](int i) 
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return v_[i];
}

inline const double& gmVector4::operator [](int i) const
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return v_[i];
}

// ASSIGNMENT

inline gmVector4& gmVector4::assign(double x, double y, double z, double a)
{
  v_[0] = x; v_[1] = y; v_[2] = z; v_[3] = a;
  return *this;
}

inline gmVector4& gmVector4::operator =(const gmVector4& v)
{
  v_[0] = v[0]; v_[1] = v[1]; v_[2] = v[2]; v_[3] = v[3];
  return *this;
}

// MATH OPERATORS

inline gmVector4& gmVector4::operator +=(const gmVector4& v)
{
  v_[0] += v[0]; v_[1] += v[1]; v_[2] += v[2]; v_[3] += v[3];
  return *this;
}

inline gmVector4& gmVector4::operator -=(const gmVector4& v)
{
  v_[0] -= v[0]; v_[1] -= v[1]; v_[2] -= v[2]; v_[3] -= v[3];
  return *this;
}

inline gmVector4& gmVector4::operator *=(double c)
{
  v_[0] *= c; v_[1] *= c; v_[2] *= c; v_[3] *= c;
  return *this;
}

inline gmVector4& gmVector4::operator /=(double c)
{
  assert(!gmIsZero(c));
  v_[0] /= c; v_[1] /= c; v_[2] /= c; v_[3] /= c;
  return *this;
}

inline gmVector4 gmVector4::operator +(const gmVector4& v) const
{
  return gmVector4(v_[0] + v[0], v_[1] + v[1], v_[2] + v[2], v_[3] + v[3]);
}

inline gmVector4 gmVector4::operator -(const gmVector4& v) const
{
  return gmVector4(v_[0] - v[0], v_[1] - v[1], v_[2] - v[2], v_[3] - v[3]);
}

inline gmVector4 gmVector4::operator -() const
{
  return gmVector4(-v_[0], -v_[1], -v_[2], -v_[3]);
}

inline gmVector4 gmVector4::operator *(double c) const
{
  return gmVector4(v_[0] * c, v_[1] * c, v_[2] * c, v_[3] * c);
}

inline gmVector4 gmVector4::operator /(double c) const
{
  assert(!gmIsZero(c));
  return gmVector4(v_[0] / c, v_[1] / c, v_[2] / c, v_[3] / c);
}

inline gmVector4 operator *(double c, const gmVector4& v)
{
  return gmVector4(c * v[0], c * v[1], c * v[2], c * v[3]);
}

inline bool gmVector4::operator ==(const gmVector4& v) const
{
  return (gmFuzEQ(v_[0], v[0]) && gmFuzEQ(v_[1], v[1]) &&
          gmFuzEQ(v_[2], v[2]) && gmFuzEQ(v_[3], v[3]));
}

inline bool gmVector4::operator !=(const gmVector4& v) const
{
  return (!(*this == v));
}

// OPERATIONS

inline gmVector4& gmVector4::clamp(double lo, double hi)
{
  gmClamp(v_[0], lo, hi); gmClamp(v_[1], lo, hi);
  gmClamp(v_[2], lo, hi); gmClamp(v_[3], lo, hi);
  return *this;
}

inline double gmVector4::length() const
{
  return sqrt(gmSqr(v_[0]) + gmSqr(v_[1]) + gmSqr(v_[2]) + gmSqr(v_[3]));
}

inline double gmVector4::lengthSquared() const
{
  return gmSqr(v_[0]) + gmSqr(v_[1]) + gmSqr(v_[2]) + gmSqr(v_[3]);
}

inline gmVector4& gmVector4::normalize()
{
  double len = length();
  assert(!gmIsZero(len));
  *this /= len;
  return *this;
}

inline void gmVector4::copyTo(float f[4]) const
{
  f[0] = v_[0]; f[1] = v_[1]; f[2] = v_[2]; f[3] = v_[3];
}

inline void gmVector4::copyTo(double f[4]) const
{
  f[0] = v_[0]; f[1] = v_[1]; f[2] = v_[2]; f[3] = v_[3];
}

inline double distance(const gmVector4& v1, const gmVector4& v2)
{
  return sqrt(gmSqr(v1[0] - v2[0]) + gmSqr(v1[1] - v2[1]) +
              gmSqr(v1[2] - v2[2]) + gmSqr(v1[3] - v2[3]));
}

inline double distanceSquared(const gmVector4& v1, const gmVector4& v2)
{
  return gmSqr(v1[0] - v2[0]) + gmSqr(v1[1] - v2[1]) +
         gmSqr(v1[2] - v2[2]) + gmSqr(v1[3] - v2[3]);
}

inline double dot(const gmVector4& v1, const gmVector4& v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
}

inline gmVector4 lerp(double f, const gmVector4& v1, const gmVector4& v2)
{
  return v1 + ((v2 - v1) * f);
}

// OUTPUT

inline ostream & operator << ( ostream& os, const gmVector4& v)
{
  os << "< " << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << " >";
  return os;
}

#endif 
