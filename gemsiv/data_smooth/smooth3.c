/* Program 3. Smoothing and interpolation with any difference equation. */
/* Contribution to Graphic Gems IV */

/* Paul H. C. Eilers, DCMR Milieudienst Rijnmond, 's-Gravelandseweg 565,
   3119 XT Schiedam, The Netherlands, E-Mail: paul@dcmr.nl */

#define MMAX 100  /* choose the right length for your application */

typedef float vec[MMAX + 1];
typedef float vecn[6];

void asmooth(vec w, vec y, vec	z, vecn a, float lambda, int m, int n)
/* Smoothing and interpolation with any difference equation of order <=5.
   Input:  weights (w), data (y): vector from 1 to m.
   Input:  smoothing parameter (lambda), length (m).
   Input:  coefficients (a) and order of difference equation (n).
   Output: smoothed vector (z): vector from 1 to m. */
{
  static float b[MMAX + 1][6];
  static int v[MMAX + 1];
  int i, j, j1, j2, k, k1;
  float s;
  for (i = 1; i <= m + n; i++) {
    v[i] = 1; if ((i <= n) || (i > m)) v[i] = 0;
  }
  /*  construct band matrix  */
  for (i = 1; i <= m; i++) {
    j2 = m - i; if (j2 > n) j2 = n;
    for (j = 0; j <= j2; j++) {
      s = 0.0; if (j == 0) s = w[i] / lambda;
      for (k = j; k <= n; k++) s = s + v[i + k] * a[k] * a[k - j];
      b[i][j] = s;
    }
  }
  /*  compute cholesky-decomposition  */
  for (i = 1; i <= m; i++) {
    s = b[i][0];
    j1 = i - n; if (j1 < 1) j1 = 1;
    for (j = j1; j <= i - 1; j++) s = s - b[j][0] * b[j][i - j] * b[j][i - j];
    b[i][0] = (s);
    j2 = i + n; if (j2 > m) j2 = m;
    for (j = i + 1; j <= j2; j++) {
      s = b[i][j - i];
      k1 = j - n; if (k1 < 1) k1 = 1;
      for (k = k1; k <= i - 1; k++) s = s - b[k][0] * b[k][i - k] * b[k][j - k];
      b[i][j - i] = s / b[i][0];
    }
  }
  /*  solve triangular systems	*/
  for (i = 1; i <= m; i++) {
    s = w[i] * y[i] / lambda;
    j1 = i - n; if (j1 < 1) j1 = 1;
    for (j = j1; j <= i - 1; j++) s = s - z[j] * b[j][i - j];
    z[i] = s;
  }
  for (i = m; i >= 1; i--) {
    s = z[i] / b[i][0];
    j2 = i + n; if (j2 > m) j2 = m;
    for (j = i + 1; j <= j2; j++) s = s - z[j] * b[i][j - i];
    z[i] = s;
  }
}

void  pascalrow(vecn a, int n)
/* Construct row n of Pascal's triangle in a */
{
  int i, j;
  for (j = 0; j <= n; j++) a[j] = 0;
  a[0] = 1;
  for (j = 1; j <= n; j++) for (i = n; i >= 1; i--) a[i] = a[i] - a[i - 1];
}

void gensmooth(vec w, vec y, vec z, float lambda, int m, int n)
/* Smoothing and interpolation differences of order <=5.
   Input:  weights (w), data (y): vector from 1 to m.
   Input:  smoothing parameter (lambda), length (m).
   Input:  order of differences (n).
   Output: smoothed vector (z): vector from 1 to m. */
{
  vecn a;
  pascalrow(a, n);
  asmooth(w, y, z, a, lambda, m, n);
}
