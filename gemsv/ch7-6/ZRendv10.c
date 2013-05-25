/*  
       Z Buffer Rendering Program Version 10, 02/12/96
       Copyright Raghu Karinthi, West Virginia University	
*/	       
#include "ZRendv10.h"

ZBuffer zb;
OrigTriangle localSet[NUM_TRIANGLES];
FrameBuffer fb;


int16 BFCULL;
int16 TRIVIAL_REJECT;
int16 CLIP;
int16 PHONG;
MAT3fvec Hvector;
SpecLightPoint SpecSource;
float out_xmin, out_xmax, out_ymin, out_ymax, out_zmin, out_zmax;


/* ---------------------------------------------------------------- */

 
/*  Generic Utilities */
 
#define TSWAP(_a_, _b_, _c_) ((_c_) = (_a_), (_a_) = (_b_), (_b_) = (_c_))
#define min(_a_, _b_) (((_a_) < (_b_)) ? (_a_) : (_b_))
#define max(_a_, _b_) (((_a_) > (_b_)) ? (_a_) : (_b_))
#define ZFABS(x) (((x) < 0.0) ? -(x) : (x))


/* --------------------------------------------------------------- */


/*      Matrix and Vector utilities */

#define MAT3_SCALE_VEC(RESULT_V,V,SCALE) \
        ((RESULT_V)[0]=(V)[0]*(SCALE), (RESULT_V)[1]=(V)[1]*(SCALE), \
	 (RESULT_V)[2]=(V)[2]*(SCALE))

#define MAT3_DOT_PRODUCT(V1,V2) \
			((V1)[0]*(V2)[0] + (V1)[1]*(V2)[1] + (V1)[2]*(V2)[2])

#define MAT3_SUB_VEC(RESULT_V,V1,V2) \
        ((RESULT_V)[0] = (V1)[0]-(V2)[0], (RESULT_V)[1] = (V1)[1]-(V2)[1], \
         (RESULT_V)[2] = (V1)[2]-(V2)[2])

#define MAT3_NORMALIZE_VEC(V,TEMP) \
	if ((TEMP = sqrt(MAT3_DOT_PRODUCT(V,V))) > MAT3_EPSILON) { \
	   TEMP = 1.0 / TEMP; \
	   MAT3_SCALE_VEC(V,V,TEMP); \
	} else TEMP = 0.0

#define MAT3_CROSS_PRODUCT(RES,V1,V2) \
((RES)[0] = (V1)[1] * (V2)[2] - (V1)[2] * (V2)[1], \
 (RES)[1] = (V1)[2] * (V2)[0] - (V1)[0] * (V2)[2], \
 (RES)[2] = (V1)[0] * (V2)[1] - (V1)[1] * (V2)[0]) 

void MAT3identity(MAT3mat m)
{
int16 i, j;

for (i=0; i < 4; i++)
  for (j=0; j < 4; j++)
    if (i==j)
      m[i][j] = 1.0;
    else
      m[i][j] = 0.0;
}

void MAT3mult(MAT3mat result_mat, MAT3mat mat1,	MAT3mat mat2)
/* result_mat = mat1 * mat2 */
{
int16 i, j, k;
double tmp;
MAT3mat tmp_mat;

for (i=0; i < 4; i++)
  for (j=0; j < 4; j++) {
    tmp = 0.0;
    for (k=0; k < 4; k++)
      tmp += mat1[i][k]*mat2[k][j];
    tmp_mat[i][j] = tmp;
  }
for (i=0; i < 4; i++)
  for (j=0; j < 4; j++)
    result_mat[i][j] = tmp_mat[i][j];
}

#define Det2D(a,b,c,d)((a)*(d)-(b)*(c))


/* ------------------------------------------------------------------ */


/* Fixed Point Utilities */
#define fp_floor(x)  \
  (((x) < 0) ? \
   (!((x) & LOMASK)) ? ((x) >> LOBITS) : (((x) >> LOBITS)+1) : \
   ((x)>> LOBITS) \
  )

/*
 *  Works only if x +ve.
 */

#define fp_floor_pos(x) ((x) >> LOBITS)

/*
 *  We use the following approximation.
 */

#define fp_div(x,y) \
	  (((x) < 0) ? (((x) / (y) - 1) << LOBITS) : (((x) / (y)) << LOBITS))

/*
 *  Works only if xlo = 0.
 */

#define fp_mult1(x,y) \
  (((x)<0) ? \
   -(((-(x) & HIMASK)>> LOBITS)*(y)) : \
   ((x)>> LOBITS)*(y))

/*
 * Works only if xhi = 0
 */

#define fp_mult2(x,y) \
 (((y)<0)?(-(((((-(y))&HIMASK)>>LOBITS)*(x))+((((-(y))&LOMASK)*(x))>>LOBITS)))\
    : ((((y) >> LOBITS) * (x)) + ((((y)&LOMASK)*(x))>>LOBITS)))

/*
 *  Fractional Part if x +ve
 */

#define fp_fraction(x) ((x) & LOMASK)

/*
  float to fix conversion
 */

#define sh_float_to_fix(x) ( \
  (((x) >> FLOATSZM1) ? \
  (-( \
  ((int32) ((((x) & EXPMASK) >> FFRACSZ) - FEXPOFFSET) > FFRACSZMLOBITS)? \
     ((((x) & FFRACMASK) | (0x00800000)) <<  \
       ((((x) & EXPMASK) >> FFRACSZ) - FEXPOFFSET - FFRACSZ + LOBITS)) \
 : \
     ((((x) & FFRACMASK) | (0x00800000)) >>  \
       (FFRACSZ - LOBITS -(((x) & EXPMASK) >> FFRACSZ) + FEXPOFFSET )) \
)) \
 :\
  (((int32) (( ((x) & EXPMASK) >> FFRACSZ) - FEXPOFFSET) > FFRACSZMLOBITS)? \
     ((((x) & FFRACMASK) | (0x00800000)) <<  \
       ((((x) & EXPMASK) >> FFRACSZ) - FEXPOFFSET - FFRACSZ + LOBITS)) \
 : \
     ((((x) & FFRACMASK) | (0x00800000)) >>  \
       (FFRACSZ - LOBITS -(((x) & EXPMASK) >> FFRACSZ) + FEXPOFFSET )) \
)) \
)

/* 
    float to fix conversion for Z
*/

