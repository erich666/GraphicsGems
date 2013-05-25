/* ptinpoly.h - point in polygon inside/outside algorithms header file.
 *
 * by Eric Haines, 3D/Eye Inc, erich@eye.com
 */

/* Define CONVEX to compile for testing only convex polygons (when possible,
 * this is faster) */
/* #define CONVEX */

/* Define HYBRID to compile triangle fan test for CONVEX with exterior edges
 * meaning an early exit (faster - recommended).
 */
/* #define HYBRID */

/* Define DISPLAY to display test triangle and test points on screen */
/* #define DISPLAY */

/* Define RANDOM to randomize order of edges for exterior test (faster -
 * recommended). */
/* #define RANDOM */

/* Define SORT to sort triangle edges and areas for half-plane and Spackman
 * tests (faster - recommended).
 * The bad news with SORT for non-convex testing is that this usually messes
 * up any coherence for the triangle fan tests, meaning that points on an
 * interior edge can be mis-classified (very rare, except when -c is used).
 * In other words, if a point lands on an edge between two test triangles,
 * normally it will be inside only one - sorting messes up the test order and
 * makes it so that the point can be inside two.
 */
/* #define SORT */

/* Define WINDING if a non-zero winding number should be used as the criterion
 * for being inside the polygon.  Only used by the general crossings test and
 * Weiler test.	 The winding number computed for each is the number of
 * counter-clockwise loops the polygon makes around the point.
 */
/* #define WINDING */

/* =========================== System Related ============================= */

/* define your own random number generator, change as needed */
/* SRAN initializes random number generator, if needed */
#define SRAN()		srand48(1)
/* RAN01 returns a double from [0..1) */
#define RAN01()		drand48()
double	drand48() ;

/* On systems without drand48() you might do this instead (though check if
 * rand()'s divisor is correct for your machine):
#define SRAN()		srand(1)
#define RAN01()		((double)rand() / 32768.0)
*/

/* =========== Grid stuff ================================================= */

#define GR_FULL_VERT	0x01	/* line crosses vertically */
#define GR_FULL_HORZ	0x02	/* line crosses horizontally */

typedef struct {
    double	xa,ya ;
    double	minx, maxx, miny, maxy ;
    double	ax, ay ;
    double	slope, inv_slope ;
} GridRec, *pGridRec;

#define GC_BL_IN	0x0001	/* bottom left corner is in (else out) */
#define GC_BR_IN	0x0002	/* bottom right corner is in (else out) */
#define GC_TL_IN	0x0004	/* top left corner is in (else out) */
#define GC_TR_IN	0x0008	/* top right corner is in (else out) */
#define GC_L_EDGE_HIT	0x0010	/* left edge is crossed */
#define GC_R_EDGE_HIT	0x0020	/* right edge is crossed */
#define GC_B_EDGE_HIT	0x0040	/* bottom edge is crossed */
#define GC_T_EDGE_HIT	0x0080	/* top edge is crossed */
#define GC_B_EDGE_PARITY	0x0100	/* bottom edge parity */
#define GC_T_EDGE_PARITY	0x0200	/* top edge parity */
#define GC_AIM_L	(0<<10) /* aim towards left edge */
#define GC_AIM_B	(1<<10) /* aim towards bottom edge */
#define GC_AIM_R	(2<<10) /* aim towards right edge */
#define GC_AIM_T	(3<<10) /* aim towards top edge */
#define GC_AIM_C	(4<<10) /* aim towards a corner */
#define GC_AIM		0x1c00

#define GC_L_EDGE_CLEAR GC_L_EDGE_HIT
#define GC_R_EDGE_CLEAR GC_R_EDGE_HIT
#define GC_B_EDGE_CLEAR GC_B_EDGE_HIT
#define GC_T_EDGE_CLEAR GC_T_EDGE_HIT

