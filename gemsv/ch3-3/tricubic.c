typedef struct
{
  float           x, y, z;
}               Point;

/*
 * TriCubic - tri-cubic interpolation at point, p.
 *   inputs:
 *     p - the interpolation point.
 *     volume - a pointer to the float volume data, stored in x,
 *              y, then z order (x index increasing fastest).
 *     xDim, yDim, zDim - dimensions of the array of volume data.
 *   returns:
 *     the interpolated value at p.
 *   note:
 *     NO range checking is done in this function.
 */


float           TriCubic (Point p, float *volume, int xDim, int yDim, int zDim)
{
  int             x, y, z;
  register int    i, j, k;
  float           dx, dy, dz;
  register float *pv;
  float           u[4], v[4], w[4];
  float           r[4], q[4];
  float           vox = 0;
  int             xyDim;

  xyDim = xDim * yDim;

  x = (int) p.x, y = (int) p.y, z = (int) p.z;
  if (x < 0 || x >= xDim || y < 0 || y >= yDim || z < 0 || z >= zDim)
    return (0);

  dx = p.x - (float) x, dy = p.y - (float) y, dz = p.z - (float) z;
  pv = volume + (x - 1) + (y - 1) * xDim + (z - 1) * xyDim;

# define CUBE(x)   ((x) * (x) * (x))
# define SQR(x)    ((x) * (x))
/*
 #define DOUBLE(x) ((x) + (x))
 #define HALF(x)   ...
 *
 * may also be used to reduce the number of floating point
 * multiplications. The IEEE standard allows for DOUBLE/HALF
 * operations.
 */

  /* factors for Catmull-Rom interpolation */

  u[0] = -0.5 * CUBE (dx) + SQR (dx) - 0.5 * dx;
  u[1] = 1.5 * CUBE (dx) - 2.5 * SQR (dx) + 1;
  u[2] = -1.5 * CUBE (dx) + 2 * SQR (dx) + 0.5 * dx;
  u[3] = 0.5 * CUBE (dx) - 0.5 * SQR (dx);

  v[0] = -0.5 * CUBE (dy) + SQR (dy) - 0.5 * dy;
  v[1] = 1.5 * CUBE (dy) - 2.5 * SQR (dy) + 1;
  v[2] = -1.5 * CUBE (dy) + 2 * SQR (dy) + 0.5 * dy;
  v[3] = 0.5 * CUBE (dy) - 0.5 * SQR (dy);

  w[0] = -0.5 * CUBE (dz) + SQR (dz) - 0.5 * dz;
  w[1] = 1.5 * CUBE (dz) - 2.5 * SQR (dz) + 1;
  w[2] = -1.5 * CUBE (dz) + 2 * SQR (dz) + 0.5 * dz;
  w[3] = 0.5 * CUBE (dz) - 0.5 * SQR (dz);

  for (k = 0; k < 4; k++)
  {
    q[k] = 0;
    for (j = 0; j < 4; j++)
    {
      r[j] = 0;
      for (i = 0; i < 4; i++)
      {
        r[j] += u[i] * *pv;
        pv++;
      }
      q[k] += v[j] * r[j];
      pv += xDim - 4;
    }
    vox += w[k] * q[k];
    pv += xyDim - 4 * xDim;
  }
  return (vox < 0 ? 0.0 : vox);
}
