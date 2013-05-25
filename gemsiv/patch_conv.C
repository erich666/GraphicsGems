/*
 * C++ code from the article
 * "Converting Rectangular Patches into Bezier Triangles"
 * by Dani Lischinski, danix@graphics.cornell.edu
 * in "Graphics Gems IV", Academic Press, 1994
 */

/***************************************************************************

  This is C++ code. Given here are class definitions and code for the
  control point (ControlPoint) class, quadratic and quartic Bezier
  triangle classes (BezierTri2 and BezierTri4, respectively), and for the
  bilinear and biquadratic Bezier rectangular patch classes (BezierRect1
  and BezierRect2.)
  The conversion described in the gem takes place in the Convert
  member functions of the BezierRect1 and BezierRect2 classes,
  which each take references to two Bezier triangles (of appropriate
  degree) as an argument.

  Note that control points do not have to be (x,y,z) triplets.
  For instance, they can be scalars, RGB triplets, etc., as long as the
  operators +, *, /, and = (assignment) are provided. If you have a
  class that you wish to use instead of the one given in the code, all
  you have to do is to remove the definitions of the ControlPoint class
  and its operators, and insert instead something like:

    #include <my_class.h>
    typedef MyClass ControlPoint;

***************************************************************************/

#include <stream.h>
#include <stdlib.h>
#include <math.h>

#define FRAND() (random()/2147483648.) /* uniform rand# in [0,1) */

/************* Control Point Class ****************************************/

class ControlPoint {
public:
    ControlPoint()				   { x = 0; y = 0; z = 0; }
    ControlPoint(float a, float b, float c)	   { x = a; y = b; z = c; }
    ControlPoint operator+(const ControlPoint&);
    ControlPoint operator-(const ControlPoint&);
    friend ControlPoint operator*(float, const ControlPoint&);
    friend ControlPoint operator/(const ControlPoint&, float);
    friend ostream& operator<<(ostream&, const ControlPoint&);
private:
    float x, y, z;
};

inline ControlPoint ControlPoint::operator+(const ControlPoint& p)
{
    return ControlPoint(x + p.x, y + p.y, z + p.z);
}

inline ControlPoint ControlPoint::operator-(const ControlPoint& p)
{
    return ControlPoint(x - p.x, y - p.y, z - p.z);
}

inline ControlPoint operator*(float c, const ControlPoint& a)
{
    return ControlPoint(c * a.x, c * a.y, c * a.z);
}

inline ControlPoint operator/(const ControlPoint& a, float c)
{
    return ControlPoint(a.x / c, a.y / c, a.z / c);
}

inline ostream& operator<<(ostream& os, const ControlPoint& p)
{
    return os << '(' << p.x << ',' << p.y << ',' << p.z << ") ";
}

/************* Quadratic Bezier Triangle Class ****************************/

class BezierTri2 {
private:
    ControlPoint cp[6];
public:
    ControlPoint& b(int, int, int);
    ControlPoint operator()(float, float);
};

ControlPoint& BezierTri2::b(int i, int j, int /* k */)
// Returns the (i,j,k) control point.
{
    static int row_start[3] = {0, 3, 5};
    return cp[row_start[j] + i];
}

ControlPoint BezierTri2::operator()(float u, float v)
// Evaluates the Bezier triangle at (u,v).
{
    float w = 1 - u - v;
    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;
    return (w2*b(0,0,2) + (2*u*w)*b(1,0,1) + u2*b(2,0,0) +
	    (2*v*w)*b(0,1,1) + (2*u*v)*b(1,1,0) + v2*b(0,2,0));
}

/************* Quartic Bezier Triangle Class ******************************/

class BezierTri4 {
private:
    ControlPoint cp[15];
public:
    ControlPoint& b(int, int, int);
    ControlPoint operator()(float, float);
};

ControlPoint& BezierTri4::b(int i, int j, int /* k */)
// Returns the (i,j,k) control point.
{
    static int row_start[5] = {0, 5, 9, 12, 14};
    return cp[row_start[j] + i];
}

