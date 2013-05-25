/*
 * ANSI C code from the article
 * "Fast Embossing Effects on Raster Image Data"
 * by John Schlag, jfs@kerner.com
 * in "Graphics Gems IV", Academic Press, 1994
 *
 *
 * Emboss - shade 24-bit pixels using a single distant light source.
 * Normals are obtained by differentiating a monochrome 'bump' image.
 * The unary case ('texture' == NULL) uses the shading result as output.
 * The binary case multiples the optional 'texture' image by the shade.
 * Images are in row major order with interleaved color components (rgbrgb...).
 * E.g., component c of pixel x,y of 'dst' is dst[3*(y*xSize + x) + c].
 *
 * To compile a test program on an SGI:
 *	cc -DMAIN emboss.c -lgl_s -lm -o emboss
 * The code for the Emboss routine itself is portable to most machines.
 */

#include <math.h>
#include <sys/types.h>

void
Emboss(
    double azimuth, double elevation,	/* light source direction */
    u_short width45,			/* filter width */
    u_char *bump,			/* monochrome bump image */
    u_char *texture, u_char *dst,	/* texture & output images */
    u_short xSize, u_short ySize	/* image size */
)
{
    long Nx, Ny, Nz, Lx, Ly, Lz, Nz2, NzLz, NdotL;
    register u_char *s1, *s2, *s3, shade, background;
    register u_short x, y;

#define pixelScale 255.9

    /*
     * compute the light vector from the input parameters.
     * normalize the length to pixelScale for fast shading calculation.
     */
    Lx = cos(azimuth) * cos(elevation) * pixelScale;
    Ly = sin(azimuth) * cos(elevation) * pixelScale;
    Lz = sin(elevation) * pixelScale;

    /*
     * constant z component of image surface normal - this depends on the
     * image slope we wish to associate with an angle of 45 degrees, which
     * depends on the width of the filter used to produce the source image.
     */
    Nz = (6 * 255) / width45;
    Nz2 = Nz * Nz;
    NzLz = Nz * Lz;

    /* optimization for vertical normals: L.[0 0 1] */
    background = Lz;

    /* mung pixels, avoiding edge pixels */
    dst += xSize*3;
    if (texture) texture += xSize*3;
    for (y = 1; y < ySize-1; y++, bump += xSize, dst += 3)
    {
	s1 = bump + 1;
	s2 = s1 + xSize;
	s3 = s2 + xSize;
	dst += 3;
	if (texture) texture += 3;
	for (x = 1; x < xSize-1; x++, s1++, s2++, s3++)
	{
	    /*
	     * compute the normal from the bump map. the type of the expression
	     * before the cast is compiler dependent. in some cases the sum is
	     * unsigned, in others it is signed. ergo, cast to signed.
	     */
	    Nx = (int)(s1[-1] + s2[-1] + s3[-1] - s1[1] - s2[1] - s3[1]);
	    Ny = (int)(s3[-1] + s3[0] + s3[1] - s1[-1] - s1[0] - s1[1]);

	    /* shade with distant light source */
	    if ( Nx == 0 && Ny == 0 )
		shade = background;
	    else if ( (NdotL = Nx*Lx + Ny*Ly + NzLz) < 0 )
		shade = 0;
	    else
		shade = NdotL / sqrt(Nx*Nx + Ny*Ny + Nz2);

	    /* do something with the shading result */
	    if ( texture ) {
		*dst++ = (*texture++ * shade) >> 8;
		*dst++ = (*texture++ * shade) >> 8;
		*dst++ = (*texture++ * shade) >> 8;
	    }
	    else {
		*dst++ = shade;
		*dst++ = shade;
		*dst++ = shade;
	    }
	}
	if (texture) texture += 3;
    }
}

#ifdef MAIN

#define TEXTURE 1

main()
{
    #define xSize 200
    #define ySize 200
    u_char bump[ySize][xSize];
    u_char texture[ySize][xSize][3], *txt = 0;
    u_char dst[ySize][xSize][3];
    int i, j;

    /* make a simple input image */
    memset(bump, 0, sizeof(bump));
    for(i = xSize/4; i < 3*xSize/4; i++)
	for(j = ySize/4; j < 3*ySize/4; j++)
	    bump[j][i] = 128;

    #if TEXTURE
    /* make a texture */
    for(i = 0; i < xSize; i++)
	for(j = 0; j < ySize; j++) {
	    texture[j][i][0] = random() >> (31 - 8);
	    texture[j][i][1] = random() >> (31 - 8);
	    texture[j][i][2] = random() >> (31 - 8);
	}
    txt = (u_char *)texture;
    #endif

    /* emboss it */
    memset(dst, 0, sizeof(dst));
    #define dToR(d) ((d)*(M_PI/180))
    Emboss(dToR(30), dToR(30), 3, (u_char *)bump, txt, (u_char *)dst,
	xSize, ySize);

    /* display it (sgi component order is ABGR) */
    prefsize(xSize, ySize);
    winopen("emboss");
    RGBmode();
    gconfig();
    for(i = 0; i < ySize; i++) {
	u_long line[xSize];
	u_char *lp = (u_char *)line;
	for(j = 0; j < xSize; j++) {
	    *lp++ = 255;
	    *lp++ = dst[i][j][2];
	    *lp++ = dst[i][j][1];
	    *lp++ = dst[i][j][0];
	}
	lrectwrite(0, ySize-1 - i, xSize-1, ySize-1 - i, line);
    }
    sleep(30);
}

#endif