#define shzfloat_to_fix(x) ( \
  (((x) >> FLOATSZM1) ? \
  (-( \
  ((int32) ((((x) & EXPMASK) >> FFRACSZ) - FEXPOFFSET) > (FFRACSZ-ZLOBITS))? \
     ((((x) & FFRACMASK) | (0x00800000)) <<  \
       ((((x) & EXPMASK) >> FFRACSZ) - FEXPOFFSET - FFRACSZ + ZLOBITS)) \
 : \
     ((((x) & FFRACMASK) | (0x00800000)) >>  \
       (FFRACSZ - ZLOBITS -(((x) & EXPMASK) >> FFRACSZ) + FEXPOFFSET )) \
)) \
 :\
  (((int32) (( ((x) & EXPMASK) >> FFRACSZ) - FEXPOFFSET) > (FFRACSZ-ZLOBITS))? \
     ((((x) & FFRACMASK) | (0x00800000)) <<  \
       ((((x) & EXPMASK) >> FFRACSZ) - FEXPOFFSET - FFRACSZ + ZLOBITS)) \
 : \
     ((((x) & FFRACMASK) | (0x00800000)) >>  \
       (FFRACSZ - ZLOBITS -(((x) & EXPMASK) >> FFRACSZ) + FEXPOFFSET )) \
)) \
)


/* ------------------------------------------------------------------ */


/* Output / Display */

/*  
 *  This function writes the framebuffer to a TARGA file with
 *  24 bit color.
 */

void write_tga_buffer (FrameBuffer fb, char *filename)
{
bitmap_hdr    output;
unsigned char *rptr, *gptr, *bptr;
int16           i, j;

allocatebitmap (&output, WW, WH, COLOR_DEPTH, 1<<COLOR_DEPTH);
rptr = output.r;
gptr = output.g;
bptr = output.b;

/*
 *  Top to bottom row, left to right within a row.
 */

for (j = (WH - 1); j >= 0; j --) {
  for (i = 0; i < WW; i ++) {
    *rptr++ = ((fb[j][i]) & RMASK);
    *gptr++ = ((fb[j][i]) & GMASK) >> 8;
    *bptr++ = ((fb[j][i]) & BMASK) >> 16;
  }
}

write_tga_file (filename,&output);
} /* write_tga_buffer */


/* ------------------------------------------------------------------ */


/*  Rasterization */
	 	
/*
 *  The following : edge data structure, and functions EdgeScan,
 *  EdgeSetup, and rasterize_sorted_triangle, are modified from the code
 *  obtained in Graphics Gems III subdirectory accurate_scan due to Kurt
 *  Fleischer.  Also Refer Lathrop, Kirk, Voorhies, IEEE CG&A 10(5),
 *  1990 for a discussion
 */

#define EdgeScan(e) \
  if ((e).E<0) { \
    (e).Ix += (e).AStep; (e).E += (e).DEA; \
  } \
  else { \
   (e).Ix += (e).BStep; (e).E += (e).DEB; \
  }

#define EdgeSetup(e,xs,ys,dx,dy) \
  if ((dy) >= FIX1) { \
    si = fp_mult2((FIX1 - (fp_fraction((ys)))),(dx)); \
    xi = (xs) + fp_div(si,(dy)); \
    si = fp_div((dx), (dy)); \
    (e).AStep = fp_floor(si); \
    (e).BStep = (e).AStep+1; \
    (e).DEA= (dx) - fp_mult1(si, (dy));  \
    (e).DEB = (e).DEA - (dy); \
    (e).E = fp_mult2(fp_fraction(xi),(dy)) + (e).DEB; \
    (e).Ix = fp_floor_pos(xi); \
} /* EdgeSetup */

/*
 *  This function does the actual rasterization. 
 */

