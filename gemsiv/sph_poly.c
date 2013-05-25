/*
 * ANSI C code from the article
 * "Computing the Area of a Spherical Polygon"
 * by Robert D. Miller
 * in "Graphics Gems IV", Academic Press, 1994
 */

/*
 * to test, compile with "cc -DMAIN -o spherical_poly spherical_poly.c -lm"
 * to create subroutine, compile with "cc -c spherical_poly.c"
 */

#include <math.h>
static const double
    HalfPi= 1.5707963267948966192313,
    Degree= 57.295779513082320876798;	/* degrees per radian */

double Hav(double X)
/*  Haversine function: hav(x)= (1-cos(x))/2.  */
{
    return (1.0 -cos(X))/2.0;
}

double SphericalPolyArea(double *Lat, double *Lon, int N)
/*  Returns the area of a spherical polygon in spherical degrees,
     given the latitudes and longitudes in Lat and Lon, respectively.
     The data points have indices which range from 0 to N. */
{
int  J, K;
double
    Lam1, Lam2, Beta1, Beta2,
    CosB1, CosB2, HavA,
    T, A, B, C, S, Sum, Excess;

    Sum= 0;
    for (J= 0; J <= N; J++)
    {
	K= J+1;
	if (J == 0)
	{
	    Lam1= Lon[J];	Beta1= Lat[J];
	    Lam2= Lon[J+1];	Beta2= Lat[J+1];
	    CosB1= cos(Beta1);	CosB2= cos(Beta2);
	}
	else
	{
	    K= (J+1) % (N+1);
	    Lam1= Lam2;		Beta1= Beta2;
	    Lam2= Lon[K];	Beta2= Lat[K];
	    CosB1= CosB2;	CosB2= cos(Beta2);
	}

	if (Lam1 != Lam2)
	{
	    HavA= Hav(Beta2-Beta1) +CosB1*CosB2*Hav(Lam2-Lam1);
	    A= 2*asin(sqrt(HavA));
	    B= HalfPi -Beta2;
	    C= HalfPi -Beta1;
	    S= 0.5*(A+B+C);
	    T= tan(S/2) * tan((S-A)/2) * tan((S-B)/2) * tan((S-C)/2);

	    Excess= fabs(4*atan(sqrt(fabs(T))))*Degree;
	    if (Lam2 < Lam1) Excess= -Excess;

	    Sum= Sum + Excess;
	}
    }
    return fabs(Sum);
}   /*	SphericalPolyArea. */

#ifdef MAIN

#include <stdlib.h>
#include <stdio.h>

double SphericalPolyArea(double *Lat, double *Lon, int N);

double
    SqMi= 273218.4, /* Square mi per spherical degree. */
    SqKm= 707632.4; /* Square km per spherical degree. */

main()	     /*	 Spherical Polygon Area test routine */
{
double	Lat[100], Lon[100];
double	P, Q, Area;
int	K, LastPt;

    LastPt= -1;
    printf("Enter Longitude Latitude. End with: 0 0.\n");
    while (1)
	{
	scanf("%lf %lf",&P,&Q);
	if ((P == 0.0) && (Q == 0.0))
	break;
	Lon[++LastPt]= P;  Lat[LastPt]= Q;
	}

    for (K= 0; K <= LastPt; K++)
	{
	Lat[K]= Lat[K]/Degree;	Lon[K]= Lon[K]/Degree;
	}

    Area= SphericalPolyArea(Lat, Lon, LastPt);
    printf("	 Area=%12.4lf sq mi., %12.4lf spherical degrees.\n",
	   Area*SqMi, Area);
    return 0;
}

#endif
