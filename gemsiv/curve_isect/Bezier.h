#ifndef _BEZIER_INCLUDED_
#include "vector.h"
class Bezier {
    public:
    point *p0, *p1, *p2, *p3;
    Bezier()
	{
	p0 = 0; p1 = 0; p2 = 0; p3 = 0;
	}
    Bezier( point *_p0, point *_p1, point *_p2, point *_p3 )
	{
	p0 = _p0; p1 = _p1; p2 = _p2; p3 = _p3;
	}
    Bezier * Split( );
    void ParameterSplitLeft( double t, Bezier &result );

    // Intersect with another curve.  Return two 10-elt arrays. Array 0 
    // contains fragments of self. Array 1 contains fragments of other curve.
    // Fragments continue until one with nil pointers pointing at point data.
    Bezier **Intersect( Bezier B ); 
    ~Bezier()
	{
	if( --p0->refcount <= 0 ) delete p0;
	if( --p1->refcount <= 0 ) delete p1;
	if( --p2->refcount <= 0 ) delete p2;
	if( --p3->refcount <= 0 ) delete p3;
	}
    };
#define _BEZIER_INCLUDED_
#endif
