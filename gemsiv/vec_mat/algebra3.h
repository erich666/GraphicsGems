/****************************************************************
*								*
* C++ Vector and Matrix Algebra routines			*
* Author: Jean-Francois DOUE					*
* Version 3.1 --- October 1993					*
*								*
****************************************************************/

#include <stream.h>
#include <stdlib.h>

// this line defines a new type: pointer to a function which returns a
// double and takes as argument a double
typedef double (*V_FCT_PTR)(double);

// min-max macros
#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define MAX(A,B) ((A) > (B) ? (A) : (B))

// error handling macro
#define V_ERROR(E) { cerr << E; exit(1); }

class vec2;
class vec3;
class vec4;
class mat3;
class mat4;

enum {VX, VY, VZ, VW};		    // axes
enum {PA, PB, PC, PD};		    // planes
enum {RED, GREEN, BLUE};	    // colors
enum {KA, KD, KS, ES};		    // phong coefficients

/****************************************************************
*								*
*			    2D Vector				*
*								*
****************************************************************/

class vec2
{
protected:

 double n[2];

public:

// Constructors

vec2();
vec2(const double x, const double y);
vec2(const double d);
vec2(const vec2& v);			// copy constructor
vec2(const vec3& v);			// cast v3 to v2
vec2(const vec3& v, int dropAxis);	// cast v3 to v2

// Assignment operators

vec2& operator	= ( const vec2& v );	// assignment of a vec2
vec2& operator += ( const vec2& v );	// incrementation by a vec2
vec2& operator -= ( const vec2& v );	// decrementation by a vec2
vec2& operator *= ( const double d );	// multiplication by a constant
vec2& operator /= ( const double d );	// division by a constant
double& operator [] ( int i);		// indexing

// special functions

double length();			// length of a vec2
double length2();			// squared length of a vec2
vec2& normalize();			// normalize a vec2
vec2& apply(V_FCT_PTR fct);		// apply a func. to each component

// friends

friend vec2 operator - (const vec2& v);			    // -v1
friend vec2 operator + (const vec2& a, const vec2& b);	    // v1 + v2
friend vec2 operator - (const vec2& a, const vec2& b);	    // v1 - v2
friend vec2 operator * (const vec2& a, const double d);	    // v1 * 3.0
friend vec2 operator * (const double d, const vec2& a);	    // 3.0 * v1
friend vec2 operator * (const mat3& a, const vec2& v);	    // M . v
friend vec2 operator * (const vec2& v, mat3& a);	    // v . M
friend double operator * (const vec2& a, const vec2& b);    // dot product
friend vec2 operator / (const vec2& a, const double d);	    // v1 / 3.0
friend vec3 operator ^ (const vec2& a, const vec2& b);	    // cross product
friend int operator == (const vec2& a, const vec2& b);	    // v1 == v2 ?
friend int operator != (const vec2& a, const vec2& b);	    // v1 != v2 ?
friend ostream& operator << (ostream& s, vec2& v);	    // output to stream
friend istream& operator >> (istream& s, vec2& v);	    // input from strm.
friend void swap(vec2& a, vec2& b);			    // swap v1 & v2
friend vec2 min(const vec2& a, const vec2& b);		    // min(v1, v2)
friend vec2 max(const vec2& a, const vec2& b);		    // max(v1, v2)
friend vec2 prod(const vec2& a, const vec2& b);		    // term by term *

// necessary friend declarations

friend class vec3;
};

/****************************************************************
*								*
*			    3D Vector				*
*								*
****************************************************************/

class vec3
{
protected:

