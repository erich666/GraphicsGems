/*
 * This software is copyrighted as noted below.  It may be freely copied,
 * modified, and redistributed, provided that the copyright notice is
 * preserved on all copies.
 *
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is".  Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * You may not include this software in a program or other software product
 * without supplying the source, or without informing the end-user that the
 * source is available for no extra charge.
 *
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of modification,
 * and the reason for such modification.
 */
/*
 * targa.h - type define to Targa format.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Wed Jan 22 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#ifndef MY_TGA

#define MY_TGA

#define TGA_INTERLACED(a)       ( (a) & 0xc0 )
#define TGA_FLIP(a)             ( ((a) & 0x20) ? 0 : 1)
#define TGA_MAPPED              1
#define TGA_RGB                 2
#define TGA_RLE_MAPPED          9
#define TGA_RLE_RGB             10

/*
 * Targa header.
 */
typedef struct {
        byte   num_id;
        byte   cmap_type;
        byte   image_type;
        ushort cmap_orign;
        ushort cmap_length;
        byte   cmap_entry_size;
        ushort xorig, yorig;
        ushort xsize, ysize;
        byte   pixel_size;
        byte   image_descriptor;
} tga_hdr;

#endif       /* MY_TGA */
