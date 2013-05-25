#ifndef GEOM2D_H
#define GEOM2D_H

#include <math.h>
#include <iostream.h>

#ifndef ABS
#define ABS(a)	((a) >= 0 ? (a) : -(a))
#endif

#ifndef MAX
#define MAX(a, b)	((a) >= (b) ? (a) : (b))
#define MIN(a, b)	((a) <= (b) ? (a) : (b))
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#define EPS 1e-6

typedef double	Real;

class Vector2d {
public:
	Real x, y;
	Vector2d()					{ x = 0; y = 0; }
	Vector2d(Real a, Real b)	{ x = a; y = b; }
	Real norm() const;
	void normalize();
	Vector2d operator+(const Vector2d&) const;
	Vector2d operator-(const Vector2d&) const;
	friend Vector2d operator*(Real, const Vector2d&);
	friend Real dot(const Vector2d&, const Vector2d&);
	friend istream& operator>>(istream&, Vector2d&);
	friend ostream& operator<<(ostream&, const Vector2d&);
};

class Point2d {
public:
	Real x, y;
	Point2d()					{ x = 0; y = 0; }
	Point2d(Real a, Real b)		{ x = a; y = b; }
	Point2d(const Point2d& p)	{ *this = p; }
	Point2d operator+(const Vector2d&) const;
	Vector2d operator-(const Point2d&) const;
	int operator==(const Point2d&) const;
	friend istream& operator>>(istream&, Point2d&);
	friend ostream& operator<<(ostream&, const Point2d&);
};

class Line {
public:
	Line()	{}
	Line(const Point2d&, const Point2d&);
	Real eval(const Point2d&) const;
	int classify(const Point2d&) const;
private:
	Real a, b, c;
};

// Vector2d:

inline Real Vector2d::norm() const
{
	return sqrt(x * x + y * y);
}

inline void Vector2d::normalize()
{
	Real len;

	if ((len = sqrt(x * x + y * y)) == 0.0)
		cerr << "Vector2d::normalize: Division by 0\n";
	else {
		x /= len;
		y /= len;
	}
}

inline Vector2d Vector2d::operator+(const Vector2d& v) const
{
	return Vector2d(x + v.x, y + v.y);
}

inline Vector2d Vector2d::operator-(const Vector2d& v) const
{
	return Vector2d(x - v.x, y - v.y);
}

inline Vector2d operator*(Real c, const Vector2d& v)
{
	return Vector2d(c * v.x, c * v.y);
}

inline Real dot(const Vector2d& u, const Vector2d& v)
{
	return u.x * v.x + u.y * v.y;
}

inline ostream& operator<<(ostream& os, const Vector2d& v)
{
	os << '(' << v.x << ", " << v.y << ')';
	return os;
}

inline istream& operator>>(istream& is, Vector2d& v)
{
	is >> v.x >> v.y;
	return is;
}

// Point2d:

inline Point2d Point2d::operator+(const Vector2d& v) const
{
	return Point2d(x + v.x, y + v.y);
}

inline Vector2d Point2d::operator-(const Point2d& p) const
{
	return Vector2d(x - p.x, y - p.y);
}

inline int Point2d::operator==(const Point2d& p) const
{
	return ((*this - p).norm() < EPS);
}

inline istream& operator>>(istream& is, Point2d& p)
{
	is >> p.x >> p.y;
	return is;
}

inline ostream& operator<<(ostream& os, const Point2d& p)
{
	os << '(' << p.x << ", " << p.y << ')';
	return os;
}

// Line:

inline Line::Line(const Point2d& p, const Point2d& q)
// Computes the normalized line equation through the
// points p and q.
{
	Vector2d t = q - p;
	Real len = t.norm();
	a =   t.y / len;
	b = - t.x / len;
	c = -(a*p.x + b*p.y);
}

inline Real Line::eval(const Point2d& p) const
// Plugs point p into the line equation.
{
	return (a * p.x + b* p.y + c);
}

inline int Line::classify(const Point2d& p) const
// Returns -1, 0, or 1, if p is to the left of, on,
// or right of the line, respectively.
{
	Real d = eval(p);
	return (d < -EPS) ? -1 : (d > EPS ? 1 : 0);
}

#endif

