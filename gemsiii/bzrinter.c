#include <stdio.h>

/*
 * bzrintrp.c:
 *   Bezier curve interpolation routines.
 *
 */

#define FloatType float

/*****************************************************************************
The following arrays were created by symbolically solving the Bezier
interpolant problem for orders 2 to 9. Each line holds the rational
weights for the given points to interpolate at node values (i/k, k is
the degree) as the i'th element divided by the zero element in line.
*****************************************************************************/
static long IB2[2][3] =
{
    {       1L,       1L,       0L },
    {       1L,       0L,       1L }
};
static long *InterpBlend2[2] =
{
    IB2[0], IB2[1]
};

static long IB3[3][4] =
{
    {       1L,       1L,       0L,       0L },
    {       2L,      -1L,       4L,      -1L },
    {       1L,       0L,       0L,       1L }
};
static long *InterpBlend3[3] =
{
    IB3[0], IB3[1], IB3[2]
};

static long IB4[4][5] =
{
    {       1L,       1L,       0L,       0L,       0L },
    {       6L,      -5L,      18L,      -9L,       2L },
    {       6L,       2L,      -9L,      18L,      -5L },
    {       1L,       0L,       0L,       0L,       1L }
};
static long *InterpBlend4[4] =
{
    IB4[0], IB4[1], IB4[2], IB4[3]
};

static long IB5[5][6] =
{
    {       1L,       1L,       0L,       0L,       0L,       0L },
    {      12L,     -13L,      48L,     -36L,      16L,      -3L },
    {      18L,      13L,     -64L,     120L,     -64L,      13L },
    {      12L,      -3L,      16L,     -36L,      48L,     -13L },
    {       1L,       0L,       0L,       0L,       0L,       1L }
};
static long *InterpBlend5[5] =
{
    IB5[0], IB5[1], IB5[2], IB5[3], IB5[4]
};

static long IB6[6][7] =
{
    {       1L,       1L,       0L,       0L,       0L,       0L,       0L },
    {      60L,     -77L,     300L,    -300L,     200L,     -75L,      12L },
    {     240L,     269L,   -1450L,    2950L,   -2300L,     925L,    -154L },
    {     240L,    -154L,     925L,   -2300L,    2950L,   -1450L,     269L },
    {      60L,      12L,     -75L,     200L,    -300L,     300L,     -77L },
    {       1L,       0L,       0L,       0L,       0L,       0L,       1L }
};
static long *InterpBlend6[6] =
{
    IB6[0], IB6[1], IB6[2], IB6[3], IB6[4], IB6[5]
};

static long IB7[7][8] =
{
    {       1L,       1L,       0L,       0L,       0L,       0L,       0L,
                                                                        0L },
    {      60L,     -87L,     360L,    -450L,     400L,    -225L,      72L,
                                                                      -10L },
    {     150L,     227L,   -1332L,    3015L,   -3080L,    1845L,    -612L,
                                                                       87L },
    {     200L,    -227L,    1512L,   -4185L,    6000L,   -4185L,    1512L,
                                                                     -227L },
    {     150L,      87L,    -612L,    1845L,   -3080L,    3015L,   -1332L,
                                                                      227L },
    {      60L,     -10L,      72L,    -225L,     400L,    -450L,     360L,
                                                                      -87L },
    {       1L,       0L,       0L,       0L,       0L,       0L,       0L,
                                                                        1L }
};
static long *InterpBlend7[7] =
{
    IB7[0], IB7[1], IB7[2], IB7[3], IB7[4], IB7[5], IB7[6]
};

static long IB8[8][9] =
{
    {       1L,       1L,       0L,       0L,       0L,       0L,       0L,
                                                              0L,       0L },
    {     420L,    -669L,    2940L,   -4410L,    4900L,   -3675L,    1764L,
                                                           -490L,      60L },
    {    7560L,   14318L,  -90846L,  228879L, -288610L,  229320L, -113778L,
                                                          32291L,   -4014L },
    {   25200L,  -42881L,  313012L, -960351L, 1568980L,-1409975L,  750876L,
                                                        -223097L,   26836L },
    {   25200L,   28636L, -223097L,  750876L,-1409975L, 1568980L, -960351L,
                                                         313012L,  -42881L },
    {    7560L,   -4014L,   32291L, -113778L,  229320L, -288610L,  228879L,
                                                         -90846L,   14318L },
    {     420L,      60L,    -490L,    1764L,   -3675L,    4900L,   -4410L,
                                                           2940L,    -669L },
    {       1L,       0L,       0L,       0L,       0L,       0L,       0L,
                                                              0L,       1L }
};
static long *InterpBlend8[8] =
{
    IB8[0], IB8[1], IB8[2], IB8[3], IB8[4], IB8[5], IB8[6], IB8[7]
};

static long IB9[9][10] =
{
    {       1L,       1L,       0L,       0L,       0L,       0L,       0L,
                                                    0L,       0L,       0L },
    {     840L,   -1443L,    6720L,  -11760L,   15680L,  -14700L,    9408L,
                                                -3920L,     960L,    -105L },
    {    8820L,   19939L, -135936L,  379008L, -568064L,  561960L, -370944L,
                                               157696L,  -39168L,    4329L },
    {   17640L,  -40953L,  324544L,-1096144L, 2030784L,-2229500L, 1563968L,
                                              -691824L,  176704L,  -19939L },
    {   22050L,   40953L, -349184L, 1298304L,-2731008L, 3503920L,-2731008L,
                                              1298304L, -349184L,   40953L },
    {   17640L,  -19939L,  176704L, -691824L, 1563968L,-2229500L, 2030784L,
                                             -1096144L,  324544L,  -40953L },
    {    8820L,    4329L,  -39168L,  157696L, -370944L,  561960L, -568064L,
                                               379008L, -135936L,   19939L },
    {     840L,    -105L,     960L,   -3920L,    9408L,  -14700L,   15680L,
                                               -11760L,    6720L,   -1443L },
    {       1L,       0L,       0L,       0L,       0L,       0L,       0L,
                                                    0L,       0L,       1L }
};
static long *InterpBlend9[9] =
{
    IB9[0], IB9[1], IB9[2], IB9[3], IB9[4], IB9[5], IB9[6], IB9[7], IB9[8]
};

static long **InterpBlend[] =
{
    NULL,
    NULL,
    InterpBlend2,
    InterpBlend3,
    InterpBlend4,
    InterpBlend5,
    InterpBlend6,
    InterpBlend7,
    InterpBlend8,
    InterpBlend9
};

/*****************************************************************************
Blends one Bezier control point using the Input points, the Bezier curve
should interpolate, and Interp (Inverse(M)) arrays.
*****************************************************************************/
static FloatType BzrCrvInterpOnePoint(Input, Interp, Size)
FloatType *Input;
long *Interp;
int Size;
{
    int i;
    long Denom = *Interp++;
    FloatType R = 0.0;

    for (i = 0; i < Size; i++)
        R += (*Input++ * *Interp++) / Denom;

    return R;
}

/*****************************************************************************
Blends the Input points, the Bezier curve should interpolate, using the
Interp array (Inverse(M)) into the Result Bezier control points array.
Input and Result arrays are of size Size, which is the curve order from 2
(linear) to 9.
*****************************************************************************/
void BzrCrvInterp(Result, Input, Size)
FloatType *Result;
FloatType *Input;
int Size;
{
    int i;
    long **Interp = InterpBlend[Size];

    for (i = 0; i < Size; i++)
        *Result++ = BzrCrvInterpOnePoint(Input, *Interp++, Size);
}