 double n[3];

public:

// Constructors

vec3();
vec3(const double x, const double y, const double z);
vec3(const double d);
vec3(const vec3& v);			    // copy constructor
vec3(const vec2& v);			    // cast v2 to v3
vec3(const vec2& v, double d);		    // cast v2 to v3
vec3(const vec4& v);			    // cast v4 to v3
vec3(const vec4& v, int dropAxis);	    // cast v4 to v3

// Assignment operators

vec3& operator	= ( const vec3& v );	    // assignment of a vec3
vec3& operator += ( const vec3& v );	    // incrementation by a vec3
vec3& operator -= ( const vec3& v );	    // decrementation by a vec3
vec3& operator *= ( const double d );	    // multiplication by a constant
vec3& operator /= ( const double d );	    // division by a constant
double& operator [] ( int i);		    // indexing

// special functions

double length();			    // length of a vec3
double length2();			    // squared length of a vec3
vec3& normalize();			    // normalize a vec3
vec3& apply(V_FCT_PTR fct);		    // apply a func. to each component

// friends

friend vec3 operator - (const vec3& v);			    // -v1
friend vec3 operator + (const vec3& a, const vec3& b);	    // v1 + v2
friend vec3 operator - (const vec3& a, const vec3& b);	    // v1 - v2
friend vec3 operator * (const vec3& a, const double d);	    // v1 * 3.0
friend vec3 operator * (const double d, const vec3& a);	    // 3.0 * v1
friend vec3 operator * (const mat4& a, const vec3& v);	    // M . v
friend vec3 operator * (const vec3& v, mat4& a);	    // v . M
friend double operator * (const vec3& a, const vec3& b);    // dot product
friend vec3 operator / (const vec3& a, const double d);	    // v1 / 3.0
friend vec3 operator ^ (const vec3& a, const vec3& b);	    // cross product
friend int operator == (const vec3& a, const vec3& b);	    // v1 == v2 ?
friend int operator != (const vec3& a, const vec3& b);	    // v1 != v2 ?
friend ostream& operator << (ostream& s, vec3& v);	    // output to stream
friend istream& operator >> (istream& s, vec3& v);	    // input from strm.
friend void swap(vec3& a, vec3& b);			    // swap v1 & v2
friend vec3 min(const vec3& a, const vec3& b);		    // min(v1, v2)
friend vec3 max(const vec3& a, const vec3& b);		    // max(v1, v2)
friend vec3 prod(const vec3& a, const vec3& b);		    // term by term *

// necessary friend declarations

friend class vec2;
friend class vec4;
friend class mat3;
friend vec2 operator * (const mat3& a, const vec2& v);	    // linear transform
friend mat3 operator * (mat3& a, mat3& b);		    // matrix 3 product
};

/****************************************************************
*								*
*			    4D Vector				*
*								*
****************************************************************/

class vec4
{
protected:

 double n[4];

public:

// Constructors

vec4();
vec4(const double x, const double y, const double z, const double w);
vec4(const double d);
vec4(const vec4& v);			    // copy constructor
vec4(const vec3& v);			    // cast vec3 to vec4
vec4(const vec3& v, const double d);	    // cast vec3 to vec4

// Assignment operators

vec4& operator	= ( const vec4& v );	    // assignment of a vec4
vec4& operator += ( const vec4& v );	    // incrementation by a vec4
vec4& operator -= ( const vec4& v );	    // decrementation by a vec4
vec4& operator *= ( const double d );	    // multiplication by a constant
vec4& operator /= ( const double d );	    // division by a constant
double& operator [] ( int i);		    // indexing

// special functions

double length();			    // length of a vec4
double length2();			    // squared length of a vec4
vec4& normalize();			    // normalize a vec4
vec4& apply(V_FCT_PTR fct);		    // apply a func. to each component

// friends

friend vec4 operator - (const vec4& v);			    // -v1
friend vec4 operator + (const vec4& a, const vec4& b);	    // v1 + v2
friend vec4 operator - (const vec4& a, const vec4& b);	    // v1 - v2
friend vec4 operator * (const vec4& a, const double d);	    // v1 * 3.0
friend vec4 operator * (const double d, const vec4& a);	    // 3.0 * v1
friend vec4 operator * (const mat4& a, const vec4& v);	    // M . v
friend vec4 operator * (const vec4& v, mat4& a);	    // v . M
friend double operator * (const vec4& a, const vec4& b);    // dot product
friend vec4 operator / (const vec4& a, const double d);	    // v1 / 3.0
friend int operator == (const vec4& a, const vec4& b);	    // v1 == v2 ?
friend int operator != (const vec4& a, const vec4& b);	    // v1 != v2 ?
friend ostream& operator << (ostream& s, vec4& v);	    // output to stream
friend istream& operator >> (istream& s, vec4& v);	    // input from strm.
friend void swap(vec4& a, vec4& b);			    // swap v1 & v2
friend vec4 min(const vec4& a, const vec4& b);		    // min(v1, v2)
friend vec4 max(const vec4& a, const vec4& b);		    // max(v1, v2)
friend vec4 prod(const vec4& a, const vec4& b);		    // term by term *

// necessary friend declarations

friend class vec3;
friend class mat4;
friend vec3 operator * (const mat4& a, const vec3& v);	    // linear transform
friend mat4 operator * (mat4& a, mat4& b);		    // matrix 4 product
};

/****************************************************************
*								*
*			   3x3 Matrix				*
*								*
****************************************************************/

class mat3
{
protected:

