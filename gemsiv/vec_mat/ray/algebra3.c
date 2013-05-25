#include "algebra3.h"
#include <ctype.h>

/****************************************************************
*								*
*		    vec2 Member functions			*
*								*
****************************************************************/

// CONSTRUCTORS

vec2::vec2() {}

vec2::vec2(const double x, const double y)
{ n[VX] = x; n[VY] = y; }

vec2::vec2(const double d)
{ n[VX] = n[VY] = d; }

vec2::vec2(const vec2& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; }

vec2::vec2(const vec3& v) // it is up to caller to avoid divide-by-zero
{ n[VX] = v.n[VX]/v.n[VZ]; n[VY] = v.n[VY]/v.n[VZ]; };

vec2::vec2(const vec3& v, int dropAxis) {
    switch (dropAxis) {
	case VX: n[VX] = v.n[VY]; n[VY] = v.n[VZ]; break;
	case VY: n[VX] = v.n[VX]; n[VY] = v.n[VZ]; break;
	default: n[VX] = v.n[VX]; n[VY] = v.n[VY]; break;
    }
}


// ASSIGNMENT OPERATORS

vec2& vec2::operator = (const vec2& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; return *this; }

vec2& vec2::operator += ( const vec2& v )
{ n[VX] += v.n[VX]; n[VY] += v.n[VY]; return *this; }

vec2& vec2::operator -= ( const vec2& v )
{ n[VX] -= v.n[VX]; n[VY] -= v.n[VY]; return *this; }

vec2& vec2::operator *= ( const double d )
{ n[VX] *= d; n[VY] *= d; return *this; }

vec2& vec2::operator /= ( const double d )
{ double d_inv = 1./d; n[VX] *= d_inv; n[VY] *= d_inv; return *this; }

double& vec2::operator [] ( int i) {
    if (i < VX || i > VY)
	V_ERROR("vec2 [] operator: illegal access; index = " << i << '\n')
    return n[i];
}


// SPECIAL FUNCTIONS

double vec2::length()
{ return sqrt(length2()); }

double vec2::length2()
{ return n[VX]*n[VX] + n[VY]*n[VY]; }

vec2& vec2::normalize() // it is up to caller to avoid divide-by-zero
{ *this /= length(); return *this; }

vec2& vec2::apply(V_FCT_PTR fct)
{ n[VX] = (*fct)(n[VX]); n[VY] = (*fct)(n[VY]); return *this; }


// FRIENDS

vec2 operator - (const vec2& a)
{ return vec2(-a.n[VX],-a.n[VY]); }

vec2 operator + (const vec2& a, const vec2& b)
{ return vec2(a.n[VX]+ b.n[VX], a.n[VY] + b.n[VY]); }

vec2 operator - (const vec2& a, const vec2& b)
{ return vec2(a.n[VX]-b.n[VX], a.n[VY]-b.n[VY]); }

vec2 operator * (const vec2& a, const double d)
{ return vec2(d*a.n[VX], d*a.n[VY]); }

vec2 operator * (const double d, const vec2& a)
{ return a*d; }

vec2 operator * (const mat3& a, const vec2& v) {
    vec3 av;

    av.n[VX] = a.v[0].n[VX]*v.n[VX] + a.v[0].n[VY]*v.n[VY] + a.v[0].n[VZ];
    av.n[VY] = a.v[1].n[VX]*v.n[VX] + a.v[1].n[VY]*v.n[VY] + a.v[1].n[VZ];
    av.n[VZ] = a.v[2].n[VX]*v.n[VX] + a.v[2].n[VY]*v.n[VY] + a.v[2].n[VZ];
    return av;
}

vec2 operator * (const vec2& v, mat3& a)
{ return a.transpose() * v; }

double operator * (const vec2& a, const vec2& b)
{ return (a.n[VX]*b.n[VX] + a.n[VY]*b.n[VY]); }

vec2 operator / (const vec2& a, const double d)
{ double d_inv = 1./d; return vec2(a.n[VX]*d_inv, a.n[VY]*d_inv); }

