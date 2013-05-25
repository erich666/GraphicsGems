/*
Mapping RGB Triples onto Four Bits
by Alan Paeth
from "Graphics Gems", Academic Press, 1990
*/

remap8(R, G, B, R2, G2, B2)
    float R, G, B, *R2, *G2, *B2;
    {
/*
 * remap8 maps floating (R,G,B) triples onto quantized
 * (R2,B2,B2) triples and returns the code (vertex)
 * value/color table entry for the quantization. The
 * points (eight) are the vertices of the cube.
 */
    int code;
    *R2 = *G2 = *B2 = 0.0;
    code = 0;
    if (R >= 0.5) { *R2 = 1.0; code |= 1; }
    if (G >= 0.5) { *G2 = 1.0; code |= 2; }
    if (B >= 0.5) { *B2 = 1.0; code |= 4; }
    return(code);
    }

/*
 * remap14 maps floating (R,G,B) triples onto quantized
 * (R2,B2,B2) triples and returns the code (vertex)
 * value/color table entry for the quantization. The
 * points (fourteen) are the vertices of the cuboctahedron.
 */

float rval[] = { 0.,.5 ,.5 , 1.,.0 , 0., 0.,.5,
                .5 , 1., 1., 1., 0.,.5 ,.5 , 1.};
float gval[] = { 0.,.5 , 0., 0.,.5 , 1., 0.,.5,
                .5 , 1., 0.,.5 , 1., 1.,.5 , 1.};
float bval[] = { 0., 0.,.5 , 0.,.5 , 0., 1.,.5,
                .5 , 0., 1.,.5 , 1.,.5 , 1., 1.};

int remap14(R, G, B,  R2, G2, B2)
    float R, G, B, *R2, *G2, *B2;
    {
    int code = 0;
    if ( R + G + B > 1.5) code |= 8;
    if (-R + G + B > 0.5) code |= 4;
    if ( R - G + B > 0.5) code |= 2;
    if ( R + G - B > 0.5) code |= 1;
    *R2 = rval[code];
    *G2 = gval[code];
    *B2 = bval[code];
    return(code);
    }
