/* ------------------------------------------------------------------------- *\
   WAVE.H :

   This package provides 3 routines for generating rectangular-like,
   triangular-like and sine-like waves including specific features.

   by Christophe Schlick (10 September 1993)

   "Wave Generators for Procedural Techniques in Computer Graphics"
   in Graphics Gems V (edited by A. Paeth), Academic Press
\* ------------------------------------------------------------------------- */

#ifndef _WAVE_
#define _WAVE_

extern double Rwave (register double t, double s, double Fvar, double Avar);
extern double Twave (register double t, double s, double Fvar, double Avar);
extern double Swave (register double t, double s, double Fvar, double Avar);

#endif

/* ------------------------------------------------------------------------- */
