// gmMatrix3.cc - 3x3 element matrix class
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#include "gmMat3.h"
#include "gmVec2.h"
#include "gmVec3.h"

// private function: RCD
// - dot product of row i of matrix A and row j of matrix B

inline double RCD(const gmMatrix3& A, const gmMatrix3& B, int i, int j)
{
  return A[i][0] * B[0][j] + A[i][1] * B[1][j] + A[i][2] * B[2][j];
}

// private function: MINOR
// - MINOR of M[r][c]; r in {0,1,2}-{r0,r1}; c in {0,1,2}-{c0,c1}

inline double MINOR(const gmMatrix3& M, int r0, int r1, int c0, int c1)
{
  return M[r0][c0] * M[r1][c1] - M[r1][c0] * M[r0][c1];
}

// CONSTRUCTORS

gmMatrix3::gmMatrix3()
{
  assign(0,0,0, 0,0,0, 0,0,0);
}

gmMatrix3::gmMatrix3(const gmMatrix3& M)
{
  assign(M[0][0], M[0][1], M[0][2],
         M[1][0], M[1][1], M[1][2],
         M[2][0], M[2][1], M[2][2]);
}

gmMatrix3::gmMatrix3(double a00, double a01, double a02,
		     double a10, double a11, double a12,
		     double a20, double a21, double a22)
{
  assign(a00, a01, a02,
         a10, a11, a12,
         a20, a21, a22);
}

// ASSIGNMENT

gmMatrix3& gmMatrix3::assign(double a00, double a01, double a02,
		             double a10, double a11, double a12,
		             double a20, double a21, double a22)
{
  m_[0][0] = a00; m_[0][1] = a01; m_[0][2] = a02;
  m_[1][0] = a10; m_[1][1] = a11; m_[1][2] = a12;
  m_[2][0] = a20; m_[2][1] = a21; m_[2][2] = a22;
  return *this;
}

gmMatrix3& gmMatrix3::operator =(const gmMatrix3& M)
{
  assign(M[0][0], M[0][1], M[0][2],
	 M[1][0], M[1][1], M[1][2],
         M[2][0], M[2][1], M[2][2]);
  return *this;
}

// OPERATORS

gmMatrix3& gmMatrix3::operator +=(const gmMatrix3& M)
{
  m_[0][0] += M[0][0]; m_[0][1] += M[0][1]; m_[0][2] += M[0][2];
  m_[1][0] += M[1][0]; m_[1][1] += M[1][1]; m_[1][2] += M[1][2];
  m_[2][0] += M[2][0]; m_[2][1] += M[2][1]; m_[2][2] += M[2][2];
  return *this;
}

gmMatrix3& gmMatrix3::operator -=(const gmMatrix3& M)
{
  m_[0][0] -= M[0][0]; m_[0][1] -= M[0][1]; m_[0][2] -= M[0][2];
  m_[1][0] -= M[1][0]; m_[1][1] -= M[1][1]; m_[1][2] -= M[1][2];
  m_[2][0] -= M[2][0]; m_[2][1] -= M[2][1]; m_[2][2] -= M[2][2];
  return *this;
}

gmMatrix3& gmMatrix3::operator *=(const gmMatrix3& M)
{
  assign(RCD(*this, M, 0, 0), RCD(*this, M, 0, 1), RCD(*this, M, 0, 2),
	 RCD(*this, M, 1, 0), RCD(*this, M, 1, 1), RCD(*this, M, 1, 2),
	 RCD(*this, M, 2, 0), RCD(*this, M, 2, 1), RCD(*this, M, 2, 2));
  return *this;
}

gmMatrix3& gmMatrix3::operator *=(double d)
{
  m_[0][0] *= d; m_[0][1] *= d; m_[0][2] *= d;
  m_[1][0] *= d; m_[1][1] *= d; m_[1][2] *= d;
  m_[2][0] *= d; m_[2][1] *= d; m_[2][2] *= d;
  return *this;
}

gmMatrix3& gmMatrix3::operator /=(double d)
{
  double di = 1 / d;
  m_[0][0] *= di; m_[0][1] *= di; m_[0][2] *= di;
  m_[1][0] *= di; m_[1][1] *= di; m_[1][2] *= di;
  m_[2][0] *= di; m_[2][1] *= di; m_[2][2] *= di;
  return *this;
}

gmMatrix3 gmMatrix3::operator +(const gmMatrix3& M) const
{
  return gmMatrix3(m_[0][0] + M[0][0], m_[0][1] + M[0][1], m_[0][2] + M[0][2],
		   m_[1][0] + M[1][0], m_[1][1] + M[1][1], m_[1][2] + M[1][2],
		   m_[2][0] + M[2][0], m_[2][1] + M[2][1], m_[2][2] + M[2][2]);
}

gmMatrix3 gmMatrix3::operator -(const gmMatrix3& M) const
{
  return gmMatrix3(m_[0][0] - M[0][0], m_[0][1] - M[0][1], m_[0][2] - M[0][2],
		   m_[1][0] - M[1][0], m_[1][1] - M[1][1], m_[1][2] - M[1][2],
		   m_[2][0] - M[2][0], m_[2][1] - M[2][1], m_[2][2] - M[2][2]);
}

gmMatrix3 gmMatrix3::operator -() const
{
  return gmMatrix3(-m_[0][0], -m_[0][1], -m_[0][2],
		   -m_[1][0], -m_[1][1], -m_[1][2],
                   -m_[2][0], -m_[2][1], -m_[2][2]);
}

