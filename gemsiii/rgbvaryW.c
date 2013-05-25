/*
    rgbvaryW.c - an improved rgbvary.c, currently runs on Microsoft Windows

    The color reduction filter (rgbvary.c) uses floating point but none 
    of the computations exceed a 16bit integer.  I've rewritten the code to 
    use 16 bit integers (with no loss of precision), updated jitter_init to 
    precompute the scaled values used in the inner loop, and allowed the 
    code to output a C include file with the table values filled in.  The 
    combined savings reduced the filter time on a 1024x768x24bit image from 
    24 seconds to 3 seconds, reduced the table size by 12K (1/3 the 
    original size), and removed 18K of runtime support for floating point 
    emulation, the rand() library, etc.

    From: Ken Sykes <kensy@microsoft.com>
*/

/****************************************************************************
     rgbvaryI.c - a color quantization pre-processor

****************************************************************************/

/* remove the next line after the first run */
#define GENTABLE 1

#ifndef GENTABLE
#include "rgbvtab.h"
#endif

#define JITTER_TABLE_BITS   10
#define JITTER_TABLE_SIZE   (1<<JITTER_TABLE_BITS)
#define JITTER_MASK         (JITTER_TABLE_SIZE-1)

/* jitter macros */
#define jitterx(x,y,s) \
(uranx[((x+(y<<2))+irand[(x+s)&JITTER_MASK])&JITTER_MASK])
#define jittery(x,y,s) \
(urany[((y+(x<<2))+irand[(y+s)&JITTER_MASK])&JITTER_MASK])

#ifdef GENTABLE

/* global varables */
int irand[JITTER_TABLE_SIZE];    /* jitter look-up table */
int uranx[JITTER_TABLE_SIZE];    /* jitter look-up table */
int urany[JITTER_TABLE_SIZE];    /* jitter look-up table */

#include <stdio.h>


void print_tables(void)
{
    FILE *fp;
    int i;

    fp = fopen("rgbvtab.h","w");

    fprintf(fp, "int irand[%d] = {", JITTER_TABLE_SIZE);
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
    {
        if (!(i % 8)) fprintf(fp, "\n\t");
        fprintf(fp, "%d,", irand[i]);
    }
    fprintf(fp, "\n};\n");

    fprintf(fp, "int uranx[%d] = {", JITTER_TABLE_SIZE);
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
    {
        if (!(i % 8)) fprintf(fp, "\n\t");
        fprintf(fp, "%d,", uranx[i]);
    }
    fprintf(fp, "\n};\n");

    fprintf(fp, "int urany[%d] = {", JITTER_TABLE_SIZE);
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
    {
        if (!(i % 8)) fprintf(fp, "\n\t");
        fprintf(fp, "%d,", urany[i]);
    }
    fprintf(fp, "\n};\n");

    fclose(fp);
}


/* function declarations */
void jitter_init();

/*
*
* name        jitter_init - initialize jitter look-up tables
*
*             Adapted from Graphic Gems I (Cyshosz, page 64).
*
* notes       This function should be called once before any call to
*             rgbvary()
*
*/

void jitter_init()
{
    int i,urand;

/* magnitude of noise generated (0,1,2 allowed values) */
#define NOISE_LEVEL 2

    /* determine order to fill table in */
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
        irand[i] = rand() % JITTER_TABLE_SIZE;

    /* fill in the X table */
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
    {
        uranx[i] = urand = rand() % JITTER_TABLE_SIZE;
        uranx[i] = ((urand << 3) + urand) >> JITTER_TABLE_BITS;
    }

    /* fill in the Y table */
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
    {
        urany[i] = urand = rand() % JITTER_TABLE_SIZE;
        urany[i] = ((((urand * NOISE_LEVEL * 2) + JITTER_TABLE_SIZE/2)
                     >> JITTER_TABLE_BITS) - NOISE_LEVEL) << 3;
    }

    print_tables();
}
#endif

BOOL VaryDIB24(HANDLE hdib)
{
    LPBITMAPINFOHEADER lpbi;
    HPBYTE hpbyBits;
    WORD wNextScan;
    int x,y;

    /* Get pointer to bits */
    if (!hdib) return FALSE;
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
    GlobalUnlock(hdib);

    if (lpbi->biBitCount != 24)
    {
       ErrMsg("Jitter only applies to 24bpp images.");
       return FALSE;
    }

    /* Make sure it is not compressed */
    if (lpbi->biCompression != BI_RGB)
    {
       ErrMsg("Jitter only works on uncompressed images.");
       return FALSE;
    }

#ifdef GENTABLE
    jitter_init();
#endif

    hpbyBits = (HPBYTE)DibXY(lpbi,0,0);

    /* DIBs are DWORD-aligned.  Determine amount of padding for each 
scanline */
    wNextScan = (WORD)(((lpbi->biWidth << 1)+lpbi->biWidth) & 3);
    if (wNextScan) wNextScan = (WORD)(4-wNextScan);

    /* Loop through the image & apply the jitter */
    for (y = 0; y < lpbi->biHeight; ++y)
    {
        for (x = 0; x < lpbi->biWidth; ++x)
        {
            int i, p, q;

            for (i = 0; i < 3; ++i)
            {
                int rgb;

                rgb = (int)*hpbyBits;

                if (rgb < 248)
                {
                   /* bump up to next intensity if there is enough jitter */
                   p = rgb & 7;
                   q = jitterx(x, y, i);
                   if (p <= q)
                      rgb += 8;

                   /* add some noise */
                   q = rgb + jittery(x, y, i);

                   /* make sure resulting intensity is within allowable 
range */
                   if (q >= 0 && q <= 255)
                      rgb = q;

                   /* mask off lower 3 bits & store */
                   *hpbyBits = rgb & 0xf8;
                }

                /* update pointer */
                ++hpbyBits;
            }
        }

        /* move pointer to next scan */
        hpbyBits += wNextScan;
    }

    return TRUE;
}
