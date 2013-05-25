/***************************************************************************

  This is C++ code. Given here are skeleton class definitions for the
  control points (ControlPoint), quadratic and cubic Bezier
  triangles (BezierTri2 and BezierTri3, respectively), and for the
  bi-quadratic and bi-cubic Bezier patches (BezierRect2 and BezierRect3).
  The conversion described in the gem takes place in the constructors
  provided for BezierRect2 and BezierRect3, which each take a Bezier
  triangle (of appropriate degree) as an argument.

  Note that the ControlPoint does not have to be to be an (x,y,z) triplet.
  For instance, it can be a scalar, an RGB triplet, etc., as long as the
  operators +, *, /, and = (assignment) are provided. If you have a
  class that you wish to use instead of the one given in the code, all
  you have to do is to remove the definitions of the ControlPoint class
  and its operators, and insert instead something like:

    #include <my_class.h>
    typedef MyClass ControlPoint;

***************************************************************************/

class ControlPoint {
    friend ControlPoint operator+(const ControlPoint&, const ControlPoint&);
    friend ControlPoint operator*(float, const ControlPoint&);
    friend ControlPoint operator/(const ControlPoint&, float);
private:
    float x, y, z;
public:
    ControlPoint() {}
    ControlPoint(float a, float b, float c)	{ x = a; y = b; z = c; }
};

ControlPoint operator+(const ControlPoint& a, const ControlPoint& b)
{
	return ControlPoint(a.x + b.x, a.y + b.y, a.z + b.z);
}

ControlPoint operator*(float c, const ControlPoint& a)
{
	return ControlPoint(c * a.x, c * a.y, c * a.z);
}

ControlPoint operator/(const ControlPoint& a, float c)
{
	return ControlPoint(a.x / c, a.y / c, a.z / c);
}

class BezierTri2 {
private:
    ControlPoint cp[6];
public:
    const ControlPoint& b(int, int, int) const;
};

const ControlPoint& BezierTri2::b(int i, int j, int /* k */) const
{
    static int row_start[3] = {0, 3, 5};
    return cp[row_start[j] + i];
}

class BezierTri3 {
private:
    ControlPoint cp[10];
public:
    const ControlPoint& b(int, int, int) const;
};

const ControlPoint& BezierTri3::b(int i, int j, int /* k */) const
{
    static int row_start[4] = {0, 4, 7, 9};
    return cp[row_start[j] + i];
}

class BezierRect2 {
private:
    ControlPoint cp[3][3];
public:
    BezierRect2(const BezierTri2&);
    ControlPoint& p(int i, int j)       { return cp[i][j]; }
};

BezierRect2::BezierRect2(const BezierTri2& bt)
// convert a quadratic triangle into a bi-quadratic patch
{
    p(0,0) = bt.b(0,0,2);
    p(0,1) = bt.b(1,0,1);
    p(0,2) = bt.b(2,0,0);
    p(1,0) = bt.b(0,1,1);
    p(1,1) = (bt.b(0,1,1) + bt.b(1,1,0)) / 2;
    p(1,2) = bt.b(1,1,0);
    p(2,0) = p(2,1) = p(2,2) = bt.b(0,2,0);
}

class BezierRect3 {
private:
    ControlPoint cp[4][4];
public:
    BezierRect3(const BezierTri3&);
    ControlPoint& p(int i, int j)       { return cp[i][j]; }
};

BezierRect3::BezierRect3(const BezierTri3& bt)
// convert a cubic triangle into a bi-cubic patch
{
    p(0,0) = bt.b(0,0,3);
    p(0,1) = bt.b(1,0,2);
    p(0,2) = bt.b(2,0,1);
    p(0,3) = bt.b(3,0,0);
    p(1,0) = bt.b(0,1,2);
    p(1,1) = (bt.b(0,1,2) + 2*bt.b(1,1,1)) / 3;
    p(1,2) = (bt.b(2,1,0) + 2*bt.b(1,1,1)) / 3;
    p(1,3) = bt.b(2,1,0);
    p(2,0) = bt.b(0,2,1);
    p(2,1) = (bt.b(1,2,0) + 2*bt.b(0,2,1)) / 3;
    p(2,2) = (bt.b(0,2,1) + 2*bt.b(1,2,0)) / 3;
    p(2,3) = bt.b(1,2,0);
    p(3,0) = p(3,1) = p(3,2) = p(3,3) = bt.b(0,3,0);
}
