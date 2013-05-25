/*
Using Quaternions for Coding 3D Transformations
Patrick-Gilles Maillot
from "Graphics Gems", Academic Press, 1990
*/

extern double P[3], Q[4], M[4][4];

set_obs_position(x,y,z)
float	x, y, z;
{
int	i;

/*
 * Set the values of the eye's position.
 * The position here represents the position of the orthonormal base
 * in respect to the observer.
 */
   	P[0] = -x;
   	P[1] = -y;
   	P[2] = -z;
/*
 * Set the visualization to be in the decreasing x axis
 */
	Q[0] = 1.;
  	for (i = 1; i < 4; i++) Q[i] = 0.;
}

translate_quaternion(x,i,w)
float	x;
int	i, w;
{
int	j, k;
float	A, B, D, E, F;
	
	if (w < 0) {
/*
 * The observer moves in respect to the scene.
 */
	P[i - 1] -= x;
  } else {

/* 
 * The scene moves in respect to the observer.
 * Compute the successor axis of i [1,2,3];
 * and then the successor axis of j [1,2,3];
 */
    if ((j = i + 1) > 3) j = 1;
    if ((k = j + 1) > 3) k = 1;
    A = Q[j]; B = Q[k]; F = Q[0]; E = Q[i];
    P[i - 1] += x * (E * E + F * F - A * A - B * B);
    D = x + x;
    P[j - 1] += D * (E * A + F * B);
    P[k - 1] += D * (E * B + F * A);
  }
}

rotate_quaternion(x,y,i,w)
float	x, y;
int	i, w;
{
int	j, k;
float	E, F, R1;
/*
 * Compute the successor axis of i [1,2,3] and  j [1,2,3];
 */
	if ((j = i + 1) > 3) j = 1;
	if ((k = j + 1) > 3) k = 1;
	E = Q[i];
	Q[i] = E * x + w * y * Q[0];
	Q[0] = Q[0] * x - w * y * E;
	E = Q[j];
	Q[j] = E * x + y * Q[k];
	Q[k] = Q[k] * x - y * E;
  	if (w < 0) {
/* Compute a new position if the observer moves in respect to the scene. */
    	j -= 1; k -= 1;
    	R1 = x * x - y * y;
    	F = 2. * x * y;
    	E = P[j];
    	P[j] = E * R1 + F * P[k];
    	P[k] = P[k] * R1 - F * E;
  	}
}


Evaluate_matrix()
{
float	e, f, r[4];
int	i, j, k;
/*
 * We will need some square values!
 */
	for (i = 0; i < 4; i++) r[i] = Q[i] * Q[i];
/*
 * Compute each element of the matrix.
 * j is the successor of i (in 1,2,3), while k is the successor of j.
 */
  	for (i = 1; i < 4; i++) {
    	if ((j = i + 1) > 3) j = 1;
    	if ((k = j + 1) > 3) k = 1;
		e = 2. * Q[i] * Q[j];
    	f = 2. * Q[k] * Q[0];
		M[j][i] = e - f;
		M[i][j] = e + f;
		M[i][i] = r[i] + r[0] - r[j] - r[k];
		M[0][i] = P[i - 1];
		M[i][0] = 0.;
  	}
	M[0][0] = 1.;
}




