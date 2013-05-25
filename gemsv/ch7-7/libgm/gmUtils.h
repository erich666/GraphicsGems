// gmUtils.h - graphics math utility functions
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#ifndef GMUTILS_H
#define GMUTILS_H

#include <iostream.h>
#include <math.h>
#include <bool.h>
#include <gmConst.h>

// ONE-ARGUMENT UTILITY FUNCTIONS

inline double gmAbs(double f)
{
  return (f >= 0) ? f : -f;
}

inline double gmCeil(double f)
{
  return (f == int(f)) ? f : (f > 0) ? double(int(f) + 1) : double(int(f));
}

inline double gmCube(double f)
{
  return f * f * f;
}

inline double gmDegrees(double f)
{
  return f * gmRADTODEG;
}

inline double gmFloor(double f)
{
  return (f == int(f)) ? f : (f > 0) ? double(int(f)) : double(int(f) - 1);
}

inline double gmInv(double f)
{
  return 1.0 / f;
}

inline bool gmIsZero(double f)
{
  return (gmAbs(f) < gmEPSILON);
}

inline double gmRadians(double f)
{
  return f * gmDEGTORAD;
}

inline double gmRound(double f)
{
  return (f >= 0) ? double(int(f + 0.5)) : double(- int(0.5 - f));
}

inline double gmSign(double f)
{
  return (f < 0) ? -1.0 : 1.0;
}

inline double gmSmooth(double f)
{
  return (3.0 - 2.0 * f) * f * f;
}

inline double gmSqr(double f)
{
  return f * f;
}

inline double gmTrunc(double f)
{
  return double(int(f));
}

inline double gmZSign(double f)
{
  return (f > 0) ? 1.0 : (f < 0) ? -1.0 : 0.0;
}

// TWO-ARGUMENT UTILITY FUNCTIONS

inline bool gmFuzEQ(double f, double g)
{
  return (f <= g) ? (f >= g - gmEPSILON) : (f <= g + gmEPSILON);
}

inline bool gmFuzGEQ(double f, double g)
{
  return (f >= g - gmEPSILON);
}

inline bool gmFuzLEQ(double f, double g)
{
  return (f <= g + gmEPSILON);
}

inline double gmMax(double f, double g)
{
  return (f > g) ? f : g;
}

inline double gmMin(double f, double g)
{
  return (f < g) ? f : g;
}

inline void gmSwap(double& f, double& g)
{
  double gmTmp = f; f = g; g = gmTmp;
}

inline void gmSwap(int& i, int& j)
{
  int gmTmp = i; i = j; j = gmTmp;
}

// MULTI-ARGUMENT UTILITY FUNCTIONS

inline void gmClamp(double &f, double l, double h)
{
  if(f < l) f = l;
  if(f > h) f = h;
}

inline double gmLerp(double f, double l, double h)
{
  return l + ((h - l) * f );
}

inline double gmMax(double f, double g, double h)
{
  return (f > g) ? gmMax(f, h) : gmMax(g, h);
}

inline double gmMin(double f, double g, double h)
{
  return (f < g) ? gmMin(f, h) : gmMin(g, h);
}

inline double gmSlide(double f, double l, double h)
{
  return (f < 0) ? l : (f > 1) ? h : gmLerp(gmSmooth(f), l, h);
}

#endif


