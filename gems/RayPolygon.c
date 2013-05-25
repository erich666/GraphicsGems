/*
An Efficient Ray/Polygon Intersection
by Didier Badouel
from "Graphics Gems", Academic Press, 1990

just code, not a procedure.
*/

/* the value of t is computed.
 * i1 and i2 come from the polygon description.
 * V is the vertex table for the polygon and N the
 * associated normal vectors.
 */
P[0] = ray.O[0] + ray.D[0]*t;
P[1] = ray.O[1] + ray.D[1]*t;
P[2] = ray.O[2] + ray.D[2]*t;
u0 = P[i1] - V[0][i1]; v0 = P[i2] - V[0][i2];
inter = FALSE; i = 2;
do {
    /* The polygon is viewed as (n-2) triangles. */
    u1 = V[i-1][i1] - V[0][i1]; v1 = V[i-1][i2] - V[0][i2];
    u2 = V[i  ][i1] - V[0][i1]; v2 = V[i  ][i2] - V[0][i2];

    if (u1 == 0)    {
        beta = u0/u2;
        if ((beta >= 0.)&&(beta <= 1.)) {
            alpha = (v0 - beta*v2)/v1;
            inter = ((alpha >= 0.)&&(alpha+beta) <= 1.));
        }
    } else {
        beta = (v0*u1 - u0*v1)/(v2*u1 - u2*v1);
        if ((beta >= 0.)&&(beta <= 1.)) {
            alpha = (u0 - beta*u2)/u1;
            inter = ((alpha >= 0)&&((alpha+beta) <= 1.));
        }
    }
} while ((!inter)&&(++i < poly.n));

if (inter) {
    /* Storing the intersection point. */
    ray.P[0] = P[0]; ray.P[1] = P[1]; ray.P[2] = P[2];
    /* the normal vector can be interpolated now or later. */
    if (poly.interpolate) {
        gamma = 1 - (alpha+beta);
        ray.normal[0] = gamma * N[0][0] + alpha * N[i-1][0] + 
		beta * N[i][0];
        ray.normal[1] = gamma * N[0][1] + alpha * N[i-1][1] +
		 beta * N[i][1];
        ray.normal[2] = gamma * N[0][2] + alpha * N[i-1][2] +
		 beta * N[i][2];
    }
}
return (inter);
