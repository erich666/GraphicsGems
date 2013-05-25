/*
Forms, Vectors,and Transforms
by Bob Wallis
from "Graphics Gems", Academic Press, 1990
*/

/*----------------------------------------------------------------------
The main program below is set up to solve the Bezier subdivision problem
in "Forms, Vectors, and Transforms".  The subroutines are useful in
solving general problems which require manipulating matrices via exact
integer arithmetic.  The intended application is validating or avoiding
tedious algebraic calculations.  As such, no thought was given to
efficiency.  
----------------------------------------------------------------------*/
#define ABS(x) ((x)>(0)? (x):(-x))
#define N 4			/* size of matrices to deal with */
int     M[N][N] =		/* Bezier weights */
{
      1,   0,   0,   0,
     -3,   3,   0,   0,
      3,  -6,   3,   0,
     -1,   3,  -3,   1,
};
int     T[N][N] =	/* re-parameterization xform for top half */
{
      1,  -1,   1,  -1,
      0,   2,  -4,   6,
      0,   0,   4, -12,
      0,   0,   0,   8
};
main ()
{
    int     i,
            j,
            scale,
            gcd,
            C[N][N],
            S[N][N],
            Madj[N][N],
            Tadj[N][N],
            Mdet,
            Tdet;


    Tdet = adjoint (T, Tadj);   /* inverse without division by */
    Mdet = adjoint (M, Madj);   /* determinant of T and M */
    matmult (Madj, Tadj, C);
    matmult (C, M, S);		/* Madj*Tadj*M -> S */
    scale = gcd = Mdet * Tdet;  /* scale factors of both determinants */
    for (i = 0; i < N; i++)	/* find the greatest common */
    {				/* denominator of S and determinants */
		for (j = 0; j < N; j++)
	    	gcd = Gcd (gcd, S[i][j]);
    }
    scale /= gcd;		/* divide everything by gcd to get */
    for (i = 0; i < N; i++)	/* matrix and scale factor in lowest */
    {				/* integer terms possible */
		for (j = 0; j < N; j++)
	    	S[i][j] /= gcd;
    }
    printf ("scale factor = 1/%d  ", scale);
    print_mat ("M=", M, N);     /* display the results */
    print_mat ("T=", T, N);
    print_mat ("S=", S, N);     /* subdivision matrix */
    exit (0);
}
Gcd (a, b)			/*returns greatest common denominator */
int     a,			/* of (a,b) */
       	b;
{
    int		i,
            	r;
    a = ABS (a);		/* force positive */
    b = ABS (b);
    if (a < b)			/* exchange so that a >= b */
    {
		i = b;
		b = a;
		a = i;
    }
    if 	(b == 0)
		return (a);	/* finished */
    r = a % b;			/* remainder */
    if (r == 0)
		return (b);	/* finished */
    else			/* recursive call */
		return (Gcd (b, r));
}

adjoint (A, Aadj)			/* returns determinant of A */
int     A[N][N],			/* input matrix */
        Aadj[N][N];			/* output = adjoint of A */
{					/* must have N >= 3 */
    int	   i,
            j,
            I[N],			/* arrays of row and column indices */
            J[N],
            Isub[N],			/* sub-arrays of the above */
            Jsub[N],
            cofactor,
            det;
    if (N < 3)
    {
		printf ("must have N >= 3\n");
		exit (1);
    }
    for (i = 0; i < N; i++)
    {					/* lookup tables to select a */
		I[i] = i;		/* particular subset of */
		J[i] = i;		/* rows and columns */
    }
    det = 0;
    for (i = 0; i < N; i++)
    {					/* delete ith row */
		subarray (I, Isub, N, i);
		for (j = 0; j < N; j++)
		{				/* delete jth column */
	    		subarray (J, Jsub, N, j);
	    		cofactor = determinant (A, Isub, Jsub, N - 1,(i + j) & 1);
	    		if (j == 0)		/* use 0th column for det */
				det += cofactor * A[i][0];
			Aadj[j][i] = cofactor;
		}
   	}
    return (det);
}
determinant (A, I, J, n, parity)/* actually gets a sub-determinant */
int     A[N][N],			/* input = entire matrix */
       	I[N],				/* row sub-array we want */
        J[N],				/* col sub-array we want */
        parity,				/* 1-> flip polarity */
        n;				/* # elements in subarrays */

{
    int     i,
            j,
            det,
            j_,
            Jsub[N];
    if (n <= 2)			/* call ourselves till we get down to */
    {				/* a 2x2 matrix */
		det =
	    	(A[I[0]][J[0]] * A[I[1]][J[1]]) -
	    	(A[I[1]][J[0]] * A[I[0]][J[1]]);
		if (parity)
	    det = -det;
		return (det);
    }					/* if (n <= 2) */
    det = 0;				/* n > 2; call recursively */
    i = I[0];				/* strike out 0th row */
    for (j_ = 0; j_ < n; j_++)
    {					/* strike out jth column */
		subarray (J, Jsub, n, j_);
		j = J[j_];		/* I + 1 => struck out 0th row */
		det += A[i][j] * determinant (A, I + 1, Jsub, n - 1, j_ & 1);
    }
    if (parity)
		det = -det;
    return (det);
}
subarray (src, dest, n, k)		/* strike out kth row/column */
int  	*src,				/* 	source array of n indices */
     	*dest,				/* dest array formed by deleting k  */
       	 n,
       	 k;
{
    int     i;
    for (i = 0; i < n; i++, src++)
		if (i != k)			/* skip over k */
	    	*dest++ = *src;
}

matmult (A, B, C)				/* C = A*B */
int     A[N][N],
        B[N][N],
        C[N][N];
{
    int		i,
		j,
		k,
		sum;
    for (i = 0; i < N; i++)
    {
		for (k = 0; k < N; k++)
		{
	    	sum = 0;
	    	for (j = 0; j < N; j++)
				sum += A[i][j] * B[j][k];
	    	C[i][k] = sum;
		}
    }
}
print_mat (string, mat, n)
char   *string;
int     mat[N][N],
         n;
{
    int     i,
             j;
    printf ("%s\n", string);
    for (i = 0; i < n; i++)
    {
		for (j = 0; j < n; j++)
	    printf (" %8ld", mat[i][j]);
	printf ("\n");
    }
}
