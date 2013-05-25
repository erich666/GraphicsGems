/*
Transforming Axis-Aligned Bounding Boxes
by Jim Arvo
from "Graphics Gems", Academic Press, 1990
*/

#include "GraphicsGems.h"                                     
/* Transforms a 3D axis-aligned box via a 3x3 matrix and a translation
 * vector and returns an axis-aligned box enclosing the result. */ 

void Transform_Box( M, T, A, B )
Matrix3  M;  	/* Transform matrix.             */
Vector3  T;  	/* Translation matrix.           */
Box3     A;  	/* The original bounding box.    */
Box3    *B;  	/* The transformed bounding box. */
    {
    float  a, b;
    float  Amin[3], Amax[3];
    float  Bmin[3], Bmax[3];
    int    i, j;

    /*Copy box A into a min array and a max array for easy reference.*/

    Amin[0] = A.min.x;  Amax[0] = A.max.x;
    Amin[1] = A.min.y;  Amax[1] = A.max.y;
    Amin[2] = A.min.z;  Amax[2] = A.max.z;

    /* Take care of translation by beginning at T. */

    Bmin[0] = Bmax[0] = T.x;
    Bmin[1] = Bmax[1] = T.y;
    Bmin[2] = Bmax[2] = T.z;

    /* Now find the extreme points by considering the product of the */
    /* min and max with each component of M.  */
                     
    for( i = 0; i < 3; i++ )
    for( j = 0; j < 3; j++ )
        {
        a = M.element[i][j] * Amin[j];
        b = M.element[i][j] * Amax[j];
        if( a < b ) 

            { 
            Bmin[i] += a; 
            Bmax[i] += b;
            }
        else 
            { 
            Bmin[i] += b; 
            Bmax[i] += a;
            }
        }

    /* Copy the result into the new box. */

    B->min.x = Bmin[0];  B->max.x = Bmax[0];
    B->min.y = Bmin[1];  B->max.y = Bmax[1];
    B->min.z = Bmin[2];  B->max.z = Bmax[2];

    } 
