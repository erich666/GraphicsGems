// Floating point vector library
#ifndef VECTORS_INCLUDED__
#include <math.h>
class point 
    {
    private:
    int refcount;
    public: 
    double x, y;
    point()
	{
	refcount = 0;
	}
    point( const double _x, const double _y )
	{
	x = _x, y = _y;
	refcount = 1;
	}
    point( const point &p )
	{
	x = p.x, y = p.y; refcount = 1;
	}
    point( const point &p, const int count )
	{
	x = p.x, y = p.y; refcount = count;
	}
    point( const point *a, const point *b, const int count )
	{
	x = ( a->x + b->x ) * 0.5;
	y = ( a->y + b->y ) * 0.5;
	refcount = count;
	}
    friend class Bezier;
    }
;

class vector
    {
    public:
    double x, y;
    vector()
	{
	;
	}
    vector( const vector &v )
	{
	x = v.x, y = v.y;
	}
    vector( const double _x, const double _y )
	{
	x = _x, y = _y;
	}
    }
;

inline vector operator-(const point a, const point b ) // p - p = v
    {
    return vector( a.x - b.x, a.y - b.y );
    }
    
inline vector operator-(const vector a, const vector b ) // v - v = v
    {
    return vector( a.x - b.x, a.y - b.y );
    }
    
inline point operator+(const point *a, const vector b ) // p + v = p
    {
    return point( a->x + b.x, a->y + b.y );
    }
    
inline point operator+(const point a, const vector b ) // p + v = p
    {
    return point( a.x + b.x, a.y + b.y );
    }
    
inline vector operator+(const vector a, const vector b ) // v + v = v
    {
    return vector( a.x + b.x, a.y + b.y );
    }
    
inline vector operator*(const double s, const vector v ) // sv = v
    {
    return vector( s * v.x, s * v.y );
    }
    
inline vector operator*(const vector v, const double s) // v s = v
    {
    return vector( s * v.x, s * v.y );
    }
    
inline double operator*(const vector a, const vector b) // v * v = s (dot product)
    {
    return a.x * b.x + a.y * b.y;
    }
    
inline point mid( const point *a, const point *b ) 
    {
    return point( ( a->x + b->x ) * 0.5, ( a->y + b->y ) * 0.5 );
    }
    
inline vector vabs( const vector a )
    {
    return vector( fabs( a.x ), fabs( a.y ) );
    }
;
#define VECTORS_INCLUDED__
#endif
