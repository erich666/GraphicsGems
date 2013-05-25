
/*
 * Author: Filippo Tampieri
 */

#define FALSE 0
#define TRUE  1
#define DOT(A,B) (A[0] * B[0] + A[1] * B[1] + A[2] * B[2])

/*
    vertexIsBehindPlane returns TRUE if point P is behind the
    plane of normal N and coefficient d, FALSE otherwise.
*/
vertexIsBehindPlane(P, N, d)
float P[3], N[3], d;
{
    return(DOT(N, P) + d <= 0. ? TRUE : FALSE);
}

/*
    boxIsBehindPlane returns TRUE if the axis-aligned box of
    minimum corner Cmin and maximum corner Cmax is behind the
    plane of normal N and coefficient d, FALSE otherwise.
*/
boxIsBehindPlane(Cmin, Cmax, N, d)
float Cmin[3], Cmax[3], N[3], d;
{
    register int i;
    float P[3];

    /*
        assign to P the corner further away
        along the direction of normal N
    */
    for(i = 0; i < 3; i++)
        P[i] = N[i] >= 0. ? Cmax[i] : Cmin[i];

    /* test P against the input plane */
    return(vertexIsBehindPlane(P, N, d));
}




