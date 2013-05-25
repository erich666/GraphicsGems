#define LEFT            1
#define RIGHT           2
#define BOTTOM          4
#define TOP             8

#define SWAP(x, y)      { int _t = x; x = y; y = _t; }

#define OUTCODE(x, y, outcode, type)                                     \
{                                                                        \
  if (x < xl) outcode = LEFT, type = 1;                                  \
  else if (x > xr) outcode = RIGHT, type = 1;                            \
  else outcode = type = 0;                                               \
  if (y < yb) outcode |= BOTTOM, type++;                                 \
  else if (y > yt) outcode |= TOP, type++;                               \
}

#define CLIP(a1, a2, b1, da, da2, db2, as, bs, sa, sb,                   \
             amin, AMIN, amax, AMAX, bmin, BMIN, bmax, BMAX)             \
{                                                                        \
  if (out1) {                                                            \
    if (out1 & AMIN) { ca = db2 * (amin - a1); as = amin; }              \
    else if (out1 & AMAX) { ca = db2 * (a1 - amax); as = amax; }         \
    if (out1 & BMIN) { cb = da2 * (bmin - b1); bs = bmin; }              \
    else if (out1 & BMAX) { cb = da2 * (b1 - bmax); bs = bmax; }         \
    if (type1 == 2)                                                      \
      out1 &= (ca + da < cb + !dir) ? ~(AMIN | AMAX) : ~(BMAX | BMIN);   \
    if (out1 & (AMIN | AMAX)) {                                          \
      cb = (ca + da - !dir) / da2;                                       \
      if (sb >= 0) { if ((bs = b1 + cb) > bmax) return; }                \
      else { if ((bs = b1 - cb) < bmin) return; }                        \
      r += ca - da2 * cb;                                                \
    }                                                                    \
    else {                                                               \
      ca = (cb - da + db2 - dir) / db2;                                  \
      if (sa >= 0) { if ((as = a1 + ca) > amax) return; }                \
      else { if ((as = a1 - ca) < amin) return; }                        \
      r += db2 * ca - cb;                                                \
    }                                                                    \
  }                                                                      \
  else { as = a1; bs = b1; }                                             \
  alt = 0;                                                               \
  if (out2) {                                                            \
    if (type2 == 2) {                                                    \
      ca = db2 * ((out2 & AMIN) ? a1 - amin : amax - a1);                \
      cb = da2 * ((out2 & BMIN) ? b1 - bmin : bmax - b1);                \
      out2 &= (cb + da < ca + dir) ? ~(AMIN | AMAX) : ~(BMIN | BMAX);    \
    }                                                                    \
    if (out2 & (AMIN | AMAX)) n = (out2 & AMIN) ? as - amin : amax - as; \
    else { n = (out2 & BMIN) ? bs - bmin : bmax - bs; alt = 1; }         \
  }                                                                      \
  else n = (a2 >= as) ? a2 - as : as - a2;                               \
}


void clip(int dir, int x1, int y1, int x2, int y2,
          int xl, int yb, int xr, int yt)
/*
 *      If dir = 0, round towards (x1, y1)
 *      If dir = 1, round towards (x2, y2)
 */
{
  int adx, ady, adx2, ady2, sx, sy;
  int out1, out2, type1, type2;
  int ca, cb, r, diff, xs, ys, n, alt;

  OUTCODE(x1, y1, out1, type1);
  OUTCODE(x2, y2, out2, type2);
  if (out1 & out2) return;
  if ((type1 != 0 && type2 == 0) || (type1 == 2 && type2 == 1)){
    SWAP(out1, out2);
    SWAP(type1, type2);
    SWAP(x1, x2);
    SWAP(y1, y2);
    dir ^= 1;
  }
  xs = x1;
  ys = y1;
  sx = 1;
  adx = x2 - x1;
  if (adx < 0) { adx = -adx; sx = -1; }
  sy = 1;
  ady = y2 - y1;
  if (ady < 0) { ady = -ady; sy = -1; }
  adx2 = adx + adx;
  ady2 = ady + ady;
  if (adx >= ady) {
/*
 *      line is semi-horizontal
 */
    r = ady2 - adx - !dir;
    CLIP(x1, x2, y1, adx, adx2, ady2, xs, ys, sx, sy,
         xl, LEFT, xr, RIGHT, yb, BOTTOM, yt, TOP);
    diff = ady2 - adx2;
    if (alt) {
      for (;; xs += sx) {       /* alternate Bresenham */
        plot(xs, ys);
        if (r >= 0 ) {
          if (--n < 0) break;
          r += diff;
          ys += sy;
        }
        else r += ady2;
      }
    }
    else{
      for (;; xs += sx) {       /* standard Bresenham */
        plot(xs, ys);
        if (--n < 0) break;
        if (r >= 0 ) { r += diff; ys += sy; }
        else r += ady2;
      }
    }
  }
  else {
/*
 *      line is semi-vertical
 */
    r = adx2 - ady - !dir;
    CLIP(y1, y2, x1, ady, ady2, adx2, ys, xs, sy, sx,
         yb, BOTTOM, yt, TOP, xl, LEFT, xr, RIGHT);
    diff = adx2 - ady2;
    if (alt) {
      for (;; ys += sy) {       /* alternate Bresenham */
        plot(xs, ys);
        if (r >= 0 ) {
          if (--n < 0) break;
          r += diff;
          xs += sx;
        }
        else r += adx2;
      }
    }
    else {
      for (;; ys += sy) {       /* standard Bresenham */
        plot(xs, ys);
        if (--n < 0) break;
        if (r >= 0 ) { r += diff; xs += sx; }
        else r += adx2;
      }
    }
  }
}