vec3 operator ^ (const vec2& a, const vec2& b)
{ return vec3(0.0, 0.0, a.n[VX] * b.n[VY] - b.n[VX] * a.n[VY]); }

int operator == (const vec2& a, const vec2& b)
{ return (a.n[VX] == b.n[VX]) && (a.n[VY] == b.n[VY]); }

int operator != (const vec2& a, const vec2& b)
{ return !(a == b); }

ostream& operator << (ostream& s, vec2& v)
{ return s << "| " << v.n[VX] << ' ' << v.n[VY] << " |"; }

istream& operator >> (istream& s, vec2& v) {
    vec2	v_tmp;
    char	c = ' ';

    while (isspace(c))
	s >> c;
    // The vectors can be formatted either as x y or | x y |
    if (c == '|') {
	s >> v_tmp[VX] >> v_tmp[VY];
	while (s >> c && isspace(c)) ;
	if (c != '|')
	    s.set(_bad);
	}
    else {
	s.putback(c);
	s >> v_tmp[VX] >> v_tmp[VY];
	}
    if (s)
	v = v_tmp;
    return s;
}

void swap(vec2& a, vec2& b)
{ vec2 tmp(a); a = b; b = tmp; }

vec2 min(const vec2& a, const vec2& b)
{ return vec2(MIN(a.n[VX], b.n[VX]), MIN(a.n[VY], b.n[VY])); }

vec2 max(const vec2& a, const vec2& b)
{ return vec2(MAX(a.n[VX], b.n[VX]), MAX(a.n[VY], b.n[VY])); }

vec2 prod(const vec2& a, const vec2& b)
{ return vec2(a.n[VX] * b.n[VX], a.n[VY] * b.n[VY]); }

/****************************************************************
*								*
*		    vec3 Member functions			*
*								*
****************************************************************/

// CONSTRUCTORS

vec3::vec3() {}

vec3::vec3(const double x, const double y, const double z)
{ n[VX] = x; n[VY] = y; n[VZ] = z; }

vec3::vec3(const double d)
{ n[VX] = n[VY] = n[VZ] = d; }

vec3::vec3(const vec3& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; }

vec3::vec3(const vec2& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = 1.0; }

vec3::vec3(const vec2& v, double d)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = d; }

vec3::vec3(const vec4& v) // it is up to caller to avoid divide-by-zero
{ n[VX] = v.n[VX] / v.n[VW]; n[VY] = v.n[VY] / v.n[VW];
  n[VZ] = v.n[VZ] / v.n[VW]; }

vec3::vec3(const vec4& v, int dropAxis) {
    switch (dropAxis) {
	case VX: n[VX] = v.n[VY]; n[VY] = v.n[VZ]; n[VZ] = v.n[VW]; break;
	case VY: n[VX] = v.n[VX]; n[VY] = v.n[VZ]; n[VZ] = v.n[VW]; break;
	case VZ: n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VW]; break;
	default: n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; break;
    }
}


// ASSIGNMENT OPERATORS

vec3& vec3::operator = (const vec3& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; return *this; }

vec3& vec3::operator += ( const vec3& v )
{ n[VX] += v.n[VX]; n[VY] += v.n[VY]; n[VZ] += v.n[VZ]; return *this; }

vec3& vec3::operator -= ( const vec3& v )
{ n[VX] -= v.n[VX]; n[VY] -= v.n[VY]; n[VZ] -= v.n[VZ]; return *this; }

vec3& vec3::operator *= ( const double d )
{ n[VX] *= d; n[VY] *= d; n[VZ] *= d; return *this; }

vec3& vec3::operator /= ( const double d )
{ double d_inv = 1./d; n[VX] *= d_inv; n[VY] *= d_inv; n[VZ] *= d_inv;
  return *this; }

double& vec3::operator [] ( int i) {
    if (i < VX || i > VZ)
	V_ERROR("vec3 [] operator: illegal access; index = " << i << '\n')
    return n[i];
}


// SPECIAL FUNCTIONS

double vec3::length()
{  return sqrt(length2()); }

double vec3::length2()
{  return n[VX]*n[VX] + n[VY]*n[VY] + n[VZ]*n[VZ]; }

