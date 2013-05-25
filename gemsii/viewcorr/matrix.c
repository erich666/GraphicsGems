/*
 * matrix.c - Simple routines for general sized matrices.
 *   
 */

#include <stdio.h>
#include <math.h>
#include "matrix.h"


double 
InvertMatrix(mat,actual_size)
Matrix mat;			/* Holds the original and inverse */
int actual_size;	/* Actual size of matrix in use, (high_subscript+1)*/
{
    int i,j,k;
					/* Locations of pivot elements */
    int *pvt_i, *pvt_j;
    double pvt_val;                     /* Value of current pivot element */
    double hold;                        /* Temporary storage */
    double determ;                      /* Determinant */

    determ = 1.0;

    pvt_i = (int *) malloc(actual_size * sizeof(int));
    pvt_j = (int *) malloc(actual_size * sizeof(int));

    for (k = 0; k < actual_size; k++)
    {
        /* Locate k'th pivot element */
        pvt_val = mat[k][k];            /* Initialize for search */
        pvt_i[k] = k;
        pvt_j[k] = k;
        for (i = k; i < actual_size; i++)
          for (j = k; j < actual_size; j++)
            if (fabs(mat[i][j]) > fabs(pvt_val))
            {
                pvt_i[k] = i;
                pvt_j[k] = j;
                pvt_val = mat[i][j];
            }
        /* Product of pivots, gives determinant when finished */
        determ *= pvt_val;
        if (determ == 0.0) {    
         /* Matrix is singular (zero determinant). */
	    free(pvt_i);
	    free(pvt_j);
            return (0.0);              
	}

        /* "Interchange" rows (with sign change stuff) */
        i = pvt_i[k];
        if (i != k)                     /* If rows are different */
          for (j = 0; j < actual_size; j++)
          {
            hold = -mat[k][j];
            mat[k][j] = mat[i][j];
            mat[i][j] = hold;
          }

        /* "Interchange" columns */
        j = pvt_j[k];
        if (j != k)                     /* If columns are different */
          for (i = 0; i < actual_size; i++)
          {
            hold = -mat[i][k];
            mat[i][k] = mat[i][j];
            mat[i][j] = hold;
          }
        /* Divide column by minus pivot value */
        for (i = 0; i < actual_size; i++)
          if (i != k)                   /* Don't touch the pivot entry */
            mat[i][k] /= ( -pvt_val) ;  /* (Tricky C syntax for division) */

        /* Reduce the matrix */
        for (i = 0; i < actual_size; i++)
        {
            hold = mat[i][k];
            for (j = 0; j < actual_size; j++)
              if ( i != k && j != k )   /* Don't touch pivot. */
                mat[i][j] += hold * mat[k][j];
        }

        /* Divide row by pivot */
        for (j = 0; j < actual_size; j++)
          if (j != k)                   /* Don't touch the pivot! */
            mat[k][j] /= pvt_val;

        /* Replace pivot by reciprocal (at last we can touch it). */
        mat[k][k] = 1.0/pvt_val;
    }

    /* That was most of the work, one final pass of row/column interchange */
    /* to finish */
    for (k = actual_size-2; k >= 0; k--)  /* Don't need to work with 1 by 1 */
                                        /* corner */
    {
        i = pvt_j[k];		 /* Rows to swap correspond to pivot COLUMN */
        if (i != k)                     /* If rows are different */
          for(j = 0; j < actual_size; j++)
          {
            hold = mat[k][j];
            mat[k][j] = -mat[i][j];
            mat[i][j] = hold;
          }

        j = pvt_i[k];           /* Columns to swap correspond to pivot ROW */
        if (j != k)                     /* If columns are different */
          for (i = 0; i < actual_size; i++)
          {
            hold = mat[i][k];
            mat[i][k] = -mat[i][j];
            mat[i][j] = hold;
          }
    }

    free(pvt_i);
    free(pvt_j);
    return(determ);
}

Matrix
NewMatrix(cols, rows)
int cols,rows;
{
    int i;
    Matrix newM;
    newM = (double **) malloc(rows * sizeof(double *));
    for(i = 0; i < rows; i++)
	newM[i] = (double *) malloc(cols * sizeof(double));
    return newM;
}

FreeMatrix(mat, rows)
Matrix mat;
int rows;
{
    int i;
    for(i = 0; i < rows; i++)
	free(mat[i]);
    free(mat);
}

TransposeMatrix(inM, outM, cols, rows)
Matrix inM, outM;
int cols,rows;
{
    int tempI, tempJ;
    for(tempI=0; tempI < rows; tempI++)
	for(tempJ=0; tempJ < cols; tempJ++)
	    outM[tempI][tempJ] = inM[tempJ][tempI];    
}

MultMatrix(firstM, secondM, outM, firstrows, cols, secondcols)
Matrix firstM, secondM, outM;
int firstrows, cols, secondcols;
{
    int i,j,k;
    double sum;

    for(i=0; i < secondcols; i++)
	for(j=0; j < firstrows; j++)
	{
	    sum = 0.0;
	    for(k=0; k < cols; k++)
		sum += firstM[j][k] * secondM[k][i];
	    outM[j][i] = sum;
	}
}
