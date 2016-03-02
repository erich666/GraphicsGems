/* matrix.h
 *    The type and externs for matrix routines.
 */

typedef double ** Matrix;

Matrix NewMatrix(int cols, int rows);
void FreeMatrix(Matrix mat, int rows);
double InvertMatrix(Matrix mat, int actual_size);
void TransposeMatrix(Matrix inM, Matrix outM, int cols, int rows);
void MultMatrix(Matrix firstM, Matrix secondM, Matrix outM, int firstrows, int cols, int secondcols);
