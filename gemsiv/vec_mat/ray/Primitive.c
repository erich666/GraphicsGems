#include "Primitive.h"

Primitive::~Primitive(){};

/************************************************************************
*									*
* This method receives as parameter a list of intersections and the	*
* number of intersections in the list. It scans the list and returns	*
* the closest positive intersection, or 0.0 if no such intersection	*
* exists.								*
*									*
************************************************************************/

double Primitive::closest_intersection(double *x, int x_num)
{
int	i;
double	x_min = (x_num) ? x[0] : 0.0;

for (i=1; i<x_num; i++)
    if (x[i] < x_min)
	x_min = x[i];
return x_min;
}

/************************************************************************
*									*
* Input from stream.							*
*									*
************************************************************************/

istream& operator >> (istream& s, Primitive& a)
{
s >> *((Object3D*) &a);
s >> a.col;
s >> a.ph;
return s;
}