gmMatrix3 gmMatrix3::operator *(const gmMatrix3& M) const
{
  return
    gmMatrix3(RCD(*this, M, 0, 0), RCD(*this, M, 0, 1), RCD(*this, M, 0, 2),
              RCD(*this, M, 1, 0), RCD(*this, M, 1, 1), RCD(*this, M, 1, 2),
              RCD(*this, M, 2, 0), RCD(*this, M, 2, 1), RCD(*this, M, 2, 2));
}

gmMatrix3 gmMatrix3::operator *(double d) const
{
  return gmMatrix3(m_[0][0] * d, m_[0][1] * d, m_[0][2] * d,
		   m_[1][0] * d, m_[1][1] * d, m_[1][2] * d,
                   m_[2][0] * d, m_[2][1] * d, m_[2][2] * d);
}

gmMatrix3 gmMatrix3::operator /(double d) const
{
  assert(!gmIsZero(d));
  double di = 1 / d;
  return gmMatrix3(m_[0][0] * di, m_[0][1] * di, m_[0][2] * di,
		   m_[1][0] * di, m_[1][1] * di, m_[1][2] * di,
                   m_[2][0] * di, m_[2][1] * di, m_[2][2] * di);
}

gmMatrix3 operator *(double d, const gmMatrix3& M)
{
  return gmMatrix3(M[0][0] * d, M[0][1] * d, M[0][2] * d,
		   M[1][0] * d, M[1][1] * d, M[1][2] * d,
                   M[2][0] * d, M[2][1] * d, M[2][2] * d);
}

bool gmMatrix3::operator ==(const gmMatrix3& M) const
{
  return(gmFuzEQ(m_[0][0], M[0][0]) &&
         gmFuzEQ(m_[0][1], M[0][1]) &&
	 gmFuzEQ(m_[0][2], M[0][2]) &&
	 
	 gmFuzEQ(m_[1][0], M[1][0]) &&
         gmFuzEQ(m_[1][1], M[1][1]) &&
	 gmFuzEQ(m_[1][2], M[1][2]) &&

	 gmFuzEQ(m_[2][0], M[2][0]) &&
         gmFuzEQ(m_[2][1], M[2][1]) &&
	 gmFuzEQ(m_[2][2], M[2][2]));
}

bool gmMatrix3::operator !=(const gmMatrix3& M) const
{
  return (!(*this == M));
}

gmVector3 gmMatrix3::operator *(const gmVector3& v) const
{
  return gmVector3(m_[0][0] * v[0] + m_[0][1] * v[1] + m_[0][2] * v[2],
                   m_[1][0] * v[0] + m_[1][1] * v[1] + m_[1][2] * v[2],
                   m_[2][0] * v[0] + m_[2][1] * v[1] + m_[2][2] * v[2]);
}

gmVector3 operator *(const gmVector3& v, const gmMatrix3& M)
{
  return gmVector3(v[0] * M[0][0] + v[1] * M[1][0] + v[2] * M[2][0],
                   v[0] * M[0][1] + v[1] * M[1][1] + v[2] * M[2][1],
                   v[0] * M[0][2] * v[1] * M[1][2] + v[2] * M[2][2]);
}

// OPERATIONS

gmMatrix3 gmMatrix3::transpose() const
{
  return gmMatrix3(m_[0][0], m_[1][0], m_[2][0],
		   m_[0][1], m_[1][1], m_[2][1],
                   m_[0][2], m_[1][2], m_[2][2]);
}

gmMatrix3 gmMatrix3::inverse() const
{
  assert(!isSingular());
  return adjoint() * gmInv(determinant());
}

gmMatrix3 gmMatrix3::adjoint() const
{
  gmMatrix3 A;
  
  A[0][0] =  MINOR(*this, 1, 2, 1, 2);
  A[0][1] = -MINOR(*this, 0, 2, 1, 2);
  A[0][2] =  MINOR(*this, 0, 1, 1, 2);
  A[1][0] = -MINOR(*this, 1, 2, 0, 2);
  A[1][1] =  MINOR(*this, 0, 2, 0, 2);
  A[1][2] = -MINOR(*this, 0, 1, 0, 2);
  A[2][0] =  MINOR(*this, 1, 2, 0, 1);
  A[2][1] = -MINOR(*this, 0, 2, 0, 1);
  A[2][2] =  MINOR(*this, 0, 1, 0, 1);

  return A;
}

double gmMatrix3::determinant() const
{
  return m_[0][0] * MINOR(*this, 1, 2, 1, 2) -
         m_[0][1] * MINOR(*this, 1, 2, 0, 2) +
	 m_[0][2] * MINOR(*this, 1, 2, 0, 1);
}

bool gmMatrix3::isSingular() const
{
  return gmIsZero(determinant());
}

gmVector2 gmMatrix3::transform(const gmVector2& v) const
{
  return gmVector2(v[0] * m_[0][0] + v[1] * m_[1][0] + m_[2][0],
                   v[0] * m_[0][1] + v[1] * m_[1][1] + m_[2][1]);
}

// TRANSFORMATION MATRICES

gmMatrix3 gmMatrix3::identity()
{
  return gmMatrix3(1, 0, 0,
                   0, 1, 0,
                   0, 0, 1);
}

gmMatrix3 gmMatrix3::rotate(double angle)
{
  double sine = sin(gmRadians(angle));
  double cosine = cos(gmRadians(angle));
  
  return gmMatrix3( cosine, sine,   0,
                   -sine,   cosine, 0,
                    0,      0,      1);
}

gmMatrix3 gmMatrix3::scale(double x, double y)
{
  return gmMatrix3(x, 0, 0,
		   0, y, 0,
		   0, 0, 1);
}

gmMatrix3 gmMatrix3::translate(double x, double y)
{
  return gmMatrix3(1, 0, 0,
		   0, 1, 0,
		   x, y, 1);
}
