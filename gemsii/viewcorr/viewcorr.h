/* viewcorr.h
 *    The global types for view correlation routines.
 */

typedef struct ViewParmsStruct {
    Point3 eye;			/* projection point */
    Matrix3 view;		/* 3x3 rotation matrix */
    Matrix3 viewinv;		/* 3x3 inverse rotation matrix */
    double d_over_s;		/* distance to screen / half screen width */
    double aspect;		/* aspect ratio (for non-square pixels) */
    double halfx, halfy;	/* half of screen resolutions */
    double xcenter, ycenter;	/* center of image */
} ViewParms;

typedef struct ViewDataStruct {
    int numpts;			/* number of data points */
    Point3 *pts;   		/* array of three D data points */
    Point2 *scrpts;   		/* array of screen data points */
} ViewData;

/* If you cannot handle arbitrary aspect ratios, change the following define
 * to an undef.  The iteration will happen with the aspect ratio given in
 * the initial set of ViewParms.
 */
#define ITERATE_ASPECT_RATIO

#ifdef ITERATE_ASPECT_RATIO
#define NUM_VIEW_PARMS 10
#else
#define NUM_VIEW_PARMS 9
#endif
