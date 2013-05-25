/*****************************************************************************
*  rad.c
*
*  This program contains three functions that should be called in sequence to
*  perform radiosity rendering:
*  InitRad(): Initialize radiosity.
*  DoRad(): Perform the main radiosity iteration loop.
*  CleanUpRad(): Clean up.
*
*  The following routines are assumed to be provided by the user:
*  BeginDraw()
*  DrawPolygon()
*  EndDraw()
*  Refer to rad.h for details
*
*  Copyright (C) 1990-1991 Apple Computer, Inc.
*  All rights reserved.
*
*  12/1990 S. Eric Chen	
******************************************************************************/

#include "rad.h"
#include <math.h>
#include <stdlib.h>

#define kMaxPolyPoints	255
#define PI	3.1415926
#define AddVector(c,a,b) (c).x=(a).x+(b).x, (c).y=(a).y+(b).y, (c).z=(a).z+(b).z
#define SubVector(c,a,b) (c).x=(a).x-(b).x, (c).y=(a).y-(b).y, (c).z=(a).z-(b).z
#define CrossVector(c,a,b)	(c).x = (a).y*(b).z - (a).z*(b).y, \
				(c).y = (a).z*(b).x - (a).x*(b).z, \
				(c).z = (a).x*(b).y - (a).y*(b).x
#define DotVector(a,b) (a).x*(b).x + (a).y*(b).y + (a).z*(b).z
#define ScaleVector(c,s) (c).x*=(s), (c).y*=(s), (c).z*=(s)
#define NormalizeVector(n,a) 	((n)=sqrt(DotVector(a,a)), \
				(n)?((a).x/=n, (a).y/=n, (a).z/=n):0)

typedef struct {
	TView	view;	/* we only need to store one face of the hemi-cube */
	double*	topFactors;	/* delta form-factors(weight for each pixel) of the top face */
	double*	sideFactors; /* delta form-factors of the side faces */
} THemicube;

static TRadParams *params;	/* input parameters */
static THemicube hemicube;	/* one hemi-cube */
static double *formfactors;	/* a form-factor array which has the same length as the number of elements */
static double totalEnergy;	/* total emitted energy; used for convergence checking */

static const TSpectra black = { 0, 0, 0 };	/* for initialization */
static int FindShootPatch(unsigned long *shootPatch);
static void SumFactors(double* formfs, int xRes, int yRes, 
	unsigned long* buf, double* deltaFactors);
static void MakeTopFactors(int hres, double* deltaFactors);
static void MakeSideFactors(int hres, double* deltaFactors);
static void ComputeFormfactors(unsigned long shootPatch);
static void DistributeRad(unsigned long shootPatch);
static void DisplayResults(TView* view);
static void DrawElement(TElement* ep, unsigned long color);
static TColor32b SpectraToRGB(TSpectra* spectra);


/* Initialize radiosity based on the input parameters p */
void InitRad(TRadParams *p)
{
	int n;
	int hRes;
	unsigned long i;
	int j;
	TPatch*	pp;
	TElement* ep;
	
	params = p;
	
	/* initialize hemi-cube */
	hemicube.view.fovx = 90;
	hemicube.view.fovy = 90;
	/* make sure hemicube resolution is an even number */
	hRes = ((int)(params->hemicubeRes/2.0+0.5))*2;
	hemicube.view.xRes = hemicube.view.yRes = hRes;
	n = hRes*hRes;
	hemicube.view.buffer = calloc(n, sizeof(unsigned long));
	hemicube.view.wid=0;
	hemicube.view.near = params->worldSize*0.001;
	hemicube.view.far = params->worldSize;
	
	/* take advantage of the symmetry in the delta form-factors */
	hemicube.topFactors= calloc(n/4, sizeof(double));
	hemicube.sideFactors= calloc(n/4, sizeof(double));
	MakeTopFactors(hRes/2, hemicube.topFactors);
	MakeSideFactors(hRes/2, hemicube.sideFactors);
	
	formfactors = calloc(params->nElements, sizeof(double));
	
	/* initialize radiosity */
	pp = params->patches;
	for (i=params->nPatches; i--; pp++)
		pp->unshotRad = *(pp->emission);
	ep = params->elements;
	for (i=params->nElements; i--; ep++)
		ep->rad = *(ep->patch->emission);

	/* compute total energy */
	totalEnergy = 0;
	pp = params->patches;
	for (i=params->nPatches; i--; pp++)
		for (j=0; j<kNumberOfRadSamples; j++)
			totalEnergy += pp->emission->samples[j] * pp->area;

	DisplayResults(&params->displayView); 

}