void rasterize_sorted_triangle (int16       minyv, int16       midyv, 
				int16       maxyv, ColorTriangleP   tp)
{
int16         xleft[WH], xright[WH];
int16         xmin, xmax, ymin, ymax;
fixpoint      x0, y0, x1, y1, x2, y2;
edge          left, right;
int16         temp; 
int16         ccw;
fixpoint      xi, si, dx, dy;
int16         y;

float fdx, fdy;
float dr_by_dy, dg_by_dy, db_by_dy, dr_by_dx, dg_by_dx, db_by_dx, dz_by_dy,
      dz_by_dx; 
int16         x; 
float fx0, fy0, z0, r0, g0, b0; /* fx0, fy0 distinguish them from x0 and y0 */
float rxy0,gxy0,bxy0,rx0ymin,gx0ymin,bx0ymin,zx0ymin; 
int32 col, rgboffset[WW]; 
fixpoint rx0yminf, gx0yminf, bx0yminf, zx0yminf, dr_by_dy_f, dg_by_dy_f,
         db_by_dy_f, dz_by_dy_f, dz_by_dx_f, dz_by_dx_x0f, z; 
float y1y0, y2y0, x1x0, x2x0, det, z1z0, z2z0;
float r1r0, r2r0, g1g0, g2g0, b1b0, b2b0;
float ndoth, ndothpown;
int16 specr, specg, specb;
int32 specterm;
  
x1x0 = (tp->vertices[midyv]).vertex[X] - (tp->vertices[minyv]).vertex[X];
x2x0 = (tp->vertices[maxyv]).vertex[X] - (tp->vertices[minyv]).vertex[X];
y1y0 = (tp->vertices[midyv]).vertex[Y] - (tp->vertices[minyv]).vertex[Y];
y2y0 = (tp->vertices[maxyv]).vertex[Y] - (tp->vertices[minyv]).vertex[Y];
  
det = x1x0 * y2y0 - x2x0 * y1y0;
  
/*
 *  Convert the vertices of the triangle and their color value
 *  to 'fixpoint' representation.
 */


x0 = sh_float_to_fix (*((fixpoint *)&((tp->vertices[minyv]).vertex[X])));
y0 = sh_float_to_fix (*((fixpoint *)&((tp->vertices[minyv]).vertex[Y])));

x1 = sh_float_to_fix (*((fixpoint *)&((tp->vertices[midyv]).vertex[X])));
y1 = sh_float_to_fix (*((fixpoint *)&((tp->vertices[midyv]).vertex[Y])));

x2 = sh_float_to_fix (*((fixpoint *)&((tp->vertices[maxyv]).vertex[X])));
y2 = sh_float_to_fix (*((fixpoint *)&((tp->vertices[maxyv]).vertex[Y])));

if (((tp->vertices[minyv]).vertex[X]) == 0.0) x0 = 0;
if (((tp->vertices[minyv]).vertex[Y]) == 0.0) y0 = 0;

if (((tp->vertices[midyv]).vertex[X]) == 0.0) x1 = 0;
if (((tp->vertices[midyv]).vertex[Y]) == 0.0) y1 = 0;

if (((tp->vertices[maxyv]).vertex[X]) == 0.0) x2 = 0;
if (((tp->vertices[maxyv]).vertex[Y]) == 0.0) y2 = 0;

xmin = max ((fp_floor_pos(min(min(x0,x1),x2))-1), 0);
xmax = min ((fp_floor_pos(max(max(x0,x1),x2))+1), (WW-1));

/*
 *  Find out whether the triangle edges are oriented in clockwise
 *  or anti-clockwise direction.
 *
 */

ccw = det > 0.0;

/*
 *  Setup first pair of edges.
 */

if (ccw) {
  EdgeSetup (left,  x0, y0, x2-x0, y2-y0)
  EdgeSetup (right, x0, y0, x1-x0, y1-y0)
} /* if ccw orientation of edges */
else {
  EdgeSetup (left, x0, y0, x1-x0, y1-y0)
  EdgeSetup (right, x0, y0, x2-x0, y2-y0)
} /* else clockwise orientation of edges */


ymin = fp_floor_pos(y0)+1;
temp = fp_floor_pos(y1);
for (y = ymin; y <= temp; y++) {
  xleft[y] = left.Ix;    EdgeScan (left)
  xright[y] = right.Ix;   EdgeScan (right)
} /* for every scan line upto the lower 'y' of the two edges */

/*
 *  Setup the third edge.
 */

if (ccw) {
  EdgeSetup (right, x1, y1, x2-x1, y2-y1)
}
else {
  EdgeSetup (left, x1, y1, x2-x1, y2-y1)
}

/*
 *  Now scan convert the rest of the triangle.
 */

temp = fp_floor_pos(y1) + 1;
ymax = fp_floor_pos (y2);
for (y = temp; y <= ymax; y++) {
  xleft[y] = left.Ix;    EdgeScan (left)
  xright[y] = right.Ix;   EdgeScan (right)
} /* for every scan line till the uppermost vertex of the triangle */

if (PHONG) {
  ndoth = tp->normal[X] * Hvector[X] + tp->normal[Y] * Hvector[Y] +
    tp->normal[Z] * Hvector[Z];
  ndothpown = pow(ndoth,SpecSource.spec_exp);
  specr = SpecSource.red * ndothpown;
  specg = SpecSource.green * ndothpown;
  specb = SpecSource.blue * ndothpown;
  specterm = specr + (specg << 8) + (specb << 16);
}
else
  specterm = 0;

z1z0 = (tp->vertices[midyv]).vertex[Z] - (tp->vertices[minyv]).vertex[Z];
z2z0 = (tp->vertices[maxyv]).vertex[Z] - (tp->vertices[minyv]).vertex[Z];

dz_by_dx = (y2y0 * z1z0 - y1y0 * z2z0)/det;
dz_by_dy = (x1x0 * z2z0 - x2x0 * z1z0)/det;

r1r0 = (tp->vertices[midyv]).red - (tp->vertices[minyv]).red;
r2r0 = (tp->vertices[maxyv]).red - (tp->vertices[minyv]).red;

dr_by_dx = (y2y0 * r1r0 - y1y0 * r2r0)/det;
dr_by_dy = (x1x0 * r2r0 - x2x0 * r1r0)/det;

g1g0 = (tp->vertices[midyv]).green - (tp->vertices[minyv]).green;
g2g0 = (tp->vertices[maxyv]).green - (tp->vertices[minyv]).green;

dg_by_dx = (y2y0 * g1g0 - y1y0 * g2g0)/det;
dg_by_dy = (x1x0 * g2g0 - x2x0 * g1g0)/det;

b1b0 = (tp->vertices[midyv]).blue - (tp->vertices[minyv]).blue;
b2b0 = (tp->vertices[maxyv]).blue - (tp->vertices[minyv]).blue;

db_by_dx = (y2y0 * b1b0 - y1y0 * b2b0)/det;
db_by_dy = (x1x0 * b2b0 - x2x0 * b1b0)/det;
  
fx0 = (tp->vertices[minyv]).vertex[X];
fy0 = (tp->vertices[minyv]).vertex[Y];
z0 = (tp->vertices[minyv]).vertex[Z];
r0 = (tp->vertices[minyv]).red;
g0 = (tp->vertices[minyv]).green;
b0 = (tp->vertices[minyv]).blue; 
	
fdy = ymin - fy0;
rx0ymin = r0 + dr_by_dy * fdy;
gx0ymin = g0 + dg_by_dy * fdy;
bx0ymin = b0 + db_by_dy * fdy;
zx0ymin = z0 + dz_by_dy * fdy;

fdx = xmin - fx0;
rxy0  = dr_by_dx * fdx;
gxy0  = dg_by_dx * fdx;
bxy0  = db_by_dx * fdx;
for (x=xmin; x <= xmax; x++) {
  rgboffset[x] = ((int32) rxy0) + (((int32) gxy0) << 8) + 
    (((int32) bxy0) << 16) + specterm;
  rxy0 += dr_by_dx;
  gxy0 += dg_by_dx;
  bxy0 += db_by_dx;
}

rx0yminf = sh_float_to_fix (*((fixpoint *)&rx0ymin));
gx0yminf = sh_float_to_fix (*((fixpoint *)&gx0ymin));
bx0yminf = sh_float_to_fix (*((fixpoint *)&bx0ymin));
zx0yminf = shzfloat_to_fix (*((fixpoint *)&zx0ymin));
dr_by_dy_f = sh_float_to_fix (*((fixpoint *)&dr_by_dy));
dg_by_dy_f = sh_float_to_fix (*((fixpoint *)&dg_by_dy));
db_by_dy_f = sh_float_to_fix (*((fixpoint *)&db_by_dy));
dz_by_dy_f = shzfloat_to_fix (*((fixpoint *)&dz_by_dy));
dz_by_dx_f = shzfloat_to_fix (*((fixpoint *)&dz_by_dx));
fdx = dz_by_dx*fx0;
dz_by_dx_x0f = shzfloat_to_fix (*((fixpoint *)&fdx));

for (y=ymin; y <= ymax; y++) {
  col = (fp_floor_pos(rx0yminf) + (fp_floor_pos(gx0yminf)<<8) +
	 (fp_floor_pos(bx0yminf)<< 16));
  z = zx0yminf + xleft[y]*dz_by_dx_f - dz_by_dx_x0f;
  for (x = xleft[y]; x <= xright[y]; x++) {
    if (zb[y][x] < z) {
      zb[y][x] = z;
      fb[y][x] = col + rgboffset[x];
    }
    z += dz_by_dx_f;
  }
  rx0yminf += dr_by_dy_f;
  gx0yminf += dg_by_dy_f;
  bx0yminf += db_by_dy_f;
  zx0yminf += dz_by_dy_f;
}
}/* rasterize_sorted_triangle */


void  sort_and_rasterize_triangle (ColorTriangleP tp)
{
int16       minyv = 0, midyv = 1, maxyv = 2, tmp = 0;
int16       minxv = 0, midxv = 1, maxxv = 2;

if ((tp->vertices[minyv]).vertex[Y] > (tp->vertices[midyv]).vertex[Y]) 
  TSWAP (minyv, midyv, tmp); 

if ((tp->vertices[minyv]).vertex[Y] > (tp->vertices[maxyv]).vertex[Y]) 
  TSWAP (minyv, maxyv, tmp);

if ((tp->vertices[midyv]).vertex[Y] > (tp->vertices[maxyv]).vertex[Y]) 
  TSWAP (midyv, maxyv, tmp);

if ((tp->vertices[minxv]).vertex[X] > (tp->vertices[midxv]).vertex[X]) 
  TSWAP (minxv, midxv, tmp); 

if ((tp->vertices[minxv]).vertex[X] > (tp->vertices[maxxv]).vertex[X]) 
  TSWAP (minxv, maxxv, tmp);

if ((tp->vertices[midxv]).vertex[X] > (tp->vertices[maxxv]).vertex[X]) 
  TSWAP (midxv, maxxv, tmp);

if ((ZFABS((tp->vertices[maxxv]).vertex[X] - (tp->vertices[minxv]).vertex[X])
     > MAT3_EPSILON) &&
    (ZFABS((tp->vertices[maxyv]).vertex[Y] - (tp->vertices[minyv]).vertex[Y])
     > MAT3_EPSILON))
  rasterize_sorted_triangle (minyv, midyv, maxyv, tp);
}