vec3& vec3::normalize() // it is up to caller to avoid divide-by-zero
{ *this /= length(); return *this; }

vec3& vec3::apply(V_FCT_PTR fct)
{ n[VX] = (*fct)(n[VX]); n[VY] = (*fct)(n[VY]); n[VZ] = (*fct)(n[VZ]);
return *this; }


// FRIENDS

vec3 operator - (const vec3& a)
{  return vec3(-a.n[VX],-a.n[VY],-a.n[VZ]); }

vec3 operator + (const vec3& a, const vec3& b)
{ return vec3(a.n[VX]+ b.n[VX], a.n[VY] + b.n[VY], a.n[VZ] + b.n[VZ]); }

vec3 operator - (const vec3& a, const vec3& b)
{ return vec3(a.n[VX]-b.n[VX], a.n[VY]-b.n[VY], a.n[VZ]-b.n[VZ]); }

vec3 operator * (const vec3& a, const double d)
{ return vec3(d*a.n[VX], d*a.n[VY], d*a.n[VZ]); }

vec3 operator * (const double d, const vec3& a)
{ return a*d; }

vec3 operator * (const mat4& a, const vec3& v)
{ return a * vec4(v); }

vec3 operator * (const vec3& v, mat4& a)
{ return a.transpose() * v; }

double operator * (const vec3& a, const vec3& b)
{ return (a.n[VX]*b.n[VX] + a.n[VY]*b.n[VY] + a.n[VZ]*b.n[VZ]); }

vec3 operator / (const vec3& a, const double d)
{ double d_inv = 1./d; return vec3(a.n[VX]*d_inv, a.n[VY]*d_inv,
  a.n[VZ]*d_inv); }

vec3 operator ^ (const vec3& a, const vec3& b) {
    return vec3(a.n[VY]*b.n[VZ] - a.n[VZ]*b.n[VY],
		a.n[VZ]*b.n[VX] - a.n[VX]*b.n[VZ],
		a.n[VX]*b.n[VY] - a.n[VY]*b.n[VX]);
}

int operator == (const vec3& a, const vec3& b)
{ return (a.n[VX] == b.n[VX]) && (a.n[VY] == b.n[VY]) && (a.n[VZ] == b.n[VZ]);
}

int operator != (const vec3& a, const vec3& b)
{ return !(a == b); }

ostream& operator << (ostream& s, vec3& v)
{ return s << "| " << v.n[VX] << ' ' << v.n[VY] << ' ' << v.n[VZ] << " |"; }

istream& operator >> (istream& s, vec3& v) {
    vec3	v_tmp;
    char	c = ' ';

    while (isspace(c))
	s >> c;
    // The vectors can be formatted either as x y z or | x y z |
    if (c == '|') {
	s >> v_tmp[VX] >> v_tmp[VY] >> v_tmp[VZ];
	while (s >> c && isspace(c)) ;
	if (c != '|')
	    s.set(_bad);
	}
    else {
	s.putback(c);
	s >> v_tmp[VX] >> v_tmp[VY] >> v_tmp[VZ];
	}
    if (s)
	v = v_tmp;
    return s;
}

void swap(vec3& a, vec3& b)
{ vec3 tmp(a); a = b; b = tmp; }

vec3 min(const vec3& a, const vec3& b)
{ return vec3(MIN(a.n[VX], b.n[VX]), MIN(a.n[VY], b.n[VY]), MIN(a.n[VZ],
  b.n[VZ])); }

vec3 max(const vec3& a, const vec3& b)
{ return vec3(MAX(a.n[VX], b.n[VX]), MAX(a.n[VY], b.n[VY]), MAX(a.n[VZ],
  b.n[VZ])); }

vec3 prod(const vec3& a, const vec3& b)
{ return vec3(a.n[VX] * b.n[VX], a.n[VY] * b.n[VY], a.n[VZ] * b.n[VZ]); }


/****************************************************************
*								*
*		    vec4 Member functions			*
*								*
****************************************************************/

// CONSTRUCTORS

vec4::vec4() {}

vec4::vec4(const double x, const double y, const double z, const double w)
{ n[VX] = x; n[VY] = y; n[VZ] = z; n[VW] = w; }

