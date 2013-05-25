/****************************************************************************
    rgbvary.c - a color quantization pre-processor

    The function jitter_init() must be call one time before any calls
    to rgbvary().

Entry:
    rgb            - an array of RGB components
    noise_level    - the amount of noise desired (0 - 8)
    x              - the x coordinate of the pixel being processed
    y              - the y coordinate of the pixel being processed

Exit:
    rgb            - the resulting array of RGB components
****************************************************************************/

/* defined constants */
#define LARGE_NUMBER        1024
#define JITTER_TABLE_SIZE     1024
#define JITTER_MASK         (JITTER_TABLE_SIZE-1)

/* global variables */
int       irand[JITTER_TABLE_SIZE];    /* jitter look-up table */
double    uranx[JITTER_TABLE_SIZE];    /* jitter look-up table */
double    urany[JITTER_TABLE_SIZE];    /* jitter look-up table */

/* jitter macros */
#define jitterx(x,y,s) (uranx[((x+(y<<2))+irand[(x+s)&JITTER_MASK])&JITTER_MASK])
#define jittery(x,y,s) (urany[((y+(x<<2))+irand[(y+s)&JITTER_MASK])&JITTER_MASK])

/* function declarations */
void jitter_init();
int rand();

void rgbvary(unsigned char rgb[3], int noise_level, int x , int y)
    {
    int i, p, q;

    if (noise_level == 0)
        {
        for (i = 0; i <= 2; ++i)
            rgb[i] &= 0xf8;
        return;
        }

    for (i = 0; i <= 2; ++i)
        {
        if (rgb[i] < 248)
            {
            p = rgb[i] % 8;
            q = (int)(jitterx(x, y, i) * 9.0);
            if (p <= q)
                rgb[i] += 8;

            /* add some noise */
            q = 8 * ((int)((jittery(x, y, i)
                * (double)(2 * noise_level)) + 0.5)
                - noise_level);
            q += (int)rgb[i];

            /* make sure resulting intensity is within allowable range */
            if (q >= 0 && q <= 255)
                rgb[i] = q;

            /* mask off lower 3 bits */
            rgb[i] &= 0xf8;
            }
        }
    }

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
    int i;

    /* initialize look-up tables */
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
        uranx[i] = (double)(rand() % LARGE_NUMBER) / (double)LARGE_NUMBER;
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
        urany[i] = (double)(rand() % LARGE_NUMBER) / (double)LARGE_NUMBER;
    for (i = 0; i < JITTER_TABLE_SIZE; ++i)
        irand[i] = (int)((double)JITTER_TABLE_SIZE
            * ((double)(rand() % LARGE_NUMBER) / (double)LARGE_NUMBER));
    }