/* ------------------------------------------------------------------ */


/* Clipping */

int16 ClassifyVertex (MAT3fvec v, int16 plane, int16 val)
{
switch (plane)
  {
  case X : switch (val)
     {
      case 1  : return ( (v[W] >= 0.0) ? (v[X] > v[W]) : (v[X] < v[W]) );     
      case -1 :	return ( (v[W] >= 0.0) ? (v[X] < -v[W]) : (v[X] > -v[W]) );  
}
  case Y : switch (val)
     {
      case 1  : return ( (v[W] >= 0.0) ? (v[Y] > v[W]) : (v[Y] < v[W]) );     
      case -1 :	return ( (v[W] >= 0.0) ? (v[Y] < -v[W]) : (v[Y] > -v[W]) );  
}
  case Z : switch (val)
     {
      case 1  : return ( (v[W] >= 0.0) ? (v[Z] > v[W]) : (v[Z] < v[W]) );     
      case 0 :	return ( (v[W] >= 0.0) ? (v[Z] < 0.0) : (v[Z] > 0.0) );    
}
}
return (0);
}

int16 ClassifyTriangle (ColorTriangleP tp, int16 plane, int16 val)
{
return (	
  (ClassifyVertex (tp->vertices[0].vertex, plane, val) << 2) |
  (ClassifyVertex (tp->vertices[1].vertex, plane, val) << 1) |
  (ClassifyVertex (tp->vertices[2].vertex, plane, val)) 
	  );
}

void CopyVertex (Color_VertexP source, Color_VertexP dest)
{
int i;

for (i=0; i < 4; i++)
  dest->vertex[i] = source->vertex[i];
dest->red = source->red;
dest->green = source->green;
dest->blue = source->blue;
}

void CopyNormal (ColorTriangleP source, ColorTriangleP dest)
{
int i;

for (i=0; i < 3; i++)
  dest->normal[i] = source->normal[i];
}

void InterpolateVertex (Color_VertexP start, Color_VertexP end, float t, 
		        Color_VertexP dest)
{
int i;

for (i=0; i < 4; i++)
  dest->vertex[i] = start->vertex[i] + t*(end->vertex[i] - start->vertex[i]);

dest->red = start->red + t * (end->red - start->red);
dest->green = start->green + t * (end->green - start->green);
dest->blue = start->blue + t * (end->blue - start->blue);
}

float ComputeT (MAT3fvec vstart, MAT3fvec vend, int16 plane, int16 val)
{
switch (plane)
  {
  case X:
  case Y: switch (val)
    {
    case 1 : return ((vstart[plane] - vstart[W]) /
	             (vend[W] - vstart[W] - vend[plane] + vstart[plane]));
    case -1: return ((vstart[plane] + vstart[W]) /
	             (vstart[plane] - vend[plane] + vstart[W] - vend[W]));
}
   case Z: switch(val)
     {
    case 1 : return ((vstart[plane] - vstart[W]) /
	             (vend[W] - vstart[W] - vend[plane] + vstart[plane]));
    case 0 : return (vstart[plane] / (vstart[plane] - vend[plane] ));
     }
}
return (0.0);
}