/* Main iterative loop */
void DoRad()
{
	unsigned long shootPatch;
	
	while (FindShootPatch(&shootPatch)) 
	{
		ComputeFormfactors(shootPatch);
		DistributeRad(shootPatch);
		DisplayResults(&params->displayView);
	}
	
}

/* Clean up */
void CleanUpRad()
{
	free(hemicube.topFactors);
	free(hemicube.sideFactors);
	free(hemicube.view.buffer);
	free(formfactors);

}

/* Find the next shooting patch based on the unshot energy of each patch */
/* Return 0 if convergence is reached; otherwise, return 1 */
static int FindShootPatch(unsigned long *shootPatch)
{
	int i, j;
	double energySum, error, maxEnergySum=0;
	TPatch* ep;

	ep = params->patches;
	for (i=0; i< params->nPatches; i++, ep++)
	{
		energySum =0;
		for (j=0; j<kNumberOfRadSamples; j++)
			energySum += ep->unshotRad.samples[j] * ep->area;
		
		if (energySum > maxEnergySum) 
		{
			*shootPatch = i;
			maxEnergySum = energySum;
		}
	}

	error = maxEnergySum / totalEnergy;
	/* check convergence */
	if (error < params->threshold)
		return (0);		/* converged */
	else
		return (1);
	

}

/* Find out the index to the delta form-factors array */
#define Index(i)	((i)<hres? i: (hres-1- ((i)%hres)))

/* Use the largest 32bit unsigned long for background */
#define kBackgroundItem 0xffffffff

/* Convert a hemi-cube face to form-factors */
static void SumFactors(
double* formfs, /* output */
int xRes, int yRes, /* resolution of the hemi-cube face */
unsigned long* buf, /* we only need the storage of the top hemi-cube face */
double* deltaFactors /* delta form-factors for each hemi-cube pixel */
)
{
	int i, j;
	int ii, jj;
	unsigned long *ip=buf;
	int hres = xRes/2;
	for (i=0; i<yRes; i++) 
	{
		ii= Index(i)*hres;
	  	for (j=0; j<xRes; j++, ip++) 
			if ((*ip) != kBackgroundItem) 
			{
				jj = Index(j);
				formfs[*ip] += deltaFactors[ii+jj];
			}
	}
}

/* Create the delta form-factors for the top face of hemi-cube */
/* Only need to compute 1/4 of the form-factors because of the 4-way symmetry */
static void MakeTopFactors(
int hres, /* half resolution of the face */
double* deltaFactors /* output */
)
{
    int j,k;
    double xSq , ySq, xy1Sq;
	double n= hres;
	double* wp;
	double dj, dk;
	
	wp = deltaFactors;
	for (j=0; j<hres; j++)
	{
		dj = (double)j;
		ySq = (n - (dj+0.5)) / n;
       	ySq *= ySq;
       	for ( k=0 ; k<hres ; k++ )
       	{
			dk = (double)k;
       		xSq = ( n - (dk + 0.5) ) / n;
			xSq *= xSq;
			xy1Sq =  xSq + ySq + 1.0 ;
			xy1Sq *= xy1Sq;
        	*wp++ = 1.0 / (xy1Sq * PI * n * n);
       	}
    }
}

/* Create the delta form-factors for the side face of hemi-cube */
/* Only need to compute 1/4 of the form-factors because of the 4-way symmetry */
static void MakeSideFactors(
int hres, /* half resolution of the face */
double* deltaFactors /* output */
)
{
    int j,k;
    double x, xSq , y, ySq, xy1, xy1Sq;
	double n= hres;
	double* wp;
	double dj, dk;
	
	wp = deltaFactors;
	for (j=0; j<hres; j++)
	{
		dj = (double)j;
		y = (n - (dj+0.5)) / n;
       	ySq = y*y;
       	for ( k=0 ; k<hres ; k++ )
       	{
			dk = (double)k;
       		x = ( n - (dk + 0.5) ) / n;
			xSq = x*x;
			xy1 =  xSq + ySq + 1.0 ;
			xy1Sq = xy1*xy1;
        	*wp++ = y / (xy1Sq * PI * n * n);
       	}
    }
}

