/*  FILENAME:   LongConst.h  [revised 18 AUG 90]

    AUTHOR:  Kelvin Thompson

    DESCRIPTION:  High-precision constants.  If this file is included
      in the same file as GraphicsGems.h, this file must come *after*
      GraphicsGems.h.  (It's okay to use this file without GraphicsGems.h.)

        The standard _Graphics_Gems_ include file has some constants
      that do not have full double-precision accuracy.  This file
      has the constants to a ridiculously high precision.  See pages
      434-435 of _Graphics_Gems_.  I got the constants from Mathematica.
      
        Also, this file has a constant and macro for finding the base-two
      logarithm of a number.
*/

/* prevent multiple inclusion */
#ifndef __LONGCONST_H__
#define __LONGCONST_H__

/* first get rid of stuff from GraphicsGems.h */
#undef PI
#undef PITIMES2
#undef PIOVER2
#undef E
#undef SQRT2
#undef SQRT3
#undef GOLDEN
#undef DTOR
#undef RTOD

/* re-define basic constants with high precision */
#define PI     3.141592653589793238462643383279502884197169399375105820975
#define E      2.718281828459045235360287471352662497757247093699959574967
#define SQRT2  1.414213562373095048801688724209698078569671875376948073177
#define SQRT3  1.732050807568877293527446341505872366942805253810380628056
#define GOLDEN 1.618033988749894848204586834365638117720309179805762862135

/* re-define derived constants */
#define PITIMES2  (2.0*PI)
#define PIOVER2   (0.5*PI)
#define DTOR      (PI/180.0)
#define RTOD      (180.0/PI)

/* macro and constant for base 2 logarithm */
#define LN2    0.693147180559945309417232121458176568075500134360255254121
#define LOG2(val) (log(val)*(1.0/LN_2))

#endif  /* __LONGCONST_H__ */