ControlPoint BezierTri4::operator()(float u, float v)
// Evaluates the Bezier triangle at (u,v).
{
    float w = 1 - u - v;
    float u2 = u * u, u3 = u2 * u, u4 = u3 * u;
    float v2 = v * v, v3 = v2 * v, v4 = v3 * v;
    float w2 = w * w, w3 = w2 * w, w4 = w3 * w;
    return (w4*b(0,0,4) + (4*u*w3)*b(1,0,3) + (6*u2*w2)*b(2,0,2) +
	    (4*u3*w)*b(3,0,1) + u4*b(4,0,0) + (4*v*w3)*b(0,1,3) +
	    (12*u*v*w2)*b(1,1,2) + (12*u2*v*w)*b(2,1,1) + (4*u3*v)*b(3,1,0) +
	    (6*v2*w2)*b(0,2,2) + (12*u*v2*w)*b(1,2,1) + (6*u2*v2)*b(2,2,0) +
	    (4*v3*w)*b(0,3,1) + (4*u*v3)*b(1,3,0) + v4*b(0,4,0));
}

/************* Bilinear Bezier Rectangle Class *****************************/

class BezierRect1 {
private:
    ControlPoint cp[2][2];
public:
    ControlPoint& p(int i, int j)	{ return cp[i][j]; }
    ControlPoint operator()(float, float);
    void Convert(BezierTri2&, BezierTri2&);
};

ControlPoint BezierRect1::operator()(float s, float t)
// Evaluates the Bezier rectangle at (s,t).
{
    float s1 = 1 - s;

    return ((1-t) * (s1*p(0,0) + s*p(0,1)) +
		t * (s1*p(1,0) + s*p(1,1)));
}

void BezierRect1::Convert(BezierTri2& t1, BezierTri2& t2)
// Converts a bilinear Bezier rectangle into two quadratic Bezier
// triangles t1 and t2, such that the value of the bilinear
// at (s,t) is equal to t1(s,t) if (s + t <= 1), and t2(1-t,1-s)
// otherwise.
{
    // lower left triangle:
    t1.b(0,0,2) = p(0,0);
    t1.b(1,0,1) = 0.5 * (p(0,0) + p(0,1));
    t1.b(2,0,0) = p(0,1);

    t1.b(0,1,1) = 0.5 * (p(0,0) + p(1,0));
    t1.b(1,1,0) = 0.5 * (p(0,0) + p(1,1));

    t1.b(0,2,0) = p(1,0);

    // upper right triangle:
    t2.b(0,0,2) = p(1,1);
    t2.b(1,0,1) = 0.5 * (p(1,1) + p(0,1));
    t2.b(2,0,0) = p(0,1);

    t2.b(0,1,1) = 0.5 * (p(1,1) + p(1,0));
    t2.b(1,1,0) = 0.5 * (p(0,0) + p(1,1));

    t2.b(0,2,0) = p(1,0);
}

/************* Biquadratic Bezier Rectangle Class **************************/

class BezierRect2 {
private:
    ControlPoint cp[3][3];
public:
    ControlPoint& p(int i, int j)	{ return cp[i][j]; }
    ControlPoint operator()(float, float);
    void Convert(BezierTri4&, BezierTri4&);
};

ControlPoint BezierRect2::operator()(float s, float t)
// Evaluates the Bezier rectangle at (s,t).
{
    float s1 = 1 - s, ss1 = 2*s*s1, s2 = s*s, s12 = s1*s1;
    float t1 = 1 - t, tt1 = 2*t*t1, t2 = t*t, t12 = t1*t1;

    return (t12 * (s12*p(0,0) + ss1*p(0,1) + s2*p(0,2)) +
	    tt1 * (s12*p(1,0) + ss1*p(1,1) + s2*p(1,2)) +
	    t2	* (s12*p(2,0) + ss1*p(2,1) + s2*p(2,2)));
}

