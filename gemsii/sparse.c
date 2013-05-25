/*
 *  Author: Jim Arvo
 *
 *  Corrected 11/05/91 to remove a redundant form.  The "C1" form as it
 *  appeared in GemsII was unnecessary.
 */

#include "GraphicsGems.h"

#define P1 (1<< 0)
#define P2 (1<< 1)
#define P3 (1<< 2)
#define P4 (1<< 3)
#define P5 (1<< 4)  
#define P6 (1<< 5)
#define RX (1<< 6)
#define RY (1<< 7)
#define RZ (1<< 8)
#define C1 (1<< 9)
#define C2 (1<<10)
#define C3 (1<<11)
#define C4 (1<<12)
#define C5 (1<<13)
#define C6 (1<<14)

/*-------------------------------------------------------------------------* 
 *                                                                         *
 * This function classifies a 3x3 matrix according to its zero structure.  * 
 * It returns an unsigned integer in which each bit signifies a zero       *
 * structure that describes the given matrix.  If all bits are zero it     *
 * means the matrix is dense or does not fit any of these 15 forms.        *
 *                                                                         *
 *                                                                         *
 *  Permutations:                                                          *
 *                                                                         *
 *  * 0 0     0 * 0     0 0 *     0 * 0     * 0 0      0 0 *               *
 *  0 * 0     0 0 *     * 0 0     * 0 0     0 0 *      0 * 0               *
 *  0 0 *     * 0 0     0 * 0     0 0 *     0 * 0      * 0 0               *
 *                                                                         *
 *   P1        P2        P3        P4        P5         P6                 *
 *                                                                         *
 *                                                                         *
 *  Simple Rotations:                                                      *
 *                                                                         *
 *  * 0 0     * 0 *     * * 0                                              *
 *  0 * *     0 * 0     * * 0                                              *
 *  0 * *     * 0 *     0 0 *                                              *
 *                                                                         *
 *   RX        RY        RZ                                                *
 *                                                                         *
 *                                                                         *
 *  Permutations of the simple rotations:                                  *
 *                                                                         *
 *  0 0 *     0 * *     0 * 0     * * 0     * 0 *     0 * *                *
 *  * * 0     0 * *     * 0 *     0 0 *     * 0 *     * 0 0                *
 *  * * 0     * 0 0     * 0 *     * * 0     0 * 0     0 * *                *
 *                                                                         *
 *   C1        C2        C3        C4        C5        C6                  *
 *                                                                         *
 *-------------------------------------------------------------------------*/
unsigned int classify_matrix( M )
Matrix3  M;
    {
    unsigned int form = 0xFFFF;

    /* Classify based on the diagonal elements. */

    if( M.element[0][0] != 0 ) form &= P1 | P5 | RX | RY | RZ | C4 | C5;
    if( M.element[1][1] != 0 ) form &= P1 | P6 | RX | RY | RZ | C1 | C2;
    if( M.element[2][2] != 0 ) form &= P1 | P4 | RX | RY | RZ | C3 | C6;

    /* Classify based on the upper triangular elements. */

    if( M.element[0][1] != 0 ) form &= P2 | P4 | RZ | C2 | C3 | C4 | C6;
    if( M.element[0][2] != 0 ) form &= P3 | P6 | RY | C1 | C2 | C5 | C6;
    if( M.element[1][2] != 0 ) form &= P2 | P5 | RX | C2 | C3 | C4 | C5;

    /* Classify based on the lower triangular elements. */

    if( M.element[1][0] != 0 ) form &= P3 | P4 | RZ | C1 | C3 | C5 | C6;
    if( M.element[2][0] != 0 ) form &= P2 | P6 | RY | C1 | C2 | C3 | C4;
    if( M.element[2][1] != 0 ) form &= P3 | P5 | RX | C1 | C4 | C5 | C6;

    /* If multiple classifications apply, return only one.  Doing this */
    /* makes it possible to do a "switch" on the returned value.       */

    if( form & P1 ) return( P1 );
    if( form & P2 ) return( P2 );
    if( form & P3 ) return( P3 );
    if( form & P4 ) return( P4 );
    if( form & P5 ) return( P5 );
    if( form & P6 ) return( P6 );
    if( form & RX ) return( RX );
    if( form & RY ) return( RY );
    if( form & RZ ) return( RZ );
    if( form & C1 ) return( C1 );
    if( form & C2 ) return( C2 );
    if( form & C3 ) return( C3 );
    if( form & C4 ) return( C4 );
    if( form & C5 ) return( C5 );
    if( form & C6 ) return( C6 );
    return( form );
    }