vec4::vec4(const double d)
{  n[VX] = n[VY] = n[VZ] = n[VW] = d; }

vec4::vec4(const vec4& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; n[VW] = v.n[VW]; }

vec4::vec4(const vec3& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; n[VW] = 1.0; }

vec4::vec4(const vec3& v, const double d)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ];  n[VW] = d; }


// ASSIGNMENT OPERATORS

vec4& vec4::operator = (const vec4& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; n[VW] = v.n[VW];
return *this; }

vec4& vec4::operator += ( const vec4& v )
{ n[VX] += v.n[VX]; n[VY] += v.n[VY]; n[VZ] += v.n[VZ]; n[VW] += v.n[VW];
return *this; }

vec4& vec4::operator -= ( const vec4& v )
{ n[VX] -= v.n[VX]; n[VY] -= v.n[VY]; n[VZ] -= v.n[VZ]; n[VW] -= v.n[VW];
return *this; }

vec4& vec4::operator *= ( const double d )
{ n[VX] *= d; n[VY] *= d; n[VZ] *= d; n[VW] *= d; return *this; }

vec4& vec4::operator /= ( const double d )
{ double d_inv = 1./d; n[VX] *= d_inv; n[VY] *= d_inv; n[VZ] *= d_inv;
  n[VW] *= d_inv; return *this; }

double& vec4::operator [] ( int i) {
    if (i < VX || i > VW)
	V_ERROR("vec4 [] operator: illegal access; index = " << i << '\n')
    return n[i];
}


// SPECIAL FUNCTIONS

double vec4::length()
{ return sqrt(length2()); }

double vec4::length2()
{ return n[VX]*n[VX] + n[VY]*n[VY] + n[VZ]*n[VZ] + n[VW]*n[VW]; }

vec4& vec4::normalize() // it is up to caller to avoid divide-by-zero
{ *this /= length(); return *this; }

vec4& vec4::apply(V_FCT_PTR fct)
{ n[VX] = (*fct)(n[VX]); n[VY] = (*fct)(n[VY]); n[VZ] = (*fct)(n[VZ]);
n[VW] = (*fct)(n[VW]); return *this; }


// FRIENDS

vec4 operator - (const vec4& a)
{ return vec4(-a.n[VX],-a.n[VY],-a.n[VZ],-a.n[VW]); }

vec4 operator + (const vec4& a, const vec4& b)
{ return vec4(a.n[VX] + b.n[VX], a.n[VY] + b.n[VY], a.n[VZ] + b.n[VZ],
  a.n[VW] + b.n[VW]); }

vec4 operator - (const vec4& a, const vec4& b)
{  return vec4(a.n[VX] - b.n[VX], a.n[VY] - b.n[VY], a.n[VZ] - b.n[VZ],
   a.n[VW] - b.n[VW]); }

vec4 operator * (const vec4& a, const double d)
{ return vec4(d*a.n[VX], d*a.n[VY], d*a.n[VZ], d*a.n[VW] ); }

vec4 operator * (const double d, const vec4& a)
{ return a*d; }

vec4 operator * (const mat4& a, const vec4& v) {
    #define ROWCOL(i) a.v[i].n[0]*v.n[VX] + a.v[i].n[1]*v.n[VY] \
    + a.v[i].n[2]*v.n[VZ] + a.v[i].n[3]*v.n[VW]
    return vec4(ROWCOL(0), ROWCOL(1), ROWCOL(2), ROWCOL(3));
    #undef ROWCOL
}

vec4 operator * (const vec4& v, mat4& a)
{ return a.transpose() * v; }

double operator * (const vec4& a, const vec4& b)
{ return (a.n[VX]*b.n[VX] + a.n[VY]*b.n[VY] + a.n[VZ]*b.n[VZ] +
  a.n[VW]*b.n[VW]); }

vec4 operator / (const vec4& a, const double d)
{ double d_inv = 1./d; return vec4(a.n[VX]*d_inv, a.n[VY]*d_inv, a.n[VZ]*d_inv,
  a.n[VW]*d_inv); }

