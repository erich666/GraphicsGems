// ******************************************************************
//
// Physically Correct Direct Lighting For Distribution Ray Tracing
//             by Changyaw Wang
//
// utility.h
//
// ******************************************************************

#include <math.h>
#include <stream.h>

class point;
class vector;
class rotation_matrix;

class point {
public:
    double data[3];
    point();
    point(double, double, double);
    point& operator=(const point&);
};

class vector {
public:
    double data[3];
    vector();
    vector(double,double,double);
    vector& operator=(const vector&);
    void normalize();
    friend vector operator*(double, const vector&);
};

class rotation_matrix {
protected:
    double data[4][4];
public:
    void set_identity();
    void set_xyz_to_uvw(const vector&, const vector&, const vector&);
    friend vector operator*(const rotation_matrix&, const vector&);
};

inline point::point() {}

inline vector::vector() {}

inline point& point::operator=(const point& v)
{
   data[0] = v.data[0];
   data[1] = v.data[1];
   data[2] = v.data[2];
   return *this;
}

inline point::point(double a, double b, double c)
{
   data[0] = a; data[1] = b; data[2] = c;
 }

inline vector::vector(double a, double b, double c)
{
   data[0] = a; data[1] = b; data[2] = c;
}

inline vector& vector::operator=(const vector& v)
{
   data[0] = v.data[0];
   data[1] = v.data[1];
   data[2] = v.data[2];
   return *this;
}

inline vector operator*(double d, const vector& v)
{
   return vector(d * v.data[0], 
		 d * v.data[1],
		 d * v.data[2] );
 }

inline void vector::normalize()
{
    double length;

    length = sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
    if (length <= 1.0e-6)
    {
        data[0] = 1.0; data[1] = 0.0; data[2] = 0.0;
    }
    else
    {
        length = 1.0 / length;
        data[0] *= length; data[1] *= length; data[2] *= length;
    }
}

inline void rotation_matrix::set_identity()
{
    data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0;
    data[0][1] = data[0][2] = data[0][3] = 0.0;
    data[1][0] = data[1][2] = data[1][3] = 0.0;
    data[2][0] = data[2][1] = data[2][3] = 0.0;
    data[3][0] = data[3][1] = data[3][2] = 0.0;
}

void rotation_matrix::set_xyz_to_uvw(const vector& u, const vector& v,
                                     const vector& w)
{
      this->set_identity();
      data[0][0] = u.data[0];
      data[1][0] = u.data[1];
      data[2][0] = u.data[2];

      data[0][1] = v.data[0];
      data[1][1] = v.data[1];
      data[2][1] = v.data[2];


      data[0][2] = w.data[0];
      data[1][2] = w.data[1];
      data[2][2] = w.data[2];
}

inline point operator+(const point& p, const vector& v)
{
    return  point(p.data[0] + v.data[0], p.data[1] + v.data[1], 
		  p.data[2] + v.data[2]);
}

inline vector operator*(const rotation_matrix& r, const vector& v)
{
  return vector(r.data[0][0]*v.data[0] + r.data[0][1]*v.data[1] +
                   r.data[0][2]*v.data[2] + r.data[0][3],
                r.data[1][0]*v.data[0] + r.data[1][1]*v.data[1] +
                   r.data[1][2]*v.data[2] + r.data[1][3],
                r.data[2][0]*v.data[0] + r.data[2][1]*v.data[1] +
                   r.data[2][2]*v.data[2] + r.data[2][3]);
}

inline vector operator-(const point& u, const point& v)
{
    return vector(u.data[0]-v.data[0], u.data[1]-v.data[1], 
		  u.data[2]-v.data[2]);
}

inline double dot(const vector& a, const vector& b)
{
   return ( a.data[0]*b.data[0] + a.data[1]*b.data[1] + 
	   a.data[2] * b.data[2] );
}

inline double distance_squared(const point& a, const point& b)
{
   double x,y,z;
   x = a.data[0] - b.data[0];
   y = a.data[1] - b.data[1];
   z = a.data[2] - b.data[2];
   return x*x + y*y + z*z;
}

inline double distance(const point& a, const point& b)
{
   double t;
   t = distance_squared(a,b);
   return sqrt(t);
}

inline vector spherical_to_vector(double theta, double phi)
{
    return vector(sin(theta)*cos(phi),
                  sin(theta)*sin(phi),
                  cos(theta));
}

inline vector cross(const vector& a, const vector &b)
{
    return vector(a.data[1]*b.data[2] - a.data[2]*b.data[1],
                  a.data[2]*b.data[0] - a.data[0]*b.data[2],
                  a.data[0]*b.data[1] - a.data[1]*b.data[0]);

}

#include "geometry_object.h"
#include "sphere_luminaire.h"
#include "triangle_luminaire.h"

