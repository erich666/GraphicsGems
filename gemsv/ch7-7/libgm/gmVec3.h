// gmVector3.h - 3 element vector class
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#ifndef GMVECTOR3_H
#define GMVECTOR3_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gmUtils.h>

class gmVector3 {

protected:
  double v_[3];

public:
  gmVector3();
  gmVector3(const gmVector3&);
  gmVector3(double, double, double);

  // array access

  double& operator [](int);
  const double& operator [](int) const;

  // assignment

  gmVector3& assign(double, double, double);
  gmVector3& operator =(const gmVector3&);

  // math operators

  gmVector3& operator +=(const gmVector3&);
  gmVector3& operator -=(const gmVector3&);
  gmVector3& operator *=(double);
  gmVector3& operator /=(double);

  gmVector3 operator +(const gmVector3&) const;
  gmVector3 operator -(const gmVector3&) const;
  gmVector3 operator -() const;
  gmVector3 operator *(double) const;
  gmVector3 operator /(double) const;

friend gmVector3 operator *(double, const gmVector3&);

  bool operator ==(const gmVector3&) const;
  bool operator !=(const gmVector3&) const;

  // operations

  gmVector3& clamp(double, double);
  double length() const;
  double lengthSquared() const;
  gmVector3& normalize();

  void copyTo(float [3]) const;
  void copyTo(double [3]) const;

friend gmVector3 cross(const gmVector3&, const gmVector3&);
friend double distance(const gmVector3&, const gmVector3&);
friend double distanceSquared(const gmVector3&, const gmVector3&);
friend double dot(const gmVector3&, const gmVector3&);
friend gmVector3 lerp(double, const gmVector3&, const gmVector3&);

  // output

friend ostream & operator << ( ostream &, const gmVector3 & );
};

// CONSTRUCTORS

inline gmVector3::gmVector3()
{
  v_[0] = v_[1] = v_[2] = 0;
}

inline gmVector3::gmVector3(const gmVector3& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2];
}

inline gmVector3::gmVector3(double x, double y, double z)
{
  v_[0] = x; v_[1] = y; v_[2] = z;
}

// ARRAY ACCESS

inline double& gmVector3::operator [](int i) 
{
  assert(i == 0 || i == 1 || i == 2);
  return v_[i];
}

inline const double& gmVector3::operator [](int i) const
{
  assert(i == 0 || i == 1 || i == 2);
  return v_[i];
}

// ASSIGNMENT

inline gmVector3& gmVector3::assign(double x, double y, double z)
{
  v_[0] = x; v_[1] = y; v_[2] = z;
  return *this;
}

inline gmVector3& gmVector3::operator =(const gmVector3& v)
{
  v_[0] = v[0]; v_[1] = v[1]; v_[2] = v[2];
  return *this;
}

// MATH OPERATORS

inline gmVector3& gmVector3::operator +=(const gmVector3& v)
{
  v_[0] += v[0]; v_[1] += v[1]; v_[2] += v[2];
  return *this;
}

inline gmVector3& gmVector3::operator -=(const gmVector3& v)
{
  v_[0] -= v[0]; v_[1] -= v[1]; v_[2] -= v[2];
  return *this;
}

inline gmVector3& gmVector3::operator *=(double c)
{
  v_[0] *= c; v_[1] *= c; v_[2] *= c;
  return *this;
}

inline gmVector3& gmVector3::operator /=(double c)
{
  assert(!gmIsZero(c));
  v_[0] /= c; v_[1] /= c; v_[2] /= c;
  return *this;
}

inline gmVector3 gmVector3::operator +(const gmVector3& v) const
{
  return gmVector3(v_[0] + v[0], v_[1] + v[1], v_[2] + v[2]);
}

inline gmVector3 gmVector3::operator -(const gmVector3& v) const
{
  return gmVector3(v_[0] - v[0], v_[1] - v[1], v_[2] - v[2]);
}

inline gmVector3 gmVector3::operator -() const
{
  return gmVector3(-v_[0], -v_[1], -v_[2]);
}

inline gmVector3 gmVector3::operator *(double c) const
{
  return gmVector3(v_[0] * c, v_[1] * c, v_[2] * c);
}

inline gmVector3 gmVector3::operator /(double c) const
{
  assert(!gmIsZero(c));
  return gmVector3(v_[0] / c, v_[1] / c, v_[2] / c);
}

inline gmVector3 operator *(double c, const gmVector3& v)
{
  return gmVector3(c * v[0], c * v[1], c * v[2]);
}

inline bool gmVector3::operator ==(const gmVector3& v) const
{
  return
    (gmFuzEQ(v_[0], v[0]) && gmFuzEQ(v_[1], v[1]) && gmFuzEQ(v_[2], v[2]));
}

inline bool gmVector3::operator !=(const gmVector3& v) const
{
  return (!(*this == v));
}

// OPERATIONS

inline gmVector3& gmVector3::clamp(double lo, double hi)
{
  gmClamp(v_[0], lo, hi); gmClamp(v_[1], lo, hi); gmClamp(v_[2], lo, hi);
  return *this;
}

inline double gmVector3::length() const
{
  return sqrt(gmSqr(v_[0]) + gmSqr(v_[1]) + gmSqr(v_[2]));
}

inline double gmVector3::lengthSquared() const
{
  return gmSqr(v_[0]) + gmSqr(v_[1]) + gmSqr(v_[2]);
}

inline gmVector3& gmVector3::normalize()
{
  double len = length();
  assert(!gmIsZero(len));
  *this /= len;
  return *this;
}

inline void gmVector3::copyTo(float f[3]) const
{
  f[0] = v_[0]; f[1] = v_[1]; f[2] = v_[2];
}

inline void gmVector3::copyTo(double f[3]) const
{
  f[0] = v_[0]; f[1] = v_[1]; f[2] = v_[2];
}

inline gmVector3 cross(const gmVector3& v1, const gmVector3& v2)
{
  return gmVector3(v1[1] * v2[2] - v1[2] * v2[1],
                   v1[2] * v2[0] - v1[0] * v2[2],
                   v1[0] * v2[1] - v1[1] * v2[0]);
}

inline double distance(const gmVector3& v1, const gmVector3& v2)
{
  return
    sqrt(gmSqr(v1[0] - v2[0]) + gmSqr(v1[1] - v2[1]) + gmSqr(v1[2] - v2[2]));
}

inline double distanceSquared(const gmVector3& v1, const gmVector3& v2)
{
  return gmSqr(v1[0] - v2[0]) + gmSqr(v1[1] - v2[1]) + gmSqr(v1[2] - v2[2]);
}

inline double dot(const gmVector3& v1, const gmVector3& v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline gmVector3 lerp(double f, const gmVector3& v1, const gmVector3& v2)
{
  return v1 + ((v2 - v1) * f);
}

// OUTPUT

inline ostream & operator << ( ostream& os, const gmVector3& v)
{
  os << "< " << v[0] << " " << v[1] << " " << v[2] << " >";
  return os;
}

#endif 



