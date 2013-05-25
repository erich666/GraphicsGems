/*
A Simple Method for Box-Sphere Intersection Testing
by Jim Arvo
from "Graphics Gems", Academic Press, 1990
*/

#include "GraphicsGems.h"

/*
 *  This routine tests for intersection between an n-dimensional             
 *  axis-aligned box and an n-dimensional sphere.  The mode argument       
 *  indicates whether the objects are to be regarded as surfaces or        
 *  solids.  The values are:                                               
 *                                                                         
 *     mode                                                                
 *                                                                         
 *       0   Hollow Box, Hollow Sphere                                     
 *       1   Hollow Box, Solid  Sphere
 *       2   Solid  Box, Hollow Sphere                                     
 *       3   Solid  Box, Solid  Sphere                                     
 *                                                                         
*/
int Box_Sphere_Intersect( n, Bmin, Bmax, C, r, mode )
int    n;       /* The dimension of the space.           */
float  Bmin[];  /* The minimum of the box for each axis.  */
float  Bmax[];  /* The maximum of the box for each axis. */
float  C[];     /* The sphere center in n-space.         */
float  r;       /* The radius of the sphere.             */
int    mode;    /* Selects hollow or solid.              */
    {
    float  a, b;
    float  dmin, dmax;
    float  r2 = SQR( r );
    int    i, face;


    switch( mode )
        {
        case 0: /* Hollow Box - Hollow Sphere */
            dmin = 0;
            dmax = 0;
            face = FALSE;
            for( i = 0; i < n; i++ ) {
                a = SQR( C[i] - Bmin[i] );
                b = SQR( C[i] - Bmax[i] );
                dmax += MAX( a, b );
                if( C[i] < Bmin[i] ) {
                    face = TRUE;
                    dmin += a;
                    }
                else if( C[i] > Bmax[i] ) {
                    face = TRUE;
                    dmin += b;
                    }
                else if( MIN( a, b ) <= r2 ) face = TRUE;
                }
            if(face && ( dmin <= r2 ) && ( r2 <= dmax)) return(TRUE);
            break;

        case 1: /* Hollow Box - Solid Sphere */
            dmin = 0;
            face = FALSE;
            for( i = 0; i < n; i++ ) {
                if( C[i] < Bmin[i] ) {
                    face = TRUE;
                    dmin += SQR( C[i] - Bmin[i] );
                    }
                else if( C[i] > Bmax[i] ) {
                    face = TRUE;
                    dmin += SQR( C[i] - Bmax[i] );     
                    }
                else if( C[i] - Bmin[i] <= r ) face = TRUE;
                else if( Bmax[i] - C[i] <= r ) face = TRUE;
                }
            if( face && ( dmin <= r2 ) ) return( TRUE );
            break;


        case 2: /* Solid Box - Hollow Sphere */
            dmax = 0;
            dmin = 0;
            for( i = 0; i < n; i++ ) {
                a = SQR( C[i] - Bmin[i] );
                b = SQR( C[i] - Bmax[i] );
                dmax += MAX( a, b );
                if( C[i] < Bmin[i] ) dmin += a; else
                if( C[i] > Bmax[i] ) dmin += b;
                }
            if( dmin <= r2 && r2 <= dmax ) return( TRUE );
            break;

        case 3: /* Solid Box - Solid Sphere */
            dmin = 0;
            for( i = 0; i < n; i++ ) {
                if( C[i] < Bmin[i] ) dmin += SQR(C[i] - Bmin[i] ); else
                if( C[i] > Bmax[i] ) dmin += SQR( C[i] - Bmax[i] );     
                }
            if( dmin <= r2 ) return( TRUE );
            break;
  
        } /* end switch */

    return( FALSE );
    } 