void Clip_Process_Rasterize_Triangle(ColorTriangleP tp)
{
ColorTriangle cur_t[7];
int16 k, plane, val, l, maxtriangles, curmax, clipcode, inout[7], j;
float t1, t2;

CopyNormal(tp, &(cur_t[0]));

for (k=0; k < 3; k++)
  CopyVertex ((Color_VertexP) (&(tp->vertices[k])), 
	  (Color_VertexP) (&(cur_t[0].vertices[k])));

curmax = maxtriangles = 1;
inout[0] = 0;

for (plane = 0; plane < 3; plane++) {
  for (k = -1; k < 2; k += 2) {
    val = k;
    if ((plane == 2) && (val == -1)) val = 0;
    for (l = 0; l < maxtriangles; l++) {
      if (inout[l] == 0) {	    
      clipcode = ClassifyTriangle ((ColorTriangleP) (&cur_t[l]), plane, val);
      switch (clipcode) {
        case 0: break;
	case 1: t1 = ComputeT (cur_t[l].vertices[2].vertex, 
		           cur_t[l].vertices[0].vertex, plane, val);
                t2 = ComputeT (cur_t[l].vertices[1].vertex, 
		           cur_t[l].vertices[2].vertex, plane, val);
		CopyVertex((Color_VertexP) (&(cur_t[l].vertices[0])),
			(Color_VertexP) (&(cur_t[curmax].vertices[0])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[2])),
			(Color_VertexP) (&(cur_t[l].vertices[0])), t1,
			(Color_VertexP) (&(cur_t[curmax].vertices[2])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[1])),
			(Color_VertexP) (&(cur_t[l].vertices[2])), t2,
			(Color_VertexP) (&(cur_t[curmax].vertices[1])));
		CopyVertex((Color_VertexP) (&(cur_t[curmax].vertices[1])),
			(Color_VertexP) (&(cur_t[l].vertices[2])));
	        CopyNormal(&(cur_t[l]), &(cur_t[curmax]));
		inout[curmax] = 0;
		curmax++;
		break;
	case 2: t1 = ComputeT (cur_t[l].vertices[0].vertex, 
		           cur_t[l].vertices[1].vertex, plane, val);
                t2 = ComputeT (cur_t[l].vertices[1].vertex, 
		           cur_t[l].vertices[2].vertex, plane, val);
		CopyVertex((Color_VertexP) (&(cur_t[l].vertices[0])),
			(Color_VertexP) (&(cur_t[curmax].vertices[0])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[0])),
			(Color_VertexP) (&(cur_t[l].vertices[1])), t1,
			(Color_VertexP) (&(cur_t[curmax].vertices[1])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[1])),
			(Color_VertexP) (&(cur_t[l].vertices[2])), t2,
			(Color_VertexP) (&(cur_t[curmax].vertices[2])));
		CopyVertex((Color_VertexP) (&(cur_t[curmax].vertices[2])),
			(Color_VertexP) (&(cur_t[l].vertices[1])));
	        CopyNormal(&(cur_t[l]), &(cur_t[curmax]));
		inout[curmax] = 0;
		curmax++;
		break;
	case 3: t1 = ComputeT (cur_t[l].vertices[0].vertex, 
		           cur_t[l].vertices[1].vertex, plane, val);
                t2 = ComputeT (cur_t[l].vertices[2].vertex, 
		           cur_t[l].vertices[0].vertex, plane, val);
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[0])),
			(Color_VertexP) (&(cur_t[l].vertices[1])), t1,
			(Color_VertexP) (&(cur_t[l].vertices[1])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[2])),
			(Color_VertexP) (&(cur_t[l].vertices[0])), t2,
			(Color_VertexP) (&(cur_t[l].vertices[2])));
		break;
	case 4: t1 = ComputeT (cur_t[l].vertices[2].vertex, 
		           cur_t[l].vertices[0].vertex, plane, val);
                t2 = ComputeT (cur_t[l].vertices[0].vertex, 
		           cur_t[l].vertices[1].vertex, plane, val);
		CopyVertex((Color_VertexP) (&(cur_t[l].vertices[1])),
			(Color_VertexP) (&(cur_t[curmax].vertices[0])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[2])),
			(Color_VertexP) (&(cur_t[l].vertices[0])), t1,
			(Color_VertexP) (&(cur_t[curmax].vertices[1])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[0])),
			(Color_VertexP) (&(cur_t[l].vertices[1])), t2,
			(Color_VertexP) (&(cur_t[curmax].vertices[2])));
		CopyVertex((Color_VertexP) (&(cur_t[curmax].vertices[1])),
			(Color_VertexP) (&(cur_t[l].vertices[0])));
	        CopyNormal(&(cur_t[l]), &(cur_t[curmax]));
		inout[curmax] = 0;
		curmax++;
		break;
	case 5: t1 = ComputeT (cur_t[l].vertices[0].vertex, 
		           cur_t[l].vertices[1].vertex, plane, val);
                t2 = ComputeT (cur_t[l].vertices[1].vertex, 
		           cur_t[l].vertices[2].vertex, plane, val);
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[0])),
			(Color_VertexP) (&(cur_t[l].vertices[1])), t1,
			(Color_VertexP) (&(cur_t[l].vertices[0])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[1])),
			(Color_VertexP) (&(cur_t[l].vertices[2])), t2,
			(Color_VertexP) (&(cur_t[l].vertices[2])));
		break;
	case 6: t1 = ComputeT (cur_t[l].vertices[2].vertex, 
		           cur_t[l].vertices[0].vertex, plane, val);
                t2 = ComputeT (cur_t[l].vertices[1].vertex, 
		           cur_t[l].vertices[2].vertex, plane, val);
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[2])),
			(Color_VertexP) (&(cur_t[l].vertices[0])), t1,
			(Color_VertexP) (&(cur_t[l].vertices[0])));
		InterpolateVertex((Color_VertexP) (&(cur_t[l].vertices[1])),
			(Color_VertexP) (&(cur_t[l].vertices[2])), t2,
			(Color_VertexP) (&(cur_t[l].vertices[1])));
		break;
	case 7: inout[l] = 1;
	        break;
	}
}
}
   maxtriangles = curmax;
}
}
for (k=0; k < maxtriangles; k++)
 if (inout[k] == 0) {
   for (j=0; j < 3; j++) { 
     cur_t[k].vertices[j].vertex[X] = (cur_t[k].vertices[j].vertex[X] + 
             	    cur_t[k].vertices[j].vertex[W]) * (WW - 1)/2.0;
     cur_t[k].vertices[j].vertex[Y] = (cur_t[k].vertices[j].vertex[Y] + 
             	    cur_t[k].vertices[j].vertex[W]) * (WH - 1)/2.0;
   }
   for (j=0; j < 3; j++) {
      cur_t[k].vertices[j].vertex[X] = cur_t[k].vertices[j].vertex[X] /
			      cur_t[k].vertices[j].vertex[W];
      cur_t[k].vertices[j].vertex[Y] = cur_t[k].vertices[j].vertex[Y] /
			      cur_t[k].vertices[j].vertex[W];
      cur_t[k].vertices[j].vertex[Z] = cur_t[k].vertices[j].vertex[Z] /
		      cur_t[k].vertices[j].vertex[W];
      cur_t[k].vertices[j].vertex[X] = ZFABS(cur_t[k].vertices[j].vertex[X]);
      cur_t[k].vertices[j].vertex[Y] = ZFABS(cur_t[k].vertices[j].vertex[Y]);
      cur_t[k].vertices[j].vertex[Z] = ZFABS(cur_t[k].vertices[j].vertex[Z]);
   }

   sort_and_rasterize_triangle ((ColorTriangleP) (&(cur_t[k])));
 }
}


/* ------------------------------------------------------------------ */


/* Viewing Transformation */

#define mathpointmult(x,y,w,vec,mat)  {\
  (x) = (vec)[0] * (mat)[0][0] + \
                    (vec)[1] * (mat)[0][1] + \
                    (vec)[2] * (mat)[0][2] + \
                    (mat)[0][3]; \
                                 \
  (y) = (vec)[0] * (mat)[1][0] + \
                    (vec)[1] * (mat)[1][1] + \
                    (vec)[2] * (mat)[1][2] + \
                    (mat)[1][3]; \
                                 \
  (w) = (vec)[0] * (mat)[3][0] + \
                    (vec)[1] * (mat)[3][1] + \
                    (vec)[2] * (mat)[3][2] + \
                    (mat)[3][3]; \
} /* mathpointmult */

