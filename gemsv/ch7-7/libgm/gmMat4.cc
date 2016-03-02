// gmMatrix4.cc - 4x4 element matrix class
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#include "gmMat4.h"
#include "gmVec3.h"
#include "gmVec4.h"

// private function: RCD
// - dot product of row i of matrix A and row j of matrix B

inline float RCD(const gmMatrix4& A, const gmMatrix4& B, int i, int j)
{
  return A[i][0] * B[0][j] + A[i][1] * B[1][j] + A[i][2] * B[2][j] +
         A[i][3] * B[3][j];
}

// private function: MINOR
// - MINOR of M[r][c]; r in {0,1,2,3}-{r0,r1,r2}; c in {0,1,2,3}-{c0,c1,c2}

inline float MINOR(const gmMatrix4& M,
                    int r0, int r1, int r2, int c0, int c1, int c2)
{
  return M[r0][c0] * (M[r1][c1] * M[r2][c2] - M[r2][c1] * M[r1][c2]) -
	 M[r0][c1] * (M[r1][c0] * M[r2][c2] - M[r2][c0] * M[r1][c2]) +
	 M[r0][c2] * (M[r1][c0] * M[r2][c1] - M[r2][c0] * M[r1][c1]);
}

// CONSTRUCTORS

gmMatrix4::gmMatrix4()
{
  assign(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
}

gmMatrix4::gmMatrix4(const gmMatrix4& M)
{
  assign(M[0][0], M[0][1], M[0][2], M[0][3],
	 M[1][0], M[1][1], M[1][2], M[1][3],
	 M[2][0], M[2][1], M[2][2], M[2][3],
	 M[3][0], M[3][1], M[3][2], M[3][3]);
}

gmMatrix4::gmMatrix4(float a00, float a01, float a02, float a03,
		     float a10, float a11, float a12, float a13,
		     float a20, float a21, float a22, float a23,
		     float a30, float a31, float a32, float a33)
{
  assign(a00, a01, a02, a03,
         a10, a11, a12, a13,
	 a20, a21, a22, a23,
	 a30, a31, a32, a33);
}

// ASSIGNMENT

gmMatrix4& gmMatrix4::assign(float a00, float a01, float a02, float a03,
		             float a10, float a11, float a12, float a13,
		             float a20, float a21, float a22, float a23,
		             float a30, float a31, float a32, float a33)
{
  m_[0][0] = a00; m_[0][1] = a01; m_[0][2] = a02; m_[0][3] = a03;
  m_[1][0] = a10; m_[1][1] = a11; m_[1][2] = a12; m_[1][3] = a13;
  m_[2][0] = a20; m_[2][1] = a21; m_[2][2] = a22; m_[2][3] = a23;
  m_[3][0] = a30; m_[3][1] = a31; m_[3][2] = a32; m_[3][3] = a33;
  return *this;
}

gmMatrix4& gmMatrix4::operator =(const gmMatrix4& M)
{
  assign(M[0][0], M[0][1], M[0][2], M[0][3],
	 M[1][0], M[1][1], M[1][2], M[1][3],
	 M[2][0], M[2][1], M[2][2], M[2][3],
	 M[3][0], M[3][1], M[3][2], M[3][3]);
  return *this;
}

////////////////////////////////////////////////////////////////////////////
// OPERATORS

gmMatrix4& gmMatrix4::operator +=(const gmMatrix4& M)
{
  m_[0][0] += M[0][0]; m_[0][1] += M[0][1];
  m_[0][2] += M[0][2]; m_[0][3] += M[0][3];
  
  m_[1][0] += M[1][0]; m_[1][1] += M[1][1];
  m_[1][2] += M[1][2]; m_[1][3] += M[1][3];
  
  m_[2][0] += M[2][0]; m_[2][1] += M[2][1];
  m_[2][2] += M[2][2]; m_[2][3] += M[2][3];
  
  m_[3][0] += M[3][0]; m_[3][1] += M[3][1];
  m_[3][2] += M[3][2]; m_[3][3] += M[3][3];
  return *this;
}

gmMatrix4& gmMatrix4::operator -=(const gmMatrix4& M)
{
  m_[0][0] -= M[0][0]; m_[0][1] -= M[0][1];
  m_[0][2] -= M[0][2]; m_[0][3] -= M[0][3];
  
  m_[1][0] -= M[1][0]; m_[1][1] -= M[1][1];
  m_[1][2] -= M[1][2]; m_[1][3] -= M[1][3];
  
  m_[2][0] -= M[2][0]; m_[2][1] -= M[2][1];
  m_[2][2] -= M[2][2]; m_[2][3] -= M[2][3];
  
  m_[3][0] -= M[3][0]; m_[3][1] -= M[3][1];
  m_[3][2] -= M[3][2]; m_[3][3] -= M[3][3];
  return *this;
}

gmMatrix4& gmMatrix4::operator *=(const gmMatrix4& M)
{
  assign(RCD(*this, M, 0, 0), RCD(*this, M, 0, 1), 
	 RCD(*this, M, 0, 2), RCD(*this, M, 0, 3),
	 RCD(*this, M, 1, 0), RCD(*this, M, 1, 1),
	 RCD(*this, M, 1, 2), RCD(*this, M, 1, 3),
	 RCD(*this, M, 2, 0), RCD(*this, M, 2, 1),
	 RCD(*this, M, 2, 2), RCD(*this, M, 2, 3),
	 RCD(*this, M, 3, 0), RCD(*this, M, 3, 1),
	 RCD(*this, M, 3, 2), RCD(*this, M, 3, 3));
  return *this;
}

gmMatrix4& gmMatrix4::operator *=(float d)
{
  m_[0][0] *= d; m_[0][1] *= d; m_[0][2] *= d; m_[0][3] *= d;
  m_[1][0] *= d; m_[1][1] *= d; m_[1][2] *= d; m_[1][3] *= d;
  m_[2][0] *= d; m_[2][1] *= d; m_[2][2] *= d; m_[2][3] *= d;
  m_[3][0] *= d; m_[3][1] *= d; m_[3][2] *= d; m_[3][3] *= d;
  return *this;
}

gmMatrix4& gmMatrix4::operator /=(float d)
{
  float di = 1 / d;
  m_[0][0] *= di; m_[0][1] *= di; m_[0][2] *= di; m_[0][3] *= di;
  m_[1][0] *= di; m_[1][1] *= di; m_[1][2] *= di; m_[1][3] *= di;
  m_[2][0] *= di; m_[2][1] *= di; m_[2][2] *= di; m_[2][3] *= di;
  m_[3][0] *= di; m_[3][1] *= di; m_[3][2] *= di; m_[3][3] *= di;
  return *this;
}

gmMatrix4 gmMatrix4::operator +(const gmMatrix4& M) const
{
  return gmMatrix4(m_[0][0] + M[0][0], m_[0][1] + M[0][1],
		   m_[0][2] + M[0][2], m_[0][3] + M[0][3],
		   m_[1][0] + M[1][0], m_[1][1] + M[1][1],
		   m_[1][2] + M[1][2], m_[1][3] + M[1][3],
		   m_[2][0] + M[2][0], m_[2][1] + M[2][1],
		   m_[2][2] + M[2][2], m_[2][3] + M[2][3],
		   m_[3][0] + M[3][0], m_[3][1] + M[3][1],
		   m_[3][2] + M[3][2], m_[3][3] + M[3][3]);
}

gmMatrix4 gmMatrix4::operator -(const gmMatrix4& M) const
{
  return gmMatrix4(m_[0][0] - M[0][0], m_[0][1] - M[0][1],
		   m_[0][2] - M[0][2], m_[0][3] - M[0][3],
		   m_[1][0] - M[1][0], m_[1][1] - M[1][1],
		   m_[1][2] - M[1][2], m_[1][3] - M[1][3],
		   m_[2][0] - M[2][0], m_[2][1] - M[2][1],
		   m_[2][2] - M[2][2], m_[2][3] - M[2][3],
		   m_[3][0] - M[3][0], m_[3][1] - M[3][1],
		   m_[3][2] - M[3][2], m_[3][3] - M[3][3]);
}

gmMatrix4 gmMatrix4::operator -() const
{
  return gmMatrix4(-m_[0][0], -m_[0][1], -m_[0][2], -m_[0][3],
		   -m_[1][0], -m_[1][1], -m_[1][2], -m_[1][3],
		   -m_[2][0], -m_[2][1], -m_[2][2], -m_[2][3],
		   -m_[3][0], -m_[3][1], -m_[3][2], -m_[3][3]);
}

gmMatrix4 gmMatrix4::operator *(const gmMatrix4& M) const
{
  return gmMatrix4(RCD(*this, M, 0, 0), RCD(*this, M, 0, 1), 
		   RCD(*this, M, 0, 2), RCD(*this, M, 0, 3),
		   RCD(*this, M, 1, 0), RCD(*this, M, 1, 1),
		   RCD(*this, M, 1, 2), RCD(*this, M, 1, 3),
		   RCD(*this, M, 2, 0), RCD(*this, M, 2, 1),
		   RCD(*this, M, 2, 2), RCD(*this, M, 2, 3),
		   RCD(*this, M, 3, 0), RCD(*this, M, 3, 1),
		   RCD(*this, M, 3, 2), RCD(*this, M, 3, 3));
}

gmMatrix4 gmMatrix4::operator *(float d) const
{
  return gmMatrix4(m_[0][0] * d, m_[0][1] * d, m_[0][2] * d, m_[0][3] * d,
		   m_[1][0] * d, m_[1][1] * d, m_[1][2] * d, m_[1][3] * d,
		   m_[2][0] * d, m_[2][1] * d, m_[2][2] * d, m_[2][3] * d,
		   m_[3][0] * d, m_[3][1] * d, m_[3][2] * d, m_[3][3] * d);
}

gmMatrix4 gmMatrix4::operator /(float d) const
{
  assert(!gmIsZero(d));
  float di = 1 / d;
  return gmMatrix4(m_[0][0] * di, m_[0][1] * di, m_[0][2] * di, m_[0][3] * di,
		   m_[1][0] * di, m_[1][1] * di, m_[1][2] * di, m_[1][3] * di,
		   m_[2][0] * di, m_[2][1] * di, m_[2][2] * di, m_[2][3] * di,
		   m_[3][0] * di, m_[3][1] * di, m_[3][2] * di, m_[3][3] * di);
}

gmMatrix4 operator *(float d, const gmMatrix4& M)
{
  return gmMatrix4(M[0][0] * d, M[0][1] * d, M[0][2] * d, M[0][3] * d,
		   M[1][0] * d, M[1][1] * d, M[1][2] * d, M[1][3] * d,
		   M[2][0] * d, M[2][1] * d, M[2][2] * d, M[2][3] * d,
		   M[3][0] * d, M[3][1] * d, M[3][2] * d, M[3][3] * d);
}

bool gmMatrix4::operator ==(const gmMatrix4& M) const
{
  return (gmFuzEQ(m_[0][0], M[0][0]) && gmFuzEQ(m_[0][1], M[0][1]) &&
	  gmFuzEQ(m_[0][2], M[0][2]) && gmFuzEQ(m_[0][3], M[0][3]) &&
	 
	  gmFuzEQ(m_[1][0], M[1][0]) && gmFuzEQ(m_[1][1], M[1][1]) &&
	  gmFuzEQ(m_[1][2], M[1][2]) && gmFuzEQ(m_[1][3], M[1][3]) &&

 	  gmFuzEQ(m_[2][0], M[2][0]) && gmFuzEQ(m_[2][1], M[2][1]) &&
	  gmFuzEQ(m_[2][2], M[2][2]) && gmFuzEQ(m_[2][3], M[2][3]) &&

	  gmFuzEQ(m_[3][0], M[3][0]) && gmFuzEQ(m_[3][1], M[3][1]) &&
	  gmFuzEQ(m_[3][2], M[3][2]) && gmFuzEQ(m_[3][3], M[3][3]));
}

bool gmMatrix4::operator !=(const gmMatrix4& M) const
{
  return (!(*this == M));
}

gmVector4 gmMatrix4::operator *(const gmVector4& v) const
{
  return gmVector4(
    m_[0][0] * v[0] + m_[0][1] * v[1] + m_[0][2] * v[2] + m_[0][3] * v[3],
    m_[1][0] * v[0] + m_[1][1] * v[1] + m_[1][2] * v[2] + m_[1][3] * v[3],
    m_[2][0] * v[0] + m_[2][1] * v[1] + m_[2][2] * v[2] + m_[2][3] * v[3],
    m_[3][0] * v[0] + m_[3][1] * v[1] + m_[3][2] * v[2] + m_[3][3] * v[3]);
}

gmVector4 operator *(const gmVector4& v, const gmMatrix4& M)
{
  return gmVector4(
    v[0] * M[0][0] + v[1] * M[1][0] + v[2] * M[2][0] + v[3] * M[3][0],
    v[0] * M[0][1] + v[1] * M[1][1] + v[2] * M[2][1] + v[3] * M[3][1],
    v[0] * M[0][2] + v[1] * M[1][2] + v[2] * M[2][2] + v[3] * M[3][2],
    v[0] * M[0][3] + v[1] * M[1][3] + v[2] * M[2][3] + v[3] * M[3][3]);
}

// OPERATIONS

gmMatrix4 gmMatrix4::transpose() const
{
  return gmMatrix4(m_[0][0], m_[1][0], m_[2][0], m_[3][0],
		   m_[0][1], m_[1][1], m_[2][1], m_[3][1],
		   m_[0][2], m_[1][2], m_[2][2], m_[3][2],
		   m_[0][3], m_[1][3], m_[2][3], m_[3][3]);
}

gmMatrix4 gmMatrix4::inverse() const
{
  assert(!isSingular());
  return adjoint() * gmInv(determinant());
}

gmMatrix4 gmMatrix4::adjoint() const
{
  gmMatrix4 A;
  
  A[0][0] =  MINOR(*this, 1, 2, 3, 1, 2, 3);
  A[0][1] = -MINOR(*this, 0, 2, 3, 1, 2, 3);
  A[0][2] =  MINOR(*this, 0, 1, 3, 1, 2, 3);
  A[0][3] = -MINOR(*this, 0, 1, 2, 1, 2, 3);
  A[1][0] = -MINOR(*this, 1, 2, 3, 0, 2, 3);
  A[1][1] =  MINOR(*this, 0, 2, 3, 0, 2, 3);
  A[1][2] = -MINOR(*this, 0, 1, 3, 0, 2, 3);
  A[1][3] =  MINOR(*this, 0, 1, 2, 0, 2, 3);
  A[2][0] =  MINOR(*this, 1, 2, 3, 0, 1, 3);
  A[2][1] = -MINOR(*this, 0, 2, 3, 0, 1, 3);
  A[2][2] =  MINOR(*this, 0, 1, 3, 0, 1, 3);
  A[2][3] = -MINOR(*this, 0, 1, 2, 0, 1, 3);
  A[3][0] = -MINOR(*this, 1, 2, 3, 0, 1, 2);
  A[3][1] =  MINOR(*this, 0, 2, 3, 0, 1, 2);
  A[3][2] = -MINOR(*this, 0, 1, 3, 0, 1, 2);
  A[3][3] =  MINOR(*this, 0, 1, 2, 0, 1, 2);

  return A;
}

float gmMatrix4::determinant() const
{
  return m_[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
	 m_[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
	 m_[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
	 m_[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
}

bool gmMatrix4::isSingular() const
{
  return gmIsZero(determinant());
}

gmVector3 gmMatrix4::transform(const gmVector3& v) const
{
  return gmVector3(
    v[0] * m_[0][0] + v[1] * m_[1][0] + v[2] * m_[2][0] + m_[3][0],
    v[0] * m_[0][1] + v[1] * m_[1][1] + v[2] * m_[2][1] + m_[3][1],
    v[0] * m_[0][2] + v[1] * m_[1][2] + v[2] * m_[2][2] + m_[3][2]);
}

////////////////////////////////////////////////////////////////////////////
// TRANSFORMATION MATRICES

gmMatrix4 gmMatrix4::identity()
{
  return gmMatrix4(1, 0, 0, 0,
		   0, 1, 0, 0,
		   0, 0, 1, 0,
		   0, 0, 0, 1);
}

gmMatrix4 gmMatrix4::rotate(float angle, const gmVector3& axis)
{
  gmMatrix4 m_;
  float length = axis.length();
  float a = axis[0] / length;
  float b = axis[1] / length;
  float c = axis[2] / length;
  float aa = a * a;
  float bb = b * b;
  float cc = c * c;
  float sine = sin(gmRadians(angle));
  float cosine = cos(gmRadians(angle));
  float omcos = 1 - cosine;

  m_[0][0] = aa + (1 - aa) * cosine;
  m_[1][1] = bb + (1 - bb) * cosine;
  m_[2][2] = cc + (1 - cc) * cosine;
  m_[0][1] = a * b * omcos + c * sine;
  m_[0][2] = a * c * omcos - b * sine;
  m_[1][0] = a * b * omcos - c * sine;
  m_[1][2] = b * c * omcos + a * sine;
  m_[2][0] = a * c * omcos + b * sine;
  m_[2][1] = b * c * omcos - a * sine;
  m_[0][3] = m_[1][3] = m_[2][3] = m_[3][0] = m_[3][1] = m_[3][2] = 0;
  m_[3][3] = 1;
   
  return m_;
}

gmMatrix4 gmMatrix4::scale(float x, float y, float z)
{
  return gmMatrix4(x, 0, 0, 0,
		   0, y, 0, 0,
		   0, 0, z, 0,
		   0, 0, 0, 1);
}

gmMatrix4 gmMatrix4::translate(float x, float y, float z)
{
  return gmMatrix4(1, 0, 0, 0,
		   0, 1, 0, 0,
		   0, 0, 1, 0,
		   x, y, z, 1);
}

////////////////////////////////////////////////////////////////////////////
// CUBIC BASIS MATRICES

gmMatrix4 gmMatrix4::bezierBasis()
{
  return gmMatrix4(-1,  3, -3,  1,
	 	    3, -6,  3,  0,
		   -3,  3,  0,  0,
		    1,  0,  0,  0);
}

gmMatrix4 gmMatrix4::bsplineBasis()
{  
  return gmMatrix4(-1,  3, -3,  1,
		    3, -6,  3,  0,
		   -3,  0,  3,  0,
		    1,  4,  1,  0) / 6;
}

gmMatrix4 gmMatrix4::catmullromBasis()
{
  return gmMatrix4(-1,  3, -3,  1,
		    2, -5,  4, -1,
		   -1,  0,  1,  0,
		    0,  2,  0,  0) / 2;
}

gmMatrix4 gmMatrix4::hermiteBasis()
{
  return gmMatrix4( 2,  1, -2,  1,
		   -3, -2,  3, -1,
		    0,  1,  0,  0,
		    1,  0,  0,  0);
}

gmMatrix4 gmMatrix4::tensedBSplineBasis(float tension)
{
  gmMatrix4 m;
  float sixth = 1.f / 6.f;

  m[0][0] = sixth * (-tension);
  m[0][1] = sixth * (12.f - 9.f * tension);
  m[0][2] = sixth * (9.f * tension - 12.f);
  m[0][3] = sixth * tension;

  m[1][0] = sixth * 3.f * tension;
  m[1][1] = sixth * (12.f * tension - 18.f);
  m[1][2] = sixth * (18.f - 15.f * tension);
  m[1][3] = 0.f;

  m[2][0] = sixth * -3.f * tension;
  m[2][1] = 0.f;
  m[2][2] = sixth * 3.f * tension;
  m[2][3] = 0.f;

  m[3][0] = sixth * tension;
  m[3][1] = sixth * (6.f - 2.f * tension);
  m[3][2] = sixth * tension;
  m[3][3] = 0.f;

  return m;
}

gmMatrix4 gmMatrix4::cardinalBasis(float tension)
{
  gmMatrix4 m;
  
  m[0][0] = -tension;
  m[0][1] = 2.f - tension;
  m[0][2] = tension - 2.f;
  m[0][3] = tension;

  m[1][0] = 2.f * tension;
  m[1][1] = tension - 3.f;
  m[1][2] = 3 - 2.f * tension;
  m[1][3] = -tension;

  m[2][0] = -tension;
  m[2][1] = 0.f;
  m[2][2] = tension;
  m[2][3] = 0.f;

  m[3][0] = 0.f;
  m[3][1] = 1.f;
  m[3][2] = 0.f;
  m[3][3] = 0.f;
  
  return m;
}

gmMatrix4 gmMatrix4::tauBasis(float bias, float tension)
{
  gmMatrix4 m;
  
  m[0][0] = tension * (bias - 1.f);
  m[0][1] = 2.f - tension * bias;
  m[0][2] = tension * (1.f - bias) - 2.f;
  m[0][3] = tension * bias;

  m[1][0] = tension * 2.f * (1.f - bias);
  m[1][1] = tension * (3.f * bias - 1.f) - 3.f;
  m[1][2] = 3.f - tension;
  m[1][3] = -tension * bias;

  m[2][0] = tension * (bias - 1.f);
  m[2][1] = tension * (1.f - 2.f * bias);
  m[2][2] = tension * bias;
  m[2][3] = 0.f;

  m[3][0] = 0.f;
  m[3][1] = 1.f;
  m[3][2] = 0.f;
  m[3][3] = 0.f;
  
  return m;
}

gmMatrix4 gmMatrix4::betaSplineBasis(float bias, float tension)
{
  gmMatrix4 m;
  float bias2, bias3, d;

  bias2 = bias * bias;
  bias3 = bias * bias2;
  d = 1.f / (tension + 2.f * bias3 + 4.f * bias2 + 4.f * bias + 2.f);

  m[0][0] = -d * 2.f * bias3;
  m[0][1] = d * 2.f * (tension + bias3 + bias2 + bias);
  m[0][2] = -d * 2.f * (tension + bias2 + bias + 1.f);
  m[0][3] = d * 2.f;

  m[1][0] = d * 6.f * bias3;
  m[1][1] = -d * 3.f * (tension + 2.f * bias3 + 2.f * bias2);
  m[1][2] = d * 3.f * (tension + 2 * bias2);
  m[1][3] = 0.f;
  
  m[2][0] = -d * 6.f * bias3;
  m[2][1] = d * 6.f * (bias3 - bias);
  m[2][2] = d * 6.f * bias;
  m[2][3] = 0.f;

  m[3][0] = d * 2.f * bias3;
  m[3][1] = d * (tension + 4 * (bias2 + bias));
  m[3][2] = d * 2.f;
  m[3][3] = 0.f;

  return m;
}