int operator == (const vec4& a, const vec4& b)
{ return (a.n[VX] == b.n[VX]) && (a.n[VY] == b.n[VY]) && (a.n[VZ] == b.n[VZ])
  && (a.n[VW] == b.n[VW]); }

int operator != (const vec4& a, const vec4& b)
{ return !(a == b); }

ostream& operator << (ostream& s, vec4& v)
{ return s << "| " << v.n[VX] << ' ' << v.n[VY] << ' ' << v.n[VZ] << ' '
  << v.n[VW] << " |"; }

istream& operator >> (istream& s, vec4& v) {
    vec4	v_tmp;
    char	c = ' ';

    while (isspace(c))
	s >> c;
    // The vectors can be formatted either as x y z w or | x y z w |
    if (c == '|') {
	s >> v_tmp[VX] >> v_tmp[VY] >> v_tmp[VZ] >> v_tmp[VW];
	while (s >> c && isspace(c)) ;
	if (c != '|')
	    s.set(_bad);
	}
    else {
	s.putback(c);
	s >> v_tmp[VX] >> v_tmp[VY] >> v_tmp[VZ] >> v_tmp[VW];
	}
    if (s)
	v = v_tmp;
    return s;
}

void swap(vec4& a, vec4& b)
{ vec4 tmp(a); a = b; b = tmp; }

vec4 min(const vec4& a, const vec4& b)
{ return vec4(MIN(a.n[VX], b.n[VX]), MIN(a.n[VY], b.n[VY]), MIN(a.n[VZ],
  b.n[VZ]), MIN(a.n[VW], b.n[VW])); }

vec4 max(const vec4& a, const vec4& b)
{ return vec4(MAX(a.n[VX], b.n[VX]), MAX(a.n[VY], b.n[VY]), MAX(a.n[VZ],
  b.n[VZ]), MAX(a.n[VW], b.n[VW])); }

vec4 prod(const vec4& a, const vec4& b)
{ return vec4(a.n[VX] * b.n[VX], a.n[VY] * b.n[VY], a.n[VZ] * b.n[VZ],
  a.n[VW] * b.n[VW]); }


/****************************************************************
*								*
*		    mat3 member functions			*
*								*
****************************************************************/

// CONSTRUCTORS

mat3::mat3() {}

mat3::mat3(const vec3& v0, const vec3& v1, const vec3& v2)
{ v[0] = v0; v[1] = v1; v[2] = v2; }

mat3::mat3(const double d)
{ v[0] = v[1] = v[2] = vec3(d); }

mat3::mat3(const mat3& m)
{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; }


// ASSIGNMENT OPERATORS

mat3& mat3::operator = ( const mat3& m )
{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; return *this; }

mat3& mat3::operator += ( const mat3& m )
{ v[0] += m.v[0]; v[1] += m.v[1]; v[2] += m.v[2]; return *this; }

mat3& mat3::operator -= ( const mat3& m )
{ v[0] -= m.v[0]; v[1] -= m.v[1]; v[2] -= m.v[2]; return *this; }

mat3& mat3::operator *= ( const double d )
{ v[0] *= d; v[1] *= d; v[2] *= d; return *this; }

mat3& mat3::operator /= ( const double d )
{ v[0] /= d; v[1] /= d; v[2] /= d; return *this; }

vec3& mat3::operator [] ( int i) {
    if (i < VX || i > VZ)
	V_ERROR("mat3 [] operator: illegal access; index = " << i << '\n')
    return v[i];
}


// SPECIAL FUNCTIONS

mat3 mat3::transpose() {
    return mat3(vec3(v[0][0], v[1][0], v[2][0]),
		vec3(v[0][1], v[1][1], v[2][1]),
		vec3(v[0][2], v[1][2], v[2][2]));
}