void EvaluateViewTransformationMatrix (MAT3vec view_ref_point, 
				       MAT3vec view_plane_normal,
				       MAT3vec view_up_vector,
				       double kay, double ell,
				       double Hither, double Yon,
				       MAT3fmat vo_inverse,  
				       MAT3mat transform, float *b)
{
MAT3vec     uvec, vvec;
MAT3mat     trans, rot, vo_matrix, transinv, rotinv, vo_inverse_d, vm_matrix;
double      det, zmin, tmp1;
int16       i, j;

MAT3_NORMALIZE_VEC(view_plane_normal, tmp1);
MAT3_CROSS_PRODUCT(uvec, view_up_vector, view_plane_normal);
MAT3_NORMALIZE_VEC(uvec, tmp1);
MAT3_CROSS_PRODUCT(vvec,  view_plane_normal, uvec);

MAT3identity (trans);
for (i=0; i < 3; i++)
  trans[i][3] = -view_ref_point[i];

MAT3identity (rot);

for(i=0; i < 3; i++)
  rot[0][i] = uvec[i];
for(i=0; i < 3; i++)
  rot[1][i] = vvec[i];
for(i=0; i < 3; i++)
  rot[2][i] = view_plane_normal[i];

MAT3mult (vo_matrix,  rot, trans); 
MAT3identity (transinv);
for (i=0; i < 3; i++)
  transinv[i][3] = view_ref_point[i];

MAT3identity (rotinv);
det = rot[0][0] * Det2D(rot[1][1],rot[1][2],rot[2][1],rot[2][2])
  -rot[0][1] * Det2D(rot[1][0],rot[1][2],rot[2][0],rot[2][2])
    +rot[0][2] * Det2D(rot[1][0],rot[1][1],rot[2][0],rot[2][1]);

rotinv[0][0] = Det2D(rot[1][1],rot[1][2],rot[2][1],rot[2][2])/det;
rotinv[1][0] = -Det2D(rot[1][0],rot[1][2],rot[2][0],rot[2][2])/det;
rotinv[2][0] = Det2D(rot[1][0],rot[1][1],rot[2][0],rot[2][1])/det;

rotinv[0][1] = -Det2D(rot[0][1],rot[0][2],rot[2][1],rot[2][2])/det;
rotinv[1][1] = Det2D(rot[0][0],rot[0][2],rot[2][0],rot[2][2])/det;
rotinv[2][1] = -Det2D(rot[0][0],rot[0][1],rot[2][0],rot[2][1])/det;

rotinv[0][2] = Det2D(rot[0][1],rot[0][2],rot[1][1],rot[1][2])/det;
rotinv[1][2] = -Det2D(rot[0][0],rot[0][2],rot[1][0],rot[1][2])/det;
rotinv[2][2] = Det2D(rot[0][0],rot[0][1],rot[1][0],rot[1][1])/det;

MAT3mult(vo_inverse_d,transinv,rotinv);

for (i=0; i < 4; i++)
  for (j=0; j < 4; j++)
    vo_inverse[i][j] = vo_inverse_d[i][j];

MAT3identity  (vm_matrix);
vm_matrix[0][0] = vm_matrix[1][1] = 
  ell/ (kay * Yon);
vm_matrix[2][2] = 1/Yon;
vm_matrix[2][3] = -ell/Yon;

zmin = - Hither/Yon;
MAT3mult (transform, vm_matrix, vo_matrix);
for (j=0; j < 4; j++)
  transform[3][j] = -transform[2][j];
*b = -zmin/(1.0 + zmin);

for (j=0; j < 3; j++)
  transform[2][j] *= (1.0+*b);

transform[2][3] = (1.0 + *b) * transform[2][3] + *b; 
}


/* ------------------------------------------------------------------ */


/* Trivial Accept and Reject */

void MAT3fmult_hvec(MAT3fvec result_vec, float a, float b, float c,
		    MAT3fmat  mat)
{
result_vec[0] = a*mat[0][0] + b*mat[0][1] + c*mat[0][2] + mat[0][3];
result_vec[1] = a*mat[1][0] + b*mat[1][1] + c*mat[1][2] + mat[1][3];
result_vec[2] = a*mat[2][0] + b*mat[2][1] + c*mat[2][2] + mat[2][3];
result_vec[3] = 1.0;
}

void set_bounds(MAT3fvec curpt, float xval, float yval, float zval,
		MAT3fmat vo_inverse)
{
MAT3fmult_hvec(curpt,xval,yval,zval,vo_inverse);
out_xmin = out_xmax = curpt[X];
out_ymin = out_ymax = curpt[Y];
out_zmin = out_zmax = curpt[Z];
}

void update_bounds(MAT3fvec curpt, float xval, float yval, float zval,
	           MAT3fmat vo_inverse)
{ 
MAT3fmult_hvec(curpt,xval,yval,zval,vo_inverse); 
out_xmin = min(curpt[X],out_xmin);
out_xmax = max(curpt[X],out_xmax);
out_ymin = min(curpt[Y],out_ymin);
out_ymax = max(curpt[Y],out_ymax);
out_zmin = min(curpt[Z],out_zmin);
out_zmax = max(curpt[Z],out_zmax);
}


/* ------------------------------------------------------------------ */


/* Reading Input */

void readNFFFile (char *filename, MAT3fvec from, float *frustumf, 
		  float *frustumr, MAT3vec vrp, MAT3vec vpn, MAT3vec vupv,
		  double *ellp, double *Hitherp, double *Yonp, double *kayp,
   	       Lights lights, BackGroundColor *backgndcolor, int16 *datasizep, 
              MtlProp *activeProp, int16 *currentlight, SpecLightP spec_lightp)
{

FILE *infile;
char string[80];
int16 i, count, state = WAITING, currentTriangle=-1;
double angle, delta[3], lx, ly, lz;

*currentlight = 0;
infile = fopen (filename,"r");
while (fgets (&string[0],80,infile) != NULL) 
  {
  if (state == WAITING) 
    {
    switch (string[0])
      {
      case 'v':
        state = VIEWING; /* Read View Specification */
        break;
      case 'b':  /* Background Color */
        sscanf (&string[1]," %lf %lf %lf\n", &lx, &ly, &lz);
        backgndcolor->red   = MAX_INTENSITY * lx;
        backgndcolor->green = MAX_INTENSITY * ly;
        backgndcolor->blue  = MAX_INTENSITY * lz;
        break;

      case 'l': /* Light Source */
        sscanf (&string[1], " %f %f %f %f %f %f",
		&lights[*currentlight].location[0], 
		&lights[*currentlight].location[1],
		&lights[*currentlight].location[2],
		&lights[*currentlight].red,
		&lights[*currentlight].green,
		&lights[*currentlight].blue);
	(*currentlight)++;
	break;
      case 's': /* PHONG Light Source */
	sscanf (&string[1], " %f %f %f %f %f %f %f %d",
		&spec_lightp->location[0],
		&spec_lightp->location[1],
		&spec_lightp->location[2],
		&spec_lightp->red,
		&spec_lightp->green,
		&spec_lightp->blue,
		&spec_lightp->ks,
		&spec_lightp->spec_exp);
	spec_lightp->location[3] = 1.0;
	break;
	
      case 'f':  /* Lighting Constants */
	sscanf (&string[1], " %f %f %f %f %f %f %f",
		&(*activeProp).red,
		&(*activeProp).green,
		&(*activeProp).blue, 
		&(*activeProp).diffuseK,
		&(*activeProp).ambientK,
		&(*activeProp).c1,
		&(*activeProp).c2);
	break;

      case 'p': /* Triangle to Follow */
	state = TRIDATA;
	count = 0;
	currentTriangle++;
	break;
      } /* switch on first char of line */
  } /* if in waiting state */
  else if (state == VIEWING) 
    {
    switch(string[0])
      {
      case 'f':   /* View point location */
	sscanf (&string[4], " %f %f %f\n", &from[0], &from[1], &from[2]);
	from[W] = 1.0;
	break;
	
      case 'a':
	if (string[1] == 't') /* Look at */
          sscanf(&string[2], " %lf %lf %lf\n", &vrp[0], &vrp[1], &vrp[2]);
	else if (string[1] == 'n')     /* Angle */
          sscanf(&string[5], "%lf\n", &angle);
	break;

      case 'u': /* Up Vector */
	sscanf (&string[2], " %lf %lf %lf\n", &vupv[0], &vupv[1], &vupv[2]);
	break;

      case 'h':  /* Front or Hither Clipping Plane */
	sscanf (&string[6], " %lf\n", Hitherp);
	break;
	
      case 'y': /* Back or Yon Clipping Plane */
	sscanf (&string[3], " %lf\n", Yonp);
	break;
	
      case 'r':  /* resolution is currently ignored */
	for (i=0; i < 3; i++)
	  delta[i] = from[i] - vrp[i];
	*ellp = sqrt(delta[0]*delta[0]+delta[1]* delta[1]+delta[2]*delta[2]);
	for (i=0; i < 3; i++)
	  vpn[i] = (from[i] - vrp[i])/ (*ellp);
	*kayp = (*ellp) * tan(angle*M_PI/360.0);
	*frustumf = (*kayp) * (*Hitherp)/(*ellp);
	*frustumr = (*kayp) * (*Yonp)/(*ellp); 
	state = WAITING;
	break;
      } /* switch first char in line */
  } /* else if in viewing state */
  else if (state == TRIDATA)
    {
    sscanf (string,"%f%f%f%f%f%f\n", 
	    &(localSet[currentTriangle].vertices[count].vertex[0]), 
	    &(localSet[currentTriangle].vertices[count].vertex[1]),
	    &(localSet[currentTriangle].vertices[count].vertex[2]), 
	    &(localSet[currentTriangle].vertices[count].normal[0]),
	    &(localSet[currentTriangle].vertices[count].normal[1]),
	    &(localSet[currentTriangle].vertices[count].normal[2]));
    if (count == 2)
      state = WAITING;
    else
      count++;
  } /* else if just read in triangle data */
} /* while not eof */
fclose(infile);
*datasizep = currentTriangle + 1;
} /* readNFFFile */


