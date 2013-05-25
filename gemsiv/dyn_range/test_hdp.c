/*
** TEST_HDP.C : Simple testing program for the HDP routines
*/

#include "hdp.h"

void main (void)
{
    realcolor RealColor;
    bytecolor ByteColor;
    real LoVal, HiVal, Bright;
    int  Index, NbTst, NbVal;

/* Dynamic range of 4000 (try also larger or smaller values) */
  LoVal = 0.25;
  HiVal = 1000.0;

/* Memory for encoding LUT = 8 Kbytes */
  NbVal = 8192;

/* Construction of the encoding LUT */
  init_HDP_encode (LoVal, HiVal, NbVal);

/* No brightness modification (try also negative and positive values) */
  Bright = 0.0;

/* Construction of the decoding LUT */
  init_HDP_decode (LoVal, HiVal, Bright);

/*
   Test NbTst sample values (ranging from 0 to HiVal)
   before and after a coding/decoding sequence
*/
  NbTst = HiVal / LoVal;
  for (Index = 0; Index <= NbTst; Index++) {
    RealColor[0] = RealColor[1] = RealColor[2] = Index * HiVal / NbTst;
    printf ("Before = %.2f\t", RealColor[0]);
    HDP_ENCODE (RealColor, ByteColor);
    printf ("Coded value = %d\t", ByteColor[0]);
    HDP_DECODE (ByteColor, RealColor);
    printf ("After = %.2f\n", RealColor[0]);
  }

/* Destruction of the look-up tables */
    exit_HDP_encode ();
    exit_HDP_decode ();
}
