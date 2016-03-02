// gmMatrix3.h - 3x3 element matrix class
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#ifndef GMMATRIX3_H
#define GMMATRIX3_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gmUtils.h"

class gmVector2;
class gmVector3;

class gmMatrix3 {

protected:
  float m_[3][3];

public:
  gmMatrix3();
  gmMatrix3(const gmMatrix3&);
  gmMatrix3(float, float, float,
  	    float, float, float,
            float, float, float);

  // array access

  float* operator [](int);
  const float* operator [](int) const;

  // assignment
  
  gmMatrix3& assign(float, float, float,
		    float, float, float,
                    float, float, float);
  gmMatrix3& operator =(const gmMatrix3&);

  // operators

  gmMatrix3& operator +=(const gmMatrix3&);
  gmMatrix3& operator -=(const gmMatrix3&);
  gmMatrix3& operator *=(const gmMatrix3&);
  gmMatrix3& operator *=(float);
  gmMatrix3& operator /=(float);
 
  gmMatrix3 operator +(const gmMatrix3&) const;
  gmMatrix3 operator -(const gmMatrix3&) const;
  gmMatrix3 operator -() const;
  gmMatrix3 operator *(const gmMatrix3&) const;
  gmMatrix3 operator *(float) const;
  gmMatrix3 operator /(float) const;

friend gmMatrix3 operator *(float, const gmMatrix3&);

  bool operator ==(const gmMatrix3&) const;
  bool operator !=(const gmMatrix3&) const;

  gmVector3 operator *(const gmVector3&) const;

friend gmVector3 operator *(const gmVector3&, const gmMatrix3&);

  // operations

  gmMatrix3 inverse() const;
  gmMatrix3 transpose() const;
  gmMatrix3 adjoint() const;
  
  float determinant() const;
  bool isSingular() const;

  gmVector2 transform(const gmVector2&) const;
  
  void copyTo(float [3][3]) const;

  // transformation matrices

  static gmMatrix3 identity();
  static gmMatrix3 rotate(float);
  static gmMatrix3 scale(float, float);
  static gmMatrix3 translate(float, float);
};

// ARRAY ACCESS

inline float* gmMatrix3::operator [](int i)
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return &m_[i][0];
}

inline const float* gmMatrix3::operator [](int i) const
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return &m_[i][0];
}

inline void gmMatrix3::copyTo(float f[3][3]) const
{
  f[0][0] = m_[0][0]; f[0][1] = m_[0][1]; f[0][2] = m_[0][2];
  f[1][0] = m_[1][0]; f[1][1] = m_[1][1]; f[1][2] = m_[1][2];
  f[2][0] = m_[2][0]; f[2][1] = m_[2][1]; f[2][2] = m_[2][2];
}

#endif


