/* Program 1. Smoothing and interpolation with first differences. */
/* Contribution to Graphic Gems IV */

/* Paul H. C. Eilers, DCMR Milieudienst Rijnmond, 's-Gravelandseweg 565,
   3119 XT Schiedam, The Netherlands, E-Mail: paul@dcmr.nl */

#define MMAX 100	/* choose the right length for your application */

typedef float vec[MMAX + 1];

void smooth1(vec w, vec y, vec z, float lambda, int m)
/* Smoothing and interpolation with first differences.
   Input:  weights (w), data (y): vector from 1 to m.
   Input:  smoothing parameter (lambda), length (m).
   Output: smoothed vector (z): vector from 1 to m. */
{
  int i, i1;
  vec c, d;
  d[1] = w[1] + lambda;
  c[1] = -lambda / d[1];
  z[1] = w[1] * y[1];
  for (i = 2; i < m; i++) {
    i1 = i - 1;
    d[i]= w[i] + 2 * lambda - c[i1] * c[i1] * d[i1];
    c[i] = -lambda / d[i];
    z[i] = w[i] * y[i] - c[i1] * z[i1];
  }
  d[m] = w[m] + lambda - c[m - 1] * c[m - 1] * d[m - 1];
  z[m] = (w[m] * y[m] - c[m - 1] * z[m - 1]) / d[m];
  for (i = m - 1; 1 <= i; i--) z[i] = z[i] / d[i] - c[i] * z[i + 1];
}
