#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#define PI 3.141592653589793324
#define GeoZeroVec(v) ((v).x = (v).y = (v).z = 0.0)
#define GeoMultVec(a,b,c) do {(c).x = a*(b).x; (c).y = a*(b).y;	(c).z = a*(b).z; } while (0)
#define Geo_Vet(a,b,c) do {(c).x = (b).x-(a).x; (c).y = (b).y-(a).y; (c).z = (b).z-(a).z;} while (0)

typedef double Rdouble;
typedef float  Rfloat;
typedef struct _GeoPoint { Rfloat x, y, z; } GeoPoint;

/*=========================  Geometrical Procedures  ======================= */

Rdouble GeoDotProd ( GeoPoint *vec0, GeoPoint *vec1 )
{
 return ( vec0->x * vec1->x + vec0->y * vec1->y + vec0->z * vec1->z );
}

void GeoCrossProd ( GeoPoint *in0, GeoPoint *in1, GeoPoint *out )
{
 out->x = (in0->y * in1->z) - (in0->z * in1->y);
 out->y = (in0->z * in1->x) - (in0->x * in1->z);
 out->z = (in0->x * in1->y) - (in0->y * in1->x);
}

Rdouble GeoTripleProd ( GeoPoint *vec0, GeoPoint *vec1, GeoPoint *vec2 )
{
 GeoPoint tmp;

 GeoCrossProd ( vec0, vec1, &tmp );
 return ( GeoDotProd( &tmp, vec2 ) );
}

Rdouble GeoVecLen ( GeoPoint *vec )
{
 return sqrt ( GeoDotProd ( vec, vec ) );
}

int GeoPolyNormal ( int	n_verts, GeoPoint *verts, GeoPoint *n ) 
{
 int      i;
 Rfloat	  n_size;		
 GeoPoint v0, v1, p; 

 GeoZeroVec ( *n );
 Geo_Vet ( verts[0], verts[1], v0 );
 for ( i = 2; i < n_verts; i++ )
     {
      Geo_Vet ( verts[0], verts[i], v1 );
      GeoCrossProd ( &v0, &v1, &p );
      n->x += p.x; n->y += p.y; n->z += p.z;
      v0 = v1;
     }

 n_size = GeoVecLen ( n );
 if ( n_size > 0.0 )
    {
     GeoMultVec ( 1/n_size, *n, *n );
     return 1;
    }
 else
     return 0;
}

/*=========================  geo_solid_angle  =========================*/
/* 
  Calculates the solid angle given by the spherical projection of 
  a 3D plane polygon
*/

Rdouble geo_solid_angle ( 
        int      n_vert,  /* number of vertices */
        GeoPoint *verts,  /* vertex coordinates list */
        GeoPoint *p )     /* point to be tested */
{
 int      i;
 Rdouble  area = 0.0, ang, s, l1, l2;
 GeoPoint p1, p2, r1, a, b, n1, n2;
 GeoPoint plane;

 if ( n_vert < 3 ) return 0.0;

 GeoPolyNormal ( n_vert, verts, &plane );
 
 /* 
    WARNING: at this point, a practical implementation should check
    whether p is too close to the polygon plane. If it is, then
    there are two possibilities: 
      a) if the projection of p onto the plane is outside the 
         polygon, then area zero should be returned;
      b) otherwise, p is on the polyhedron boundary.
 */ 

 p2 = verts[n_vert-1];  /* last vertex */
 p1 = verts[0];         /* first vertex */
 Geo_Vet ( p1, p2, a ); /* a = p2 - p1 */  

 for ( i = 0; i < n_vert; i++ )
     {
      Geo_Vet(*p, p1, r1); 
      p2 = verts[(i+1)%n_vert];
      Geo_Vet ( p1, p2, b );
      GeoCrossProd ( &a, &r1, &n1 );
      GeoCrossProd ( &r1, &b, &n2 );
    
      l1 = GeoVecLen ( &n1 );
      l2 = GeoVecLen ( &n2 );
      s  = GeoDotProd ( &n1, &n2 ) / ( l1 * l2 );
      ang = acos ( max(-1.0,min(1.0,s)) );
      s = GeoTripleProd( &b, &a, &plane );
      area += s > 0.0 ? PI - ang : PI + ang;
     
      GeoMultVec ( -1.0, b, a );
      p1 = p2;
     }

 area -= PI*(n_vert-2);

 return ( GeoDotProd ( &plane, &r1 ) > 0.0 ) ? -area : area; 
}

/* ======================  main ========================== */

int main ( void ) 
{
 FILE     *f;
 char     s[32];
 int      nv, j;
 GeoPoint verts[100], p;
 Rdouble  Area =0.0;
     
 fprintf ( stdout, "\nFile Name: " );
 gets ( s );
 if ( (f = fopen ( s, "r" )) == NULL )
    {
     fprintf ( stdout, "Can not open the Polyhedron file \n" );
     exit ( 1 );
    }
 fprintf ( stdout, "\nPoint to be tested: " );
 fscanf( stdin, "%f %f %f", &p.x, &p.y, &p.z );

 while ( fscanf ( f, "%d", &nv ) == 1 )
       {	
        for ( j = 0; j < nv; j++ )
            if ( fscanf ( f, "%f %f %f", 
                 &verts[j].x, &verts[j].y, &verts[j].z ) != 3 )
               {
                fprintf ( stdout, "Invalid Polyhedron file \n" );
                exit ( 2 );
               }

        Area += geo_solid_angle ( nv, verts, &p ); 
       }
 	   
 fprintf ( stdout, "\n  Area = %12.4lf spherical radians.\n", Area);
 fprintf ( stdout, "\n  The point is %s", 
         ( (Area > 2*PI) || (Area < -2*PI) )? "inside" : "outside" );
 fprintf ( stdout, "the given polyhedron \n" );
 return 1;
}