void BezierRect2::Convert(BezierTri4& t1, BezierTri4& t2)
// Converts a biquadratic Bezier rectangle into two quartic Bezier
// triangles t1 and t2, such that the value of the biquadratic
// at (s,t) is equal to t1(s,t) if (s + t <= 1), and t2(1-t,1-s)
// otherwise.
{
    // lower left triangle:
    t1.b(0,0,4) = p(0,0);
    t1.b(1,0,3) = 0.5 * (p(0,0) + p(0,1));
    t1.b(2,0,2) = (p(0,0) + 4 * p(0,1) + p(0,2)) / 6;
    t1.b(3,0,1) = 0.5 * (p(0,1) + p(0,2));
    t1.b(4,0,0) = p(0,2);

    t1.b(0,1,3) = 0.5 * (p(0,0) + p(1,0));
    t1.b(1,1,2) = (p(0,0) + p(1,1)) / 3 + (p(0,1) + p(1,0)) / 6;
    t1.b(2,1,1) = (p(0,0) + p(1,2)) / 6 + (p(0,1) + p(1,1)) / 3;
    t1.b(3,1,0) = 0.5 * (p(0,1) + p(1,2));

    t1.b(0,2,2) = (p(0,0) + 4 * p(1,0) + p(2,0)) / 6;
    t1.b(1,2,1) = (p(0,0) + p(2,1)) / 6 + (p(1,0) + p(1,1)) / 3;
    t1.b(2,2,0) = (p(0,0) + 4 * p(1,1) + p(2,2)) / 6;

    t1.b(0,3,1) = 0.5 * (p(1,0) + p(2,0));
    t1.b(1,3,0) = 0.5 * (p(1,0) + p(2,1));

    t1.b(0,4,0) = p(2,0);

    // upper right triangle:
    t2.b(0,0,4) = p(2,2);
    t2.b(1,0,3) = 0.5 * (p(2,2) + p(1,2));
    t2.b(2,0,2) = (p(2,2) + 4 * p(1,2) + p(0,2)) / 6;
    t2.b(3,0,1) = 0.5 * (p(1,2) + p(0,2));
    t2.b(4,0,0) = p(0,2);

    t2.b(0,1,3) = 0.5 * (p(2,2) + p(2,1));
    t2.b(1,1,2) = (p(2,2) + p(1,1)) / 3 + (p(1,2) + p(2,1)) / 6;
    t2.b(2,1,1) = (p(2,2) + p(0,1)) / 6 + (p(1,2) + p(1,1)) / 3;
    t2.b(3,1,0) = 0.5 * (p(0,1) + p(1,2));

    t2.b(0,2,2) = (p(2,2) + 4 * p(2,1) + p(2,0)) / 6;
    t2.b(1,2,1) = (p(2,2) + p(1,0)) / 6 + (p(2,1) + p(1,1)) / 3;
    t2.b(2,2,0) = (p(2,2) + 4 * p(1,1) + p(0,0)) / 6;

    t2.b(0,3,1) = 0.5 * (p(2,1) + p(2,0));
    t2.b(1,3,0) = 0.5 * (p(1,0) + p(2,1));

    t2.b(0,4,0) = p(2,0);
}

/************ A Test Program **********************************************/

void main()
{
    // init biquadratic Bezier rectangle:
    BezierRect2 brect;
    brect.p(0,0) = ControlPoint(FRAND(), FRAND(), FRAND());
    brect.p(0,1) = ControlPoint(FRAND(), FRAND(), FRAND());
    brect.p(0,2) = ControlPoint(FRAND(), FRAND(), FRAND());
    brect.p(1,0) = ControlPoint(FRAND(), FRAND(), FRAND());
    brect.p(1,1) = ControlPoint(FRAND(), FRAND(), FRAND());
    brect.p(1,2) = ControlPoint(FRAND(), FRAND(), FRAND());
    brect.p(2,0) = ControlPoint(FRAND(), FRAND(), FRAND());
    brect.p(2,1) = ControlPoint(FRAND(), FRAND(), FRAND());
    brect.p(2,2) = ControlPoint(FRAND(), FRAND(), FRAND());

    // convert to two quartic Bezier triangles:
    BezierTri4 btri1, btri2;
    brect.Convert(btri1, btri2);

    float s, t;
    ControlPoint pt1, pt2;

    while (cin >> s >> t) {
	pt1 = brect(s, t);
	if (s + t <= 1.0) {
	    pt2 = btri1(s, t);
	} else {
	    pt2 = btri2(1-t, 1-s);
	}
	cerr << pt1 - pt2 << endl;
    }
}
