/*****************************************************************************
*	rad.h
*
*	This is the headerfile which defines the data structures used in rad.c
*
* 	Copyright (C) 1990-1991 Apple Computer, Inc.
* 	All rights reserved.
*
*	12/1990 S. Eric Chen	
******************************************************************************/

#ifndef __RAD__
#define __RAD__

#define kNumberOfRadSamples	3

typedef struct { float x, y, z; } TPoint3f;
typedef TPoint3f TVector3f;
typedef struct { unsigned char a, r, g, b;} TColor32b;

typedef struct {
	double samples[kNumberOfRadSamples];
} TSpectra;

typedef struct {
	TSpectra* reflectance; /* diffuse reflectance of the patch */
	TSpectra* emission;	/* emission of the patch */
	TPoint3f center;	/* center of the patch where hemi-cubes will be placed */
	TVector3f normal;	/* normal of the patch; for orienting the hemi-cube */
	TSpectra unshotRad;	/* unshot radiosity of the patch */
	double area;	/* area of the patch */
} TPatch;

typedef struct {
	unsigned short nVerts;	/* number of vertices of the element */
	unsigned long* verts;	/* vertices */
	TVector3f normal;	/* normal of the element; for backface removal */
	TSpectra rad;	/* total radiosity of the element */
	double area;	/* area of the patch */
	TPatch* patch;	/* pointer to the parent patch */
} TElement;

typedef struct {
	TPoint3f camera;	/* camera location */
	TPoint3f lookat;	/* point of interest */
	TVector3f up;	/* view up vector */
	float fovx, fovy;	/* field of view in x, y (in degree) */
	float near, far;	/* distance from the camera to the near and far planes */
	unsigned short xRes, yRes;	/* resolution of the frame buffer */
	unsigned long* buffer;	/* pointer to the frame buffer */
	long wid;	/* id or pointer to the window associated with the view */
} TView;	

/* Radiosity input parameters */
typedef struct {
	double threshold; /* convergence threshold (fraction of the total emitted 						energy) */
	unsigned long nPatches; /* number of patches */
	TPatch *patches;	/* patches */
	unsigned long nElements; /* number of elements */
	TElement *elements;	/* elements */
	unsigned long nPoints;	/* number of element vertices */
	TPoint3f *points;	/* element vertices */
	TView displayView;	/* view to display the results */
	unsigned short hemicubeRes; /* hemi-cube resolution */
	float worldSize;	/* approximate diameter of the bounding sphere of the world.  			used for placing near and far planes in the hemi-cube computation*/
	float intensityScale;	/* used to scale intensity for display */
	int	addAmbient;		/* whether or not to add the ambient approximation in display */
} TRadParams;

/* make it C++ friendly */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* initialization */
void InitRad(TRadParams *p);
/* main iterative loop */
void DoRad();
/* final clean up */
void CleanUpRad();

/* The following routines should be provided by the user */

/* Clear buffer. Set up view transformation.*/
/* Open a window if necessary (check if view->wid is zero) */
void BeginDraw(
TView *view, 	/* the viewing parameters and frame buffer to draw to*/
unsigned long color /* color used to clear the buffer */
);

/* Draw a 3-d polygon with a constant color */
void DrawPolygon(
int nPts, /* number of points in the polygon */
TPoint3f *pts, /* points of the polygon */
TVector3f* normal, /* normal of the polygon */
unsigned long color /* color to be drawn with */
);

/* Finish the drawing of polygons to the frame buffer*/
/* Display the buffer content in the window if necessary */
void EndDraw();

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* __RAD__ */

