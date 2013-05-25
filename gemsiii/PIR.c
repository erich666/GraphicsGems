/***********************************************************************
The following function displays an image using progressive refinement
via gridded sampling.  It assumes the existence of two functions:

    SetColor (x, y)
        Sets the current color to the color at image location x,y.

    Rectangle (x, y, width, height)
        Draws a rectangular region filled with the current color.  The
        rectangle ranges from x to (x+width-1), and from y to
        (y+height-1).

The parameters given are the subdivision start level (a power of two),
and the X and Y dimensions of the entire image.
***********************************************************************/

void  PIR_Display  (start_level, Xdim, Ydim)
    int  start_level;      /* Starting Subdivision Level */
    int  Xdim, Ydim;       /* Image Dimensions */
{
    auto int size, size2;  /* Current Region Size & size/2 */
    auto int Ix, Iy;       /* Image Space Indices */


    /* Initialization loop:  display the initial coarse image tiling. */

    size = 1 << start_level;

    for (Iy=0;  Iy <= Ydim;  Iy += size)
    {   for (Ix=0;  Ix <= Xdim;  Ix += size)
        {   SetColor  (Ix, Iy);
            Rectangle (Ix, Iy, size, size);
        }
    }

    /* Sampling and Gridding Loop */

    size2 = size / 2;

    while (size > 1)       /* Subdivide down to the pixel level. */
    {
        for (Iy=0;  Iy <= Ydim;  Iy += size)
        {   for (Ix=0;  Ix <= Xdim;  Ix += size)
            {
                /* Draw the three new subpixel regions. */

                SetColor  (Ix, Iy + size2);
                Rectangle (Ix, Iy + size2, size2, size2);

                SetColor  (Ix + size2, Iy + size2);
                Rectangle (Ix + size2, Iy + size2, size2, size2);

                SetColor  (Ix + size2, Iy);
                Rectangle (Ix + size2, Iy, size2, size2);
            }
        }

        /* The new region edge length is half the old edge length. */

        size  = size2;
        size2 = size2 / 2;
    }
}
