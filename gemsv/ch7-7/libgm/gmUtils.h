// gmUtils.h - graphics math utility functions
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994

#ifndef GMUTILS_H
#define GMUTILS_H

#include <iostream>
#include <math.h>
#include <stdbool.h>
#include "gmConst.h"

// ONE-ARGUMENT UTILITY FUNCTIONS

inline float gmAbs(float f)
{
  return (f >= 0) ? f : -f;
}

inline float gmCeil(float f)
{
  return (f == int(f)) ? f : (f > 0) ? float(int(f) + 1) : float(int(f));
}

inline float gmCube(float f)
{
  return f * f * f;
}

inline float gmDegrees(float f)
{
  return f * gmRADTODEG;
}

inline float gmFloor(float f)
{
  return (f == int(f)) ? f : (f > 0) ? float(int(f)) : float(int(f) - 1);
}

inline float gmInv(float f)
{
  return 1.f / f;
}

inline bool gmIsZero(float f)
{
  return (gmAbs(f) < gmEPSILON);
}

inline float gmRadians(float f)
{
  return f * gmDEGTORAD;
}

inline float gmRound(float f)
{
  return (f >= 0) ? float(int(f + 0.5)) : float(- int(0.5 - f));
}

inline float gmSign(float f)
{
  return (f < 0) ? -1.f : 1.f;
}

inline float gmSmooth(float f)
{
  return (3.f - 2.f * f) * f * f;
}

inline float gmSqr(float f)
{
  return f * f;
}

inline float gmTrunc(float f)
{
  return float(int(f));
}

inline float gmZSign(float f)
{
  return (f > 0) ? 1.f : (f < 0) ? -1.f : 0.f;
}

// TWO-ARGUMENT UTILITY FUNCTIONS

inline bool gmFuzEQ(float f, float g)
{
  return (f <= g) ? (f >= g - gmEPSILON) : (f <= g + gmEPSILON);
}

inline bool gmFuzGEQ(float f, float g)
{
  return (f >= g - gmEPSILON);
}

inline bool gmFuzLEQ(float f, float g)
{
  return (f <= g + gmEPSILON);
}

inline float gmMax(float f, float g)
{
  return (f > g) ? f : g;
}

inline float gmMin(float f, float g)
{
  return (f < g) ? f : g;
}

inline void gmSwap(float& f, float& g)
{
  float gmTmp = f; f = g; g = gmTmp;
}

inline void gmSwap(int& i, int& j)
{
  int gmTmp = i; i = j; j = gmTmp;
}

// MULTI-ARGUMENT UTILITY FUNCTIONS

inline void gmClamp(float &f, float l, float h)
{
  if(f < l) f = l;
  if(f > h) f = h;
}

inline float gmLerp(float f, float l, float h)
{
  return l + ((h - l) * f );
}

inline float gmMax(float f, float g, float h)
{
  return (f > g) ? gmMax(f, h) : gmMax(g, h);
}

inline float gmMin(float f, float g, float h)
{
  return (f < g) ? gmMin(f, h) : gmMin(g, h);
}

inline float gmSlide(float f, float l, float h)
{
  return (f < 0) ? l : (f > 1) ? h : gmLerp(gmSmooth(f), l, h);
}

#endif