mat3 mat3::inverse()	    // Gauss-Jordan elimination with partial pivoting
    {
    mat3 a(*this),	    // As a evolves from original mat into identity
	 b(identity2D());   // b evolves from identity into inverse(a)
    int	 i, j, i1;

    // Loop over cols of a from left to right, eliminating above and below diag
    for (j=0; j<3; j++) {   // Find largest pivot in column j among rows j..2
    i1 = j;		    // Row with largest pivot candidate
    for (i=j+1; i<3; i++)
	if (fabs(a.v[i].n[j]) > fabs(a.v[i1].n[j]))
	    i1 = i;

    // Swap rows i1 and j in a and b to put pivot on diagonal
    swap(a.v[i1], a.v[j]);
    swap(b.v[i1], b.v[j]);

    // Scale row j to have a unit diagonal
    if (a.v[j].n[j]==0.)
	V_ERROR("mat3::inverse: singular matrix; can't invert\n")
    b.v[j] /= a.v[j].n[j];
    a.v[j] /= a.v[j].n[j];

    // Eliminate off-diagonal elems in col j of a, doing identical ops to b
    for (i=0; i<3; i++)
	if (i!=j) {
	b.v[i] -= a.v[i].n[j]*b.v[j];
	a.v[i] -= a.v[i].n[j]*a.v[j];
	}
    }
    return b;
}

mat3& mat3::apply(V_FCT_PTR fct) {
    v[VX].apply(fct);
    v[VY].apply(fct);
    v[VZ].apply(fct);
    return *this;
}


// FRIENDS

mat3 operator - (const mat3& a)
{ return mat3(-a.v[0], -a.v[1], -a.v[2]); }

mat3 operator + (const mat3& a, const mat3& b)
{ return mat3(a.v[0] + b.v[0], a.v[1] + b.v[1], a.v[2] + b.v[2]); }

mat3 operator - (const mat3& a, const mat3& b)
{ return mat3(a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2]); }

mat3 operator * (mat3& a, mat3& b) {
    #define ROWCOL(i, j) \
    a.v[i].n[0]*b.v[0][j] + a.v[i].n[1]*b.v[1][j] + a.v[i].n[2]*b.v[2][j]
    return mat3(vec3(ROWCOL(0,0), ROWCOL(0,1), ROWCOL(0,2)),
		vec3(ROWCOL(1,0), ROWCOL(1,1), ROWCOL(1,2)),
		vec3(ROWCOL(2,0), ROWCOL(2,1), ROWCOL(2,2)));
    #undef ROWCOL
}

mat3 operator * (const mat3& a, const double d)
{ return mat3(a.v[0] * d, a.v[1] * d, a.v[2] * d); }

mat3 operator * (const double d, const mat3& a)
{ return a*d; }

mat3 operator / (const mat3& a, const double d)
{ return mat3(a.v[0] / d, a.v[1] / d, a.v[2] / d); }

int operator == (const mat3& a, const mat3& b)
{ return (a.v[0] == b.v[0]) && (a.v[1] == b.v[1]) && (a.v[2] == b.v[2]); }

int operator != (const mat3& a, const mat3& b)
{ return !(a == b); }

ostream& operator << (ostream& s, mat3& m)
{ return s << m.v[VX] << '\n' << m.v[VY] << '\n' << m.v[VZ]; }

istream& operator >> (istream& s, mat3& m) {
    mat3    m_tmp;

    s >> m_tmp[VX] >> m_tmp[VY] >> m_tmp[VZ];
    if (s)
	m = m_tmp;
    return s;
}

void swap(mat3& a, mat3& b)
{ mat3 tmp(a); a = b; b = tmp; }


/****************************************************************
*								*
*		    mat4 member functions			*
*								*
****************************************************************/

// CONSTRUCTORS

mat4::mat4() {}

mat4::mat4(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3)
{ v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; }

mat4::mat4(const double d)
{ v[0] = v[1] = v[2] = v[3] = vec4(d); }

mat4::mat4(const mat4& m)
{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; v[3] = m.v[3]; }


// ASSIGNMENT OPERATORS

mat4& mat4::operator = ( const mat4& m )
{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; v[3] = m.v[3];
return *this; }

mat4& mat4::operator += ( const mat4& m )
{ v[0] += m.v[0]; v[1] += m.v[1]; v[2] += m.v[2]; v[3] += m.v[3];
return *this; }

mat4& mat4::operator -= ( const mat4& m )
{ v[0] -= m.v[0]; v[1] -= m.v[1]; v[2] -= m.v[2]; v[3] -= m.v[3];
return *this; }

