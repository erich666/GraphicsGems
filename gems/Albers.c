/*
Albers Equal-Area Conic Map Projection
by Paul Bame
from "Graphics Gems", Academic Press, 1990
*/

/*
 * Albers Conic Equal-Area Projection
 * Formulae taken from "Map Projections Used by the U.S. 
 * Geological Survey" Bulletin #1532
 *
 * Equation reference numbers and some variable names taken
 * from the reference.
 * To use, call albers setup() once and then albers_project()
 * for each coordinate pair of interest.
*/

#include "GraphicsGems.h"		
#include <stdio.h>
#include <math.h>

/*
 * This is the Clarke 1866 Earth spheroid data which is often
 * used by the USGS - there are other spheroids however - see the
 * book.
 */
 
/*
 * Earth radii in different units */
#define CONST_EradiusKM (6378.2064) 	/* Kilometers */
#define CONST_EradiusMI (CONST_EradiusKM/1.609)	/* Miles */
#define CONST_Ec		(0.082271854)	/* Eccentricity */
#define CONST_Ecsq	(0.006768658)	/* Eccentricity squared */



/*
 * To keep things simple, assume Earth radius is 1.0. Projected
 * coordinates (X and Y obtained from albers project ()) are
 *  dimensionless and may be multiplied by any desired radius
 *  to convert to desired units (usually Kilometers or Miles).
*/
#define CONST_Eradius 1.0

/* Pre-computed variables */
static double middlelon;		/* longitude at center of map */
static double bigC, cone_const, r0;	/* See the reference */

static
calc_q_msq(lat, qp, msqp)
double lat;
double *qp;
double *msqp;
/*
 * Given latitude, calculate 'q' [eq 3-12] 
 * if msqp is != NULL, m^2  [eq. 12-15].
*/
{
	double s, c, es;

	s = sin(lat);
	es = s * CONST_Ec;

	*qp = (1.0 - CONST_Ecsq) * ((s / (1 - es * es))-
		(1 / (2 * CONST_Ec)) * log((1 - es) / (1 + es)));

	if (msqp != NULL)
	{
		c = cos(lat);
		*msqp = c * c/ (1 - es * es);
	}
}




albers_setup(southlat, northlat, originlon, originlat)
double southlat, northlat;
double originlon;
double originlat;
/*
 * Pre-compute a bunch of variables which are used by the 
 * albers_project()
 *
 * southlat 	Southern latitude for Albers projection
 * northlat	Northern latitude for Albers projection
 * originlon	Longitude for origin of projected map
 * originlat	Latitude for origin of projected map -
 *				often (northlat + southlat) / 2
*/
{
	double q1, q2, q0;
	double m1sq, m2sq;

	middlelon = originlon;
	
	calc_q_msq(southlat, &q1, &m1sq);
	calc_q_msq(northlat, &q2, &m2sq);
	calc_q_msq(originlat, &q0, (double *)NULL);

	cone_const = (m1sq - m2sq) / (q2 - q1);
	bigC = m1sq + cone_const * q1;
	r0 = CONST_Eradius * sqrt(bigC - cone_const *q0) / cone_const;
}

/***************************************************************/

albers_project(lon, lat, xp, yp)
double lon, lat;
double *xp, *yp;
/*
 * Project lon/lat (in radians) according to albers_setup and 
 * return the results via xp, yp. Units of *xp and *yp are same
 * as the units used by CONST_Eradius.
*/
{
	double q, r, theta;
	calc_q_msq(lat, &q, (double *)NULL);
	theta = cone_const * (lon -middlelon);
	r = CONST_Eradius * sqrt(bigC - cone_const * q) / cone_const;
	*xp = r * sin(theta);
	*yp = r0 - r * cos(theta);
}

#ifdef TESTPROGRAM

/*
 * Test value from the USGS book. Because of roundoff, the 
 * actual values are printed for visual inspection rather 
 * than guessing what constitutes "close enough".
*/
/* Convert a degrees, minutes pair to radians */
#define DEG_MIN2RAD(degrees, minutes) \
	((double) ((degrees + minutes / 60.0) * M_PI / 180.0))

#define Nlat DEG_MIN2RAD(29, 30)	/* 29 degrees, 30' North Lat */
#define Slat DEG_MIN2RAD(45, 30)	
#define Originlat DEG_MIN2RAD(23, 0)	
#define Originlon DEG_MIN2RAD(-96, 0) /* West longitude is negative */

#define Testlat DEG_MIN2RAD(35, 0)
#define Testlon DEG_MIN2RAD(-75, 0)

#define TestX 1885.4727
#define TestY 1535.9250

main()
{
	int i;
	double x, y;

/* Setup is also from USGS book test set */
	albers_setup(Slat, Nlat, Originlon, Originlat);

	albers_project(Testlon, Testlat, &x, &y);
	printf("%.41f, %.41f =?= %.41f, %.41f/n",
		x * CONST_EradiusKM, y * CONST_EradiusKM,
		TestX, TestY);
}
#endif		/* TESTPROGRAM */
