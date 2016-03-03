// gmMatrix4.h - 4x4 element matrix class
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#ifndef GMMATRIX4_H
#define GMMATRIX4_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gmUtils.h"

class gmVector3;
class gmVector4;

class gmMatrix4 {

protected:
  float m_[4][4];

public:
  gmMatrix4();
  gmMatrix4(const gmMatrix4&);
  gmMatrix4(float, float, float, float,
  	    float, float, float, float,
 	    float, float, float, float,
	    float, float, float, float);

  // array access

  float* operator [](int);
  const float* operator [](int) const;

  // assignment
  
  gmMatrix4& assign(float, float, float, float,
		    float, float, float, float,
		    float, float, float, float,
		    float, float, float, float);
  gmMatrix4& operator =(const gmMatrix4&);

  // operators

  gmMatrix4& operator +=(const gmMatrix4&);
  gmMatrix4& operator -=(const gmMatrix4&);
  gmMatrix4& operator *=(const gmMatrix4&);
  gmMatrix4& operator *=(float);
  gmMatrix4& operator /=(float);
 
  gmMatrix4 operator +(const gmMatrix4&) const;
  gmMatrix4 operator -(const gmMatrix4&) const;
  gmMatrix4 operator -() const;
  gmMatrix4 operator *(const gmMatrix4&) const;
  gmMatrix4 operator *(float) const;
  gmMatrix4 operator /(float) const;

friend gmMatrix4 operator *(float, const gmMatrix4&);

  bool operator ==(const gmMatrix4&) const;
  bool operator !=(const gmMatrix4&) const;

  gmVector4 operator *(const gmVector4&) const;

friend gmVector4 operator *(const gmVector4&, const gmMatrix4&);

  // operations

  gmMatrix4 inverse() const;
  gmMatrix4 transpose() const;
  gmMatrix4 adjoint() const;
  
  float determinant() const;
  bool isSingular() const;

  gmVector3 transform(const gmVector3&) const;
  
  void copyTo(float [4][4]) const;

  // transformation matrices

  static gmMatrix4 identity();
  static gmMatrix4 rotate(float, const gmVector3& axis);
  static gmMatrix4 scale(float, float, float);
  static gmMatrix4 translate(float, float, float);

  // cubic basis matrices

  static gmMatrix4 bezierBasis();
  static gmMatrix4 bsplineBasis();
  static gmMatrix4 catmullromBasis();
  static gmMatrix4 hermiteBasis();
  
  static gmMatrix4 tensedBSplineBasis(float);
  static gmMatrix4 cardinalBasis(float);
  static gmMatrix4 tauBasis(float, float);
  static gmMatrix4 betaSplineBasis(float, float);

};

// ARRAY ACCESS

inline float* gmMatrix4::operator [](int i)
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return &m_[i][0];
}

inline const float* gmMatrix4::operator [](int i) const
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return &m_[i][0];
}

inline void gmMatrix4::copyTo(float f[4][4]) const
{
  f[0][0] = m_[0][0]; f[0][1] = m_[0][1]; 
  f[0][2] = m_[0][2]; f[0][3] = m_[0][3];
  f[1][0] = m_[1][0]; f[1][1] = m_[1][1]; 
  f[1][2] = m_[1][2]; f[1][3] = m_[1][3];
  f[2][0] = m_[2][0]; f[2][1] = m_[2][1]; 
  f[2][2] = m_[2][2]; f[2][3] = m_[2][3];
  f[3][0] = m_[3][0]; f[3][1] = m_[3][1]; 
  f[3][2] = m_[3][2]; f[3][3] = m_[3][3];
}

#endif // GMMATRIX4_H