mat4& mat4::operator *= ( const double d )
{ v[0] *= d; v[1] *= d; v[2] *= d; v[3] *= d; return *this; }

mat4& mat4::operator /= ( const double d )
{ v[0] /= d; v[1] /= d; v[2] /= d; v[3] /= d; return *this; }

vec4& mat4::operator [] ( int i) {
    if (i < VX || i > VW)
	V_ERROR("mat4 [] operator: illegal access; index = " << i << '\n')
    return v[i];
}

// SPECIAL FUNCTIONS;

mat4 mat4::transpose() {
    return mat4(vec4(v[0][0], v[1][0], v[2][0], v[3][0]),
		vec4(v[0][1], v[1][1], v[2][1], v[3][1]),
		vec4(v[0][2], v[1][2], v[2][2], v[3][2]),
		vec4(v[0][3], v[1][3], v[2][3], v[3][3]));
}

mat4 mat4::inverse()	    // Gauss-Jordan elimination with partial pivoting
{
    mat4 a(*this),	    // As a evolves from original mat into identity
	 b(identity3D());   // b evolves from identity into inverse(a)
    int i, j, i1;

    // Loop over cols of a from left to right, eliminating above and below diag
    for (j=0; j<4; j++) {   // Find largest pivot in column j among rows j..3
    i1 = j;		    // Row with largest pivot candidate
    for (i=j+1; i<4; i++)
	if (fabs(a.v[i].n[j]) > fabs(a.v[i1].n[j]))
	i1 = i;

    // Swap rows i1 and j in a and b to put pivot on diagonal
    swap(a.v[i1], a.v[j]);
    swap(b.v[i1], b.v[j]);

    // Scale row j to have a unit diagonal
    if (a.v[j].n[j]==0.)
	V_ERROR("mat4::inverse: singular matrix; can't invert\n");
    b.v[j] /= a.v[j].n[j];
    a.v[j] /= a.v[j].n[j];

    // Eliminate off-diagonal elems in col j of a, doing identical ops to b
    for (i=0; i<4; i++)
	if (i!=j) {
	b.v[i] -= a.v[i].n[j]*b.v[j];
	a.v[i] -= a.v[i].n[j]*a.v[j];
	}
    }
    return b;
}

mat4& mat4::apply(V_FCT_PTR fct)
{ v[VX].apply(fct); v[VY].apply(fct); v[VZ].apply(fct); v[VW].apply(fct);
return *this; }


// FRIENDS

mat4 operator - (const mat4& a)
{ return mat4(-a.v[0], -a.v[1], -a.v[2], -a.v[3]); }

mat4 operator + (const mat4& a, const mat4& b)
{ return mat4(a.v[0] + b.v[0], a.v[1] + b.v[1], a.v[2] + b.v[2],
  a.v[3] + b.v[3]);
}

mat4 operator - (const mat4& a, const mat4& b)
{ return mat4(a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2], a.v[3] - b.v[3]); }

mat4 operator * (mat4& a, mat4& b) {
    #define ROWCOL(i, j) a.v[i].n[0]*b.v[0][j] + a.v[i].n[1]*b.v[1][j] + \
    a.v[i].n[2]*b.v[2][j] + a.v[i].n[3]*b.v[3][j]
    return mat4(
    vec4(ROWCOL(0,0), ROWCOL(0,1), ROWCOL(0,2), ROWCOL(0,3)),
    vec4(ROWCOL(1,0), ROWCOL(1,1), ROWCOL(1,2), ROWCOL(1,3)),
    vec4(ROWCOL(2,0), ROWCOL(2,1), ROWCOL(2,2), ROWCOL(2,3)),
    vec4(ROWCOL(3,0), ROWCOL(3,1), ROWCOL(3,2), ROWCOL(3,3))
    );
}

mat4 operator * (const mat4& a, const double d)
{ return mat4(a.v[0] * d, a.v[1] * d, a.v[2] * d, a.v[3] * d); }

mat4 operator * (const double d, const mat4& a)
{ return a*d; }