/* Use drand48 instead if it is supported */
#define RandomFloat ((float)(rand())/(float)RAND_MAX)

/* Compute form-factors from the shooting patch to every elements */
static void ComputeFormfactors(unsigned long shootPatch)
{
	unsigned long i;
	TVector3f	up[5]; 
	TPoint3f	lookat[5];
	TPoint3f	center;
	TVector3f	normal, tangentU, tangentV, vec;
	int face;
	double		norm;
	TPatch*		sp;
	double*		fp;
	TElement*	ep;

	/* get the center of shootPatch */
	sp = &(params->patches[shootPatch]);
	center = sp->center;
	normal = sp->normal;
	
	/* rotate the hemi-cube along the normal axis of the patch randomly */
	/* this will reduce the hemi-cube aliasing artifacts */
	do {
		vec.x = RandomFloat;
		vec.y = RandomFloat;
		vec.z = RandomFloat;
		/* get a tangent vector */
		CrossVector(tangentU, normal, vec);
		NormalizeVector(norm, tangentU);
	} while (norm==0);	/* bad choice of the random vector */
	
	/* compute tangentV */
	CrossVector(tangentV, normal, tangentU);
	
	/* assign the lookats and ups for each hemicube face */
	AddVector(lookat[0], center, normal);
	up[0] = tangentU;
	AddVector(lookat[1], center, tangentU);
	up[1] = normal;
	AddVector(lookat[2], center, tangentV);
	up[2] = normal;
	SubVector(lookat[3], center, tangentU);
	up[3] = normal;
	SubVector(lookat[4], center, tangentV);
	up[4] = normal;
	
	/* position the hemicube slightly above the center of the shooting patch */
	ScaleVector(normal, params->worldSize*0.0001);
	AddVector(hemicube.view.camera, center, normal);
	
	/* clear the formfactors */
	fp = formfactors;
	for (i=params->nElements; i--; fp++)
		*fp = 0.0;
		
	for (face=0; face < 5; face++)
	{
		hemicube.view.lookat = lookat[face];
		hemicube.view.up = up[face];

		/* draw elements */
		BeginDraw(&(hemicube.view), kBackgroundItem);
		for (i=0; i< params->nElements; i++)
			DrawElement(&params->elements[i], i);	
			/* color element i with its index */
		EndDraw();
		
		/* get formfactors */
		if (face==0)
			SumFactors(formfactors, hemicube.view.xRes, hemicube.view.yRes, 
				hemicube.view.buffer, hemicube.topFactors);
		else
			SumFactors(formfactors, hemicube.view.xRes, hemicube.view.yRes/2, 
				hemicube.view.buffer, hemicube.sideFactors);
	}
	
	/* compute reciprocal form-factors */
	ep = params->elements;
	fp = formfactors;
	for (i=params->nElements; i--; ep++, fp++)
	{
		*fp *= sp->area / ep->area;

		/* This is a potential source of hemi-cube aliasing */
		/* To do this right, we need to subdivide the shooting patch
		and reshoot. For now we just clip it to unity */
		if ((*fp) > 1.0) 	*fp = 1.0;	
	}

}

/* Distribute radiosity form shootPatch to every element */
/* Reset the shooter's unshot radiosity to 0 */
static void DistributeRad(unsigned long shootPatch)
{
	unsigned long i;
	int j;
	TPatch* sp;
	TElement* ep;
	double* fp;
	TSpectra deltaRad;
	double w;

	sp = &(params->patches[shootPatch]);
	
	/* distribute unshotRad to every element */
	ep = params->elements;
	fp = formfactors;
	for (i=params->nElements; i--; ep++, fp++)
	{
		if ((*fp) != 0.0) 
		{
			for (j=0; j<kNumberOfRadSamples; j++)
				 deltaRad.samples[j] = 	sp->unshotRad.samples[j] * (*fp) * 
										ep->patch->reflectance->samples[j];

			/* incremental element's radiosity and patch's unshot radiosity */
			w = ep->area/ep->patch->area;
			for (j=0; j<kNumberOfRadSamples; j++) 
			{
				ep->rad.samples[j] += deltaRad.samples[j];
				ep->patch->unshotRad.samples[j] += deltaRad.samples[j] * w;
			}
		}
	}
	
	/* reset shooting patch's unshot radiosity */
	sp->unshotRad = black;
}

