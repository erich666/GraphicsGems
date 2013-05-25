/*
 * len4.c - find length of the 4-vector v having components v = [a b c d]
 * programmed by: Alan Paeth, 17-Oct-1994
 *
 * This Euclidean distance estimator
 *
 *   1) can only overestimate, making it useful for containment heuristics,
 *   2) is exact for vectors [1 0 0 0] (len4 = 1) and [1 1 1 1] (len4 = 2) 
 *      under all permutations, sign changes and (integer) scaling (*),
 *   3) is nearly exact for vectors [1 1 0 0] and [1 1 1 0] (len4=irrational)
 *      subject to the same conditions as in (2) above,
 *   4) offers best 3D fit using the nested form len3(a,b,c) = len4(a,b,c,0)
 *   5) may be easily reprogrammed for floating point operation by rewriting
 *      all occurences of the string "int" with "double" or "float" and by
 *      removing the "a++" rounding fudge immediately before the return.
 *  (*) - exact estimates hold only in the presence of the mod: one line up.
 *
 * Note: worst-case error occurs for vectors of the form [60c 25c 19c 16c]
 *       and is worsened (because of increased a++ rounding imprecision)
 *       with vector [2 1 1 1]. Otherwise (c large), the maximum error in
 *       overestimation is always bounded by 16% [1.1597+ = sqrt(538)/20].
 */
  
#define absv(x)	if (x < 0) x = -x
#define inorder(x,y)	{int t; if ((t = a - b) < 0) {a -= t; b += t; } }

int len4(a, b, c, d)
    {
    absv(a); absv(b);			/* get the absolute values */
    absv(c); absv(d);			/* (component magnitudes) */
    inorder(a, b); inorder(c, d);	/* everyone has a chance to play */
    inorder(a, c); inorder(b, d);	/* (a,d) are big (winner, loser) */
    inorder(b, c);			/* playoff for 2nd and 3rd slots */
    a += (25*b + 19*c + 16*d)/60;	/* compute 4D approximate length */
/*  a +=  (5*b +  4*c +  3*d)/12;	.. only .1% worse; easy to eval  */
    a++;				/* Roundoff -> underestimation   */
    return(a);				/* omit the above one bit jitter */
    }
