/*  FILENAME:  AALines.h  [revised 17 AUG 90]

    AUTHOR:  Kelvin Thompson

    DESCRIPTION:  Symbols and globals for the anti-aliased line
      renderer.

    #INCLUDED IN:
      AAMain.c -- Calling routine for renderer.
      AATables.c -- Initialization routines for lookup tables.
      AALines.c -- Rendering code.
*/

/* frame buffer to hold the anti-aliased line */
#define xpix 60
#define ypix 60
extern char *fbuff;

/* macros to access the frame buffer */
#define PIXADDR(xx,yy) (fbuff+(yy)*xpix+(xx))
#define PIXINC(dx,dy)  ((dy)*xpix+(dx))

/* fixed-point data types and macros */
typedef int FX;
typedef unsigned int UFX;
#define FX_FRACBITS 16  /* bits of fraction in FX format */
#define FX_0        0   /* zero in fixed-point format */
#define FLOAT_TO_FX(flt)  ((FX)((flt)*(1<<FX_FRACBITS)+0.5))

/* some important constants */
#define PI      3.1415926535897932384626433832795028841971693993751
#define SQRT_2  1.4142135623730950488016887242096980785696718753769

/* square-root function globals */
extern UFX *sqrtfunc;
extern int sqrtcells;
extern int sqrtshift;
#define SQRTFUNC(fxval)  (sqrtfunc[ (fxval) >> sqrtshift ])

/* AA globals */
extern float line_r;  /* line radius */
extern float pix_r;   /* pixel radius */
extern FX *coverage;
extern int covercells;
extern int covershift;
#define COVERAGE(fxval) (coverage[ (fxval) >> covershift ])