 vec3 v[3];

public:

// Constructors

mat3();
mat3(const vec3& v0, const vec3& v1, const vec3& v2);
mat3(const double d);
mat3(const mat3& m);

// Assignment operators

mat3& operator	= ( const mat3& m );	    // assignment of a mat3
mat3& operator += ( const mat3& m );	    // incrementation by a mat3
mat3& operator -= ( const mat3& m );	    // decrementation by a mat3
mat3& operator *= ( const double d );	    // multiplication by a constant
mat3& operator /= ( const double d );	    // division by a constant
vec3& operator [] ( int i);		    // indexing

// special functions

mat3 transpose();			    // transpose
mat3 inverse();				    // inverse
mat3& apply(V_FCT_PTR fct);		    // apply a func. to each element

// friends

friend mat3 operator - (const mat3& a);			    // -m1
friend mat3 operator + (const mat3& a, const mat3& b);	    // m1 + m2
friend mat3 operator - (const mat3& a, const mat3& b);	    // m1 - m2
friend mat3 operator * (mat3& a, mat3& b);		    // m1 * m2
friend mat3 operator * (const mat3& a, const double d);	    // m1 * 3.0
friend mat3 operator * (const double d, const mat3& a);	    // 3.0 * m1
friend mat3 operator / (const mat3& a, const double d);	    // m1 / 3.0
friend int operator == (const mat3& a, const mat3& b);	    // m1 == m2 ?
friend int operator != (const mat3& a, const mat3& b);	    // m1 != m2 ?
friend ostream& operator << (ostream& s, mat3& m);	    // output to stream
friend istream& operator >> (istream& s, mat3& m);	    // input from strm.
friend void swap(mat3& a, mat3& b);			    // swap m1 & m2

// necessary friend declarations

friend vec3 operator * (const mat3& a, const vec3& v);	    // linear transform
friend vec2 operator * (const mat3& a, const vec2& v);	    // linear transform
};

/****************************************************************
*								*
*			   4x4 Matrix				*
*								*
****************************************************************/

class mat4
{
protected:

 vec4 v[4];

public:

// Constructors

mat4();
mat4(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3);
mat4(const double d);
mat4(const mat4& m);

// Assignment operators

mat4& operator	= ( const mat4& m );	    // assignment of a mat4
mat4& operator += ( const mat4& m );	    // incrementation by a mat4
mat4& operator -= ( const mat4& m );	    // decrementation by a mat4
mat4& operator *= ( const double d );	    // multiplication by a constant
mat4& operator /= ( const double d );	    // division by a constant
vec4& operator [] ( int i);		    // indexing

// special functions

mat4 transpose();			    // transpose
mat4 inverse();				    // inverse
mat4& apply(V_FCT_PTR fct);		    // apply a func. to each element

// friends

friend mat4 operator - (const mat4& a);			    // -m1
friend mat4 operator + (const mat4& a, const mat4& b);	    // m1 + m2
friend mat4 operator - (const mat4& a, const mat4& b);	    // m1 - m2
friend mat4 operator * (mat4& a, mat4& b);		    // m1 * m2
friend mat4 operator * (const mat4& a, const double d);	    // m1 * 4.0
friend mat4 operator * (const double d, const mat4& a);	    // 4.0 * m1
friend mat4 operator / (const mat4& a, const double d);	    // m1 / 3.0
friend int operator == (const mat4& a, const mat4& b);	    // m1 == m2 ?
friend int operator != (const mat4& a, const mat4& b);	    // m1 != m2 ?
friend ostream& operator << (ostream& s, mat4& m);	    // output to stream
friend istream& operator >> (istream& s, mat4& m);	    // input from strm.
friend void swap(mat4& a, mat4& b);			    // swap m1 & m2

// necessary friend declarations

friend vec4 operator * (const mat4& a, const vec4& v);	    // linear transform
friend vec3 operator * (const mat4& a, const vec3& v);	    // linear transform
};

/****************************************************************
*								*
*	       2D functions and 3D functions			*
*								*
****************************************************************/

mat3 identity2D();					    // identity 2D
mat3 translation2D(vec2& v);				    // translation 2D
mat3 rotation2D(vec2& Center, const double angleDeg);	    // rotation 2D
mat3 scaling2D(vec2& scaleVector);			    // scaling 2D
mat4 identity3D();					    // identity 3D
mat4 translation3D(vec3& v);				    // translation 3D
mat4 rotation3D(vec3& Axis, const double angleDeg);	    // rotation 3D
mat4 scaling3D(vec3& scaleVector);			    // scaling 3D
mat4 perspective3D(const double d);			    // perspective 3D