#define GC_ALL_EDGE_CLEAR	(GC_L_EDGE_HIT | \
				 GC_R_EDGE_HIT | \
				 GC_B_EDGE_HIT | \
				 GC_T_EDGE_HIT )

typedef struct {
    short		tot_edges ;
    unsigned short	gc_flags ;
    GridRec		*gr ;
} GridCell, *pGridCell;

typedef struct {
    int		xres, yres ;	/* grid size */
    int		tot_cells ;	/* xres * yres */
    double	minx, maxx, miny, maxy ;	/* bounding box */
    double	xdelta, ydelta ;
    double	inv_xdelta, inv_ydelta ;
    double	*glx, *gly ;
    GridCell	*gc ;
} GridSet, *pGridSet ;


#ifdef	CONVEX
/* =========== Inclusion stuff ============================================ */
typedef struct {
    double	dot ;		/* angle to beginning of edge */
    double	ex, ey, ec ;	/* edge equation */
} InclusionSet, *pInclusionSet ;

typedef struct {
    int		    flip_edge ; /* clockwise/counterclockwise */
    int		    hi_start ;	/* hi start for binary search: numverts-1 */
    double	    ax, ay ;	/* anchor edge vector */
    double	    qx, qy ;	/* anchor point */
    pInclusionSet    pis ;
} InclusionAnchor, *pInclusionAnchor ;
#endif	/* end CONVEX */

/* =========== Half-Plane stuff =========================================== */

typedef struct {
    double	vx, vy, c ;	/* edge equation  vx*X + vy*Y + c = 0 */
#ifdef CONVEX
#ifdef HYBRID
    int		ext_flag ;	/* TRUE == exterior edge of polygon */
#endif
#endif
} PlaneSet, *pPlaneSet ;


#ifdef	CONVEX
#ifdef	SORT
/* Size sorting structure for half-planes */
typedef struct {
    double	size ;
    pPlaneSet	pps ;
} SizePlanePair, *pSizePlanePair ;
#endif
#endif


/* =========== Spackman (precomputed barycentric) stuff =================== */
typedef struct {
    double	u1p, u2, v1p, v2, inv_u1, inv_u2, inv_v1 ;
    int		u1_nonzero ;
} SpackmanSet, *pSpackmanSet ;



/* =========== Trapezoid stuff ============================================ */
/* how many bins shall we put the edges into? */
#define TOT_BINS	1000	/* absolutely the maximum number of bins */

/* add a little to the limits of the polygon bounding box to avoid precision
 * problems.
 */
#define EPSILON		0.00001

/* The following structure is associated with a polygon */
typedef struct {
    int		id ;		/* vertex number of edge */
    int		full_cross ;	/* 1 if extends from top to bottom */
    double	minx, maxx ;	/* X bounds for bin */
} Edge, *pEdge ;

typedef struct {
    pEdge	*edge_set ;
    double	minx, maxx ;	/* min and max for all edges in bin */
    int		count ;
} Trapezoid, *pTrapezoid ;

typedef struct {
    int		bins ;
    double	minx, maxx ;	/* bounding box for polygon */
    double	miny, maxy ;
    double	ydelta ;	/* (maxy - miny)/bins */
    double	inv_ydelta ;
    Trapezoid	*trapz ;
} TrapezoidSet, *pTrapezoidSet ;


#ifdef	CONVEX
pPlaneSet	ExteriorSetup() ;
void	ExteriorCleanup() ;

pInclusionAnchor	InclusionSetup() ;
void	InclusionCleanup() ;

#ifdef	SORT
int	CompareSizePlanePairs() ;
#endif
#endif

pPlaneSet	PlaneSetup() ;
void	PlaneCleanup() ;

pSpackmanSet	SpackmanSetup() ;
void	SpackmanCleanup() ;

void	TrapezoidCleanup() ;
void	TrapBound() ;
int	CompareEdges() ;
void	TrapezoidSetup() ;

void	GridSetup() ;
int	AddGridRecAlloc() ;
void	GridCleanup() ;