/* Convert a TSpectra (radiosity) to a TColor32b (rgb color) */
/* Assume the first three samples of the spectra are the r, g, b colors */
/* More elaborated color space transformation could be performed here */
static TColor32b
SpectraToRGB(TSpectra* spectra)
{
	TColor32b	c;
	TSpectra	r;
	double 	max=1.0;
	int k;

	for (k=kNumberOfRadSamples; k--;) {
		if (spectra->samples[k] > max)
			max = spectra->samples[k];
	}
	/* Clip the intensity*/
	r = *spectra;
	if (max>1.0) {
		for (k=kNumberOfRadSamples; k--; )
			r.samples[k] /= max;
	}
	
	/* Convert to a 32-bit color; Assume the first 3 samples in TSpectra 
	are the r, g, b colors we want. Otherwise, do color conversion here */
	c.a= 0;
	c.r= (unsigned char) (r.samples[0] * 255.0 + 0.5);
	c.g= (unsigned char) (r.samples[1] * 255.0 + 0.5);
	c.b= (unsigned char) (r.samples[2] * 255.0 + 0.5);
	
	return c;
}

static void
GetAmbient(TSpectra* ambient)
{
	TPatch* p;
	unsigned long i;
	int k;
	static int first = 1;
	static TSpectra baseSum; 
	TSpectra uSum;

	uSum=black;
	if (first) {
		double areaSum;
		TSpectra rSum;
		areaSum=0;
		rSum=black;
		/* sum area and (area*reflectivity) */
		p= params->patches;
		for (i=params->nPatches; i--; p++) {
			areaSum += p->area;
			for (k=kNumberOfRadSamples; k--; )
				rSum.samples[k] += p->reflectance->samples[k]* p->area;
		}
		for (k=kNumberOfRadSamples; k--; )
			baseSum.samples[k] = areaSum - rSum.samples[k];
		first = 0;
	}

	/* sum (unshot radiosity * area) */
	p= params->patches;
	for (i=params->nPatches; i--; p++) {
		for (k=kNumberOfRadSamples; k--; )
			uSum.samples[k] += p->unshotRad.samples[k] * p->area;
	}
	
	/* compute ambient */
	for (k=kNumberOfRadSamples; k--; )
		ambient->samples[k] = uSum.samples[k] / baseSum.samples[k];

}

static void
DisplayResults(TView* view)
{
	unsigned long i;
	register TElement* ep;
	TSpectra ambient;
	GetAmbient(&ambient);
	
	BeginDraw(view, 0);
	ep = params->elements;
	for (i=0; i< params->nElements; i++, ep++) {
		TColor32b	c;
		TSpectra  s;
		int k;
		/* add ambient approximation */
		if (params->addAmbient) {
			for (k=kNumberOfRadSamples; k--;)
				s.samples[k] = (ep->rad.samples[k] + (ambient.samples[k]*
					ep->patch->reflectance->samples[k]))*params->intensityScale;
		} else {
			for (k=kNumberOfRadSamples; k--; )
				s.samples[k] = ep->rad.samples[k]*params->intensityScale;
		}
		/* quantize color */
		c = SpectraToRGB(&s);
		DrawElement(ep, *(unsigned long*)&c);
	}
			
	EndDraw();

}

static void
DrawElement(TElement* ep, unsigned long color)
{
	static TPoint3f pts[kMaxPolyPoints];
	int nPts = ep->nVerts;
	int j;
	for (j=0; j<nPts; j++)
		pts[j] = params->points[ep->verts[j]];
	
	DrawPolygon(nPts, pts, &ep->normal, color);

}




