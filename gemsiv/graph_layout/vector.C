/*****************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    MODUL   - 2D VECTOR OPERATIONS
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/
#include <math.h>

#ifdef MSWINDOWS
#include "vector.hxx"
#else
#include "vector.hxx"
#endif

/*------------------ Overloaded + operator  --------------------*/
/* Adds two vectors						*/
/* IN  : reference of the two operands				*/
/* OUT : result vector						*/
/*--------------------------------------------------------------*/
vector operator+(vector& a, vector& b)
{
	vector sum;
	sum.x = a.x + b.x;
	sum.y = a.y + b.y;
	return sum;
}

/*--------------- Overloaded binary - operator	----------------*/
/* Subtract two vectors						*/
/* IN  : reference of the two operands				*/
/* OUT : result vector						*/
/*--------------------------------------------------------------*/
vector operator-(vector& a, vector& b)
{
	vector dif;
	dif.x = a.x - b.x;
	dif.y = a.y - b.y;
	return dif;
}

/*-------------- Overloaded unary - operator -------------------*/
/* Negates a vector						*/
/* IN  : operand						*/
/* OUT : negated vector						*/
/*--------------------------------------------------------------*/
vector operator-(vector& a)
{
	vector neg;
	neg.x = -a.x;
	neg.y = -a.y;
	return neg;
}

/*------------------ Overloaded * operator  --------------------*/
/* Multiplies a vector with a scalar				*/
/* IN  : vector and scalar operand				*/
/* OUT : result vector						*/
/*--------------------------------------------------------------*/
vector operator*(vector& a, double s)
{
	vector scaled;
	scaled.x = s * a.x;
	scaled.y = s * a.y;
	return scaled;
}

vector operator*(double s, vector& a)
{
	vector scaled;
	scaled.x = s * a.x;
	scaled.y = s * a.y;
	return scaled;
}

/*------------------------- Size -------------------------------*/
/* Calculates the absolute value of the vector			*/
/* IN  : -							*/
/* OUT : length							*/
/*--------------------------------------------------------------*/
double vector::Size()
{
	return sqrt( x * x + y * y );
}