/* ------------------------------------------------------------------ */


/* Backface Culling, Trivial Accept and Reject, Lighting, 
   Viewing Transformation and Clip Check */

/*
 *  Processes a triangle. It also orients the triangle edges so that
 *  y(vertex 0) <= y(vertex 1) <= y(vertex 2).
 */

void ProcessTriangle (OrigTriangleP tp, MAT3fvec from,
			MAT3fmat transform, float b,
			float Iar, float Iag, float Iab,
			float c1, float c2, int16 num_lights, Lights lights)
{
ColorTriangle cur_t;
int16           j,k;
MAT3fvec   temp1;
float    dist, tmp3;

if (BFCULL)
  if ((from[X]*tp->normal[X]+from[Y]*tp->normal[Y]+from[Z]*tp->normal[Z])
      < tp->v0dotn)
    return;

if (TRIVIAL_REJECT) {
  if ((tp->vertices[0].vertex[X] < out_xmin) &&
      (tp->vertices[1].vertex[X] < out_xmin) &&
      (tp->vertices[2].vertex[X] < out_xmin)) {
    return;	      
  }
  if ((tp->vertices[0].vertex[X] > out_xmax) &&
      (tp->vertices[1].vertex[X] > out_xmax) &&
      (tp->vertices[2].vertex[X] > out_xmax)) {
    return;	      
  }
  if ((tp->vertices[0].vertex[Y] < out_ymin) &&
      (tp->vertices[1].vertex[Y] < out_ymin) &&
      (tp->vertices[2].vertex[Y] < out_ymin)) {
    return;	      
  }
  if ((tp->vertices[0].vertex[Y] > out_ymax) &&
      (tp->vertices[1].vertex[Y] > out_ymax) &&
      (tp->vertices[2].vertex[Y] > out_ymax)) {
    return;	      
  }
  if ((tp->vertices[0].vertex[Z] < out_zmin) &&
      (tp->vertices[1].vertex[Z] < out_zmin) &&
      (tp->vertices[2].vertex[Z] < out_zmin)) {
    return;	      
  }
  if ((tp->vertices[0].vertex[Z] > out_zmax) &&
      (tp->vertices[1].vertex[Z] > out_zmax) &&
      (tp->vertices[2].vertex[Z] > out_zmax)) {
    return;	      
  }
}

for (j=0; j < 3; j++)
  cur_t.normal[j] = tp->normal[j];

for (j = 0; j < 3; j++) {
  cur_t.vertices[j].red = Iar;
  cur_t.vertices[j].green = Iag; 
  cur_t.vertices[j].blue = Iab;
  for (k = 0; k < num_lights; k++) {
    MAT3_SUB_VEC(temp1,lights[k].location,tp->vertices[j].vertex);
    
    dist = sqrt (temp1[X]*temp1[X] + temp1[Y]*temp1[Y] + temp1[Z]*temp1[Z]);
    dist = 1 / (dist * (1 + dist * (c1 + c2*dist)));
    tmp3 = dist * ZFABS(MAT3_DOT_PRODUCT(temp1,tp->vertices[j].normal));
    cur_t.vertices[j].red += lights[k].red * tmp3;
    cur_t.vertices[j].green += lights[k].green * tmp3;
    cur_t.vertices[j].blue += lights[k].blue * tmp3;
  } /* for k */
} /* for j */
{
float x[3], y[3], z[3], w[3];	

for( j = 0; j < 3; j++) {
  mathpointmult (x[j],y[j],w[j],tp->vertices[j].vertex,transform)
    z[j] = b - w[j]*b; 
} /* for j */
    
if ((ZFABS(x[0]) <= ZFABS(w[0])) && (ZFABS(y[0]) <= ZFABS(w[0])) && 
    (ZFABS(z[0])<=ZFABS(w[0])) && ((w[0] >= 0.0)?(z[0] >= 0.0):(z[0]<0.0)) && 
    (ZFABS(x[1]) <= ZFABS(w[1])) && (ZFABS(y[1]) <= ZFABS(w[1])) && 
    (ZFABS(z[1])<=ZFABS(w[1])) && ((w[1] >= 0.0)?(z[1] >= 0.0):(z[1]<0.0)) && 
    (ZFABS(x[2]) <= ZFABS(w[2])) && (ZFABS(y[2]) <= ZFABS(w[2])) && 
    (ZFABS(z[2])<=ZFABS(w[2])) && ((w[2] >= 0.0)?(z[2] >= 0.0):(z[2]<0.0)) 
    ) {
  for (j=0; j < 3; j++) { 
    x[j] = (x[j] + w[j]) * (WW - 1)/2.0; 
    y[j] = (y[j] + w[j]) * (WH - 1)/2.0; 
  }
  for (j=0; j < 3; j++) {
    cur_t.vertices[j].vertex[X] = x[j]/w[j];
    cur_t.vertices[j].vertex[Y] = y[j]/w[j];
    cur_t.vertices[j].vertex[Z] = z[j]/w[j];
  }
  for (j=0; j < 3; j++) {		
    cur_t.vertices[j].vertex[X] = ZFABS(cur_t.vertices[j].vertex[X]);
    cur_t.vertices[j].vertex[Y] = ZFABS(cur_t.vertices[j].vertex[Y]);
    cur_t.vertices[j].vertex[Z] = ZFABS(cur_t.vertices[j].vertex[Z]);
  } 
  sort_and_rasterize_triangle ((ColorTriangleP) (&cur_t));
}
else if (CLIP) {
  for (j=0; j < 3; j++) {		
    cur_t.vertices[j].vertex[X] = x[j];
    cur_t.vertices[j].vertex[Y] = y[j];
    cur_t.vertices[j].vertex[Z] = z[j];
    cur_t.vertices[j].vertex[W] = w[j];
  }
  Clip_Process_Rasterize_Triangle((ColorTriangleP) (&cur_t));  
}
}
} /* ProcessTriangle */

