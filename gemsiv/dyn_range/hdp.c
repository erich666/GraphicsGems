/*
** HDP.C : Encoding and Decoding of High Dynamic Range Pixels
*/

#include <stdio.h>
#include <stdlib.h>
#include "hdp.h"

/*
** Encoding and Decoding Look-Up Tables
*/
byte *EncodeLut;
real *DecodeLut;
real  LutScale;

/*
** Construction of the Encoding Look-Up Table
**
** Input :
**    LoVal = Less significant (ie lowest non zero) value of the incoming range
**    HiVal = Most significant (ie highest) value of the incoming range
**    NbVal = Number of elements in the encoding LUT
**
** Output :
**    The function returns 0 if the allocation failed
*/
int init_HDP_encode (real LoVal, real HiVal, int NbVal)
{
    real t, r;
    int	 n;

  EncodeLut = (byte *) malloc (NbVal * sizeof (byte));
  if (! EncodeLut) return 0;

  NbVal--;

/* Scaling factor = ratio between the encoding LUT and the incoming range */
  LutScale = NbVal / HiVal;

/* Bias factor = ratio between the outcoming and the incoming range */
  r = 256.f * LoVal / HiVal;

  for (n = 0; n <= NbVal; n++) {
      t = (float) n / NbVal;
      EncodeLut[n] = (byte)(255.f * t / (t-r*t+r) + 0.5f);
  }
  return (! NULL);
}

/*
** Destruction of the Encoding Look-Up Table
*/
void exit_HDP_encode (void)
{
  free (EncodeLut);
}

/*
** Construction of the Decoding Look-Up Table
**
** Input :
**    LoVal  = Less significant (ie lowest non zero) value of the incoming range
**    HiVal  = Most significant (ie highest) value of the incoming range
**    Bright = Brightness factor in the range (-1,1)
**	       (Bright < 0 : Image is darkened, Bright > 0 : Image is lightened)
**
** Output :
**    The function returns 0 if the allocation failed
*/
int init_HDP_decode (real LoVal, real HiVal, real Bright)
{
    float t, r;
    int n;

  DecodeLut = (real *) malloc (256 * sizeof (real));
  if (! DecodeLut) return 0;

/* Change Bright from (-1,1) into a scaling coefficient (0,infinity) */
  Bright = Bright < 0.f ? Bright+1.f : 1.f / (1.f-Bright);

/* Bias factor = ratio of incoming and outcoming range * brightness factor */
  r = Bright * HiVal / LoVal / 256.f;

  for (n = 0; n <= 256; n++) {
    t = (float) n / 255.f;
    DecodeLut[n] = t / (t-t*r+r) * HiVal;
  }
  return (! NULL);
}

/*
** Destruction of the Decoding Look-Up Table
*/
void exit_HDP_decode (void)
{
  free (DecodeLut);
}