mat4 operator / (const mat4& a, const double d)
{ return mat4(a.v[0] / d, a.v[1] / d, a.v[2] / d, a.v[3] / d); }

int operator == (const mat4& a, const mat4& b)
{ return ((a.v[0] == b.v[0]) && (a.v[1] == b.v[1]) && (a.v[2] == b.v[2]) &&
  (a.v[3] == b.v[3])); }

int operator != (const mat4& a, const mat4& b)
{ return !(a == b); }

ostream& operator << (ostream& s, mat4& m)
{ return s << m.v[VX] << '\n' << m.v[VY] << '\n' << m.v[VZ] << '\n' << m.v[VW]; }

istream& operator >> (istream& s, mat4& m)
{
    mat4    m_tmp;

    s >> m_tmp[VX] >> m_tmp[VY] >> m_tmp[VZ] >> m_tmp[VW];
    if (s)
	m = m_tmp;
    return s;
}

void swap(mat4& a, mat4& b)
{ mat4 tmp(a); a = b; b = tmp; }


/****************************************************************
*								*
*	       2D functions and 3D functions			*
*								*
****************************************************************/

mat3 identity2D()
{   return mat3(vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)); }

mat3 translation2D(vec2& v)
{   return mat3(vec3(1.0, 0.0, v[VX]),
		vec3(0.0, 1.0, v[VY]),
		vec3(0.0, 0.0, 1.0)); }

mat3 rotation2D(vec2& Center, const double angleDeg) {
    double  angleRad = angleDeg * M_PI / 180.0,
	    c = cos(angleRad),
	    s = sin(angleRad);

    return mat3(vec3(c, -s, Center[VX] * (1.0-c) + Center[VY] * s),
		vec3(s, c, Center[VY] * (1.0-c) - Center[VX] * s),
		vec3(0.0, 0.0, 1.0));
}

mat3 scaling2D(vec2& scaleVector)
{   return mat3(vec3(scaleVector[VX], 0.0, 0.0),
		vec3(0.0, scaleVector[VY], 0.0),
		vec3(0.0, 0.0, 1.0)); }

mat4 identity3D()
{   return mat4(vec4(1.0, 0.0, 0.0, 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(0.0, 0.0, 0.0, 1.0)); }

mat4 translation3D(vec3& v)
{   return mat4(vec4(1.0, 0.0, 0.0, v[VX]),
		vec4(0.0, 1.0, 0.0, v[VY]),
		vec4(0.0, 0.0, 1.0, v[VZ]),
		vec4(0.0, 0.0, 0.0, 1.0)); }

mat4 rotation3D(vec3& Axis, const double angleDeg) {
    double  angleRad = angleDeg * M_PI / 180.0,
	    c = cos(angleRad),
	    s = sin(angleRad),
	    t = 1.0 - c;

    Axis.normalize();
    return mat4(vec4(t * Axis[VX] * Axis[VX] + c,
		     t * Axis[VX] * Axis[VY] - s * Axis[VZ],
		     t * Axis[VX] * Axis[VZ] + s * Axis[VY],
		     0.0),
		vec4(t * Axis[VX] * Axis[VY] + s * Axis[VZ],
		     t * Axis[VY] * Axis[VY] + c,
		     t * Axis[VY] * Axis[VZ] - s * Axis[VX],
		     0.0),
		vec4(t * Axis[VX] * Axis[VZ] - s * Axis[VY],
		     t * Axis[VY] * Axis[VZ] + s * Axis[VX],
		     t * Axis[VZ] * Axis[VZ] + c,
		     0.0),
		vec4(0.0, 0.0, 0.0, 1.0));
}

mat4 scaling3D(vec3& scaleVector)
{   return mat4(vec4(scaleVector[VX], 0.0, 0.0, 0.0),
		vec4(0.0, scaleVector[VY], 0.0, 0.0),
		vec4(0.0, 0.0, scaleVector[VZ], 0.0),
		vec4(0.0, 0.0, 0.0, 1.0)); }

mat4 perspective3D(const double d)
{   return mat4(vec4(1.0, 0.0, 0.0, 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(0.0, 0.0, 1.0/d, 0.0)); }