/*
 *  The function processes a set of triangles one at a time.
 */

void PipelineCompute (int16 count, MAT3fvec from,
		       MAT3fmat transform, float b,
		       float Iar, float Iag, float Iab, float c1, float c2,
		       int16 num_lights, Lights lights)
{
int16 i;

for (i = 0; i < count; i ++) 
    ProcessTriangle (&localSet[i], from, transform, b,
		       Iar, Iag, Iab, c1, c2, num_lights, lights);
} /* Pipeline Compute */



/* ------------------------------------------------------------------ */


void main(int argc, char **argv)
{
int16 i, j, tmp;
int16 datasize, num_lights, scale_factor;
MAT3vec vrp, vpn, vupv;
MAT3fvec from, temp1, temp2, curpt, Lvector, Vvector, SpecLocNPC; 
double ell, Hither, Yon, kay;
MAT3mat transform;
float mag, b, frustumf, frustumr, ffplane, fbplane;
MAT3fmat vo_inverse, ftransform;
MtlProp activeProp;
BackGroundColor backgndcolor;
color fbackgndcolor;
LightPoint lights[NUM_LIGHT_SOURCES];

if (argc < 7) {
  printf ("Usage: ZRndv10 <bf> <tr> <clip> <phong> <datafile> <outputfile>\n");
  return;
}
if (strcmp(argv[1],"-bf")) 
  BFCULL = 0;
else
  BFCULL = 1;

if (strcmp(argv[2],"-tr")) 
  TRIVIAL_REJECT = 0;
else
  TRIVIAL_REJECT = 1;
  
if (strcmp(argv[3],"-clip")) 
  CLIP = 0;
else
  CLIP = 1;
if (strcmp(argv[4],"-phong")) 
  PHONG = 0;
else
  PHONG = 1;
    
readNFFFile (argv[5], from, &frustumf, &frustumr, vrp, vpn, vupv,
	     &ell, &Hither, &Yon, &kay, lights, &backgndcolor,
	     &datasize, &activeProp, &num_lights, &SpecSource);

for (i=0; i < datasize; i++) {
  MAT3_SUB_VEC(temp1,localSet[i].vertices[1].vertex,
	       localSet[i].vertices[0].vertex);
  MAT3_SUB_VEC(temp2,localSet[i].vertices[2].vertex,
	       localSet[i].vertices[0].vertex);
  MAT3_CROSS_PRODUCT(localSet[i].normal,temp1,temp2);
  MAT3_NORMALIZE_VEC(localSet[i].normal,mag);
  
  localSet[i].v0dotn=localSet[i].vertices[0].vertex[X]*localSet[i].normal[X]+ 
    localSet[i].vertices[0].vertex[Y]*localSet[i].normal[Y]+ 
      localSet[i].vertices[0].vertex[Z]*localSet[i].normal[Z]; 
}

if (PHONG)
  scale_factor = num_lights + 2;
else
  scale_factor = num_lights + 1;

activeProp.red = activeProp.red * activeProp.ambientK * MAX_INTENSITY / 
  scale_factor;
activeProp.green = activeProp.green * activeProp.ambientK * MAX_INTENSITY /
  scale_factor;
activeProp.blue = activeProp.blue * activeProp.ambientK * MAX_INTENSITY / 
  scale_factor;

for (j=0; j < num_lights; j++) {
  lights[j].red *= activeProp.diffuseK * MAX_INTENSITY / scale_factor;
  lights[j].green *= activeProp.diffuseK * MAX_INTENSITY / scale_factor;
  lights[j].blue *= activeProp.diffuseK * MAX_INTENSITY / scale_factor;
}

fbackgndcolor  = ((int32) backgndcolor.red) + 
  (((int32) backgndcolor.green) << 8) + (((int32) backgndcolor.blue) << 16);

EvaluateViewTransformationMatrix (vrp, vpn, vupv, kay, ell, Hither, Yon,
				    vo_inverse, transform, &b);
    			    
for (i=0; i < 4; i++)
  for (j=0; j < 4; j++)
    ftransform[i][j] = transform[i][j];

if (PHONG) {
  SpecSource.red *= SpecSource.ks * MAX_INTENSITY/scale_factor;
  SpecSource.green *= SpecSource.ks * MAX_INTENSITY/scale_factor;
  SpecSource.blue *= SpecSource.ks * MAX_INTENSITY/scale_factor;
  mathpointmult (SpecLocNPC[X],SpecLocNPC[Y],SpecLocNPC[W],
		 SpecSource.location,ftransform)
  SpecLocNPC[Z] = b - SpecLocNPC[W]*b; 
 
  SpecLocNPC[X] = (SpecLocNPC[X] + SpecLocNPC[W]);
  SpecLocNPC[Y] = (SpecLocNPC[Y] + SpecLocNPC[W]);

  for (i=0; i < 3; i++) 
    SpecLocNPC[i] /= SpecLocNPC[W];

  MAT3_SUB_VEC(Lvector,SpecSource.location,vrp);
  MAT3_SUB_VEC(Vvector,from,vrp);
  for(i=0; i < 3; i++)
    Hvector[i] = Lvector[i] + Vvector[i];
  MAT3_NORMALIZE_VEC(Hvector,mag);
}

ffplane = ell - Hither; fbplane = ell - Yon;
set_bounds (curpt,frustumf,frustumf,ffplane,vo_inverse);
update_bounds (curpt,-frustumf,-frustumf,ffplane,vo_inverse);
update_bounds (curpt,-frustumf,frustumf,ffplane,vo_inverse);
update_bounds (curpt,frustumf,-frustumf,ffplane,vo_inverse);
update_bounds (curpt,frustumr,frustumr,fbplane,vo_inverse);
update_bounds (curpt,-frustumr,-frustumr,fbplane,vo_inverse);
update_bounds (curpt,-frustumr,frustumr,fbplane,vo_inverse);
update_bounds (curpt,frustumr,-frustumr,fbplane,vo_inverse);

for (i=0; i < WH; i++)
  for (j=0; j < WW; j++)
    fb[i][j] = fbackgndcolor;

for (i=0; i < WH; i++)
  for (j=0; j < WW; j++)
    zb[i][j] = 0;  

PipelineCompute (datasize, from, ftransform, b,
		 activeProp.red, activeProp.green, activeProp.blue, 
		 activeProp.c1, activeProp.c2, num_lights, lights);

write_tga_buffer (fb, argv[6]);
} 

