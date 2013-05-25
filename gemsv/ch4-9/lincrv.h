/****** lincrv.h ******/
/* Ken Shoemake, 1994 */

#define MAXDIM 2
typedef float Vect[MAXDIM];
typedef float Knot;
typedef int Bool;

int DialASpline(Knot t, Knot a[], Vect p[], int m, int n, Vect work[],
                unsigned int Cn, Bool interp, Vect val);