/*-------------------------------------------------------------------------* 
 *                                                                         *
 * This routine transforms a vector "v" by a matrix "M" whose structure    *
 * is specified by "form".  The result is returned in the vector "w".      *
 * Note that "v" and "w" cannot be the same vector.                        *
 *                                                                         *
 * All zero elements must still be stored in the matrix "M"; the form      *
 * argument simply indicates which ones to use.  Thus, no storage is       *
 * saved in this implementation -- just time.                              *
 *                                                                         *
 *-------------------------------------------------------------------------*/
void sparse_transform( M, form, v, w )
Matrix3 M;
unsigned int form;
Vector3 *v;
Vector3 *w;
    {
    switch( form )
        {
        case P1:
            w->x = M.element[0][0] * v->x;
            w->y = M.element[1][1] * v->y;
            w->z = M.element[2][2] * v->z;
            break;
        case P2:
            w->x = M.element[0][1] * v->x;
            w->y = M.element[1][2] * v->y;
            w->z = M.element[2][0] * v->z;
            break;
        case P3:
            w->x = M.element[0][2] * v->x;
            w->y = M.element[1][0] * v->y;
            w->z = M.element[2][1] * v->z;
            break;
        case P4:
            w->x = M.element[0][1] * v->x;
            w->y = M.element[1][0] * v->y;
            w->z = M.element[2][2] * v->z;
            break;
        case P5:
            w->x = M.element[0][0] * v->x;
            w->y = M.element[1][2] * v->y;
            w->z = M.element[2][1] * v->z;
            break;
        case P6:
            w->x = M.element[0][2] * v->x;
            w->y = M.element[1][1] * v->y;
            w->z = M.element[2][0] * v->z;
            break;
        case RX:
            w->x = M.element[0][0] * v->x;
            w->y = M.element[1][1] * v->y + M.element[1][2] * v->z;
            w->z = M.element[2][1] * v->y + M.element[2][2] * v->z;
            break;
        case RY: 
            w->x = M.element[0][0] * v->x + M.element[0][2] * v->z;
            w->y = M.element[1][1] * v->y;
            w->z = M.element[2][0] * v->x + M.element[2][2] * v->z;
            break;
        case RZ: 
            w->x = M.element[0][0] * v->x + M.element[0][1] * v->y;
            w->y = M.element[1][0] * v->x + M.element[1][1] * v->y;
            w->z = M.element[2][2] * v->z;
            break;
        case C1: 
            w->x = M.element[0][2] * v->z;
            w->y = M.element[1][0] * v->x + M.element[1][1] * v->y;
            w->z = M.element[2][0] * v->x + M.element[2][1] * v->y;
            break;
        case C2: 
            w->x = M.element[0][1] * v->y + M.element[0][2] * v->z;
            w->y = M.element[1][1] * v->y + M.element[1][2] * v->z;
            w->z = M.element[2][0] * v->x;
            break;
        case C3: 
            w->x = M.element[0][1] * v->y;
            w->y = M.element[1][0] * v->x + M.element[1][2] * v->z;
            w->z = M.element[2][0] * v->x + M.element[2][2] * v->z;
            break;
        case C4: 
            w->x = M.element[0][0] * v->x + M.element[0][1] * v->y;
            w->y = M.element[1][2] * v->z;
            w->z = M.element[2][0] * v->x + M.element[2][1] * v->y;
            break;
        case C5: 
            w->x = M.element[0][0] * v->x + M.element[0][2] * v->z;
            w->y = M.element[1][0] * v->x + M.element[1][2] * v->z;
            w->z = M.element[2][1] * v->y;
            break;
        case C6: 
            w->x = M.element[0][1] * v->y + M.element[0][2] * v->z;
            w->y = M.element[1][0] * v->x;
            w->z = M.element[2][1] * v->y + M.element[2][2] * v->z;
            break;
        default: 
            w->x = M.element[0][0] * v->x + M.element[0][1] * v->y + M.element[0][2] * v->z;
            w->y = M.element[1][0] * v->x + M.element[1][1] * v->y + M.element[1][2] * v->z;
            w->z = M.element[2][0] * v->x + M.element[2][1] * v->y + M.element[2][2] * v->z;
            break;
        } /* switch */

    } /* sparse_transform */







