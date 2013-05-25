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
 * lugfnts.c - define all prototypes of LUG subroutines.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Fri 9 Jul 1993
 * Copyright (c) 1993, Raul Rivero
 *
 */

#ifndef MY_DEFINE_PROTOTYPES

#define MY_DEFINE_PROTOTYPES

/*
 * We need some of the lug header definitions.
 */
#include "lug.h"

/*
 * Include all what functions needs.
 */
#include "targa.h"
#include "alias.h"
#include "rla.h"

/************************************************
 *
 * Interface (read/write) definitions.
 *
 */

/* cnv.c */

extern int
read_lug_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_lug_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern ifunptr
get_readlug_function(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern ifunptr
get_writelug_function(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern int
get_depth_writelug_function(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern int
get_index_function(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern int
get_real_index_function(
#ifdef USE_PROTOTYPES
        char *
#endif
);


/* gif.c */

extern int
read_gif_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_gif(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
uncode_gif(
#ifdef USE_PROTOTYPES
        FILE *,
        int,
        int,
        bitmap_hdr *
#endif
);

extern int
read_code(
#ifdef USE_PROTOTYPES
        byte *,
        int,
        int *,
        int
#endif
);

extern int
push_gif(
#ifdef USE_PROTOTYPES
        byte *,
        int
#endif
);

extern int
read_gif_hdr(
#ifdef USE_PROTOTYPES
        FILE *
#endif
);

extern byte
*read_gif_screen_hdr(
#ifdef USE_PROTOTYPES
        FILE *,
        int *,
        int *
#endif
);

extern byte
*read_gif_image_hdr(
#ifdef USE_PROTOTYPES
        FILE *,
        int *,
        int *,
        int *,
        int *
#endif
);

extern byte
*unblock(
#ifdef USE_PROTOTYPES
        FILE *
#endif
);

extern int
write_gif_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_gif(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
write_gif_hdr(
#ifdef USE_PROTOTYPES
        FILE *
#endif
);

extern int
write_gif_screen_hdr(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
write_gif_cmap(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
write_gif_image_hdr(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);


/* heightfield.c */

extern int
write_hf_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_hf(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *,
        double
#endif
);


/* jpeg.c */

extern int
write_jpeg_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_jpeg(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
write_jpeg_opt(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *,
        int
#endif
);

extern int
read_jpeg_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_jpeg(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);


/* pbm.c */

extern int
read_pbm_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_pbm(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
write_pbm_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_pbm(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
skip_pbm(
#ifdef USE_PROTOTYPES
        FILE *
#endif
);


/* pcx.c */

extern int
read_pcx_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_pcx(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
decodePCX(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        int
#endif
);

extern int
write_pcx_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_pcx(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
write_pcx_header(
#ifdef USE_PROTOTYPES
        FILE *,
        int,
        int
#endif
);

extern int
write_pcx_cmap(
#ifdef USE_PROTOTYPES
        FILE *,
        int ,
        byte *
#endif
);

extern int
encodePCX(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        int
#endif
);


/* pix.c */

extern int
read_alias_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_alias(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
read_alias_header(
#ifdef USE_PROTOTYPES
        FILE *,
        alias_hdr *
#endif
);

extern int
read_line_alias24(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        byte *,
        byte *,
        register int
#endif
);

extern int
read_line_alias(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        register int
#endif
);

extern int
create_alias_cmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *
#endif
);

extern int
uncode_alias24(
#ifdef USE_PROTOTYPES
        byte *,
        byte *,
        byte *,
        byte *,
        byte *
#endif
);

extern int
uncode_alias(
#ifdef USE_PROTOTYPES
        byte *,
        byte *,
        byte *
#endif
);

extern int
write_alias_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_alias(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
code_alias24(
#ifdef USE_PROTOTYPES
        byte *,
        byte *,
        byte *,
        int ,
        FILE *
#endif
);


/* ps.c */

extern int
write_ps_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_ps_file_dimensions(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *,
        double,
        double
#endif
);

extern int
ImagenGrisesPs(
#ifdef USE_PROTOTYPES
        int ,
        int ,
        byte *,
        double ,
        double ,
        char *
#endif
);


/* raw.c */

extern int
read24bitmap(
#ifdef USE_PROTOTYPES
        int ,
        int ,
        FILE *,
        FILE *,
        FILE *,
        bitmap_hdr *
#endif
);

extern int
read_raw_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_8bitmap_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *,
        int ,
        int
#endif
);

extern int
read8bitmap(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *,
        int,
        int
#endif
);

extern int
write24bitmap(
#ifdef USE_PROTOTYPES
        FILE *,
        FILE *,
        FILE *,
        bitmap_hdr *
#endif
);

extern int
write_raw_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_8bitmap_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write8bitmap(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);


/* rgb.c */

extern int
read_rgb_i_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_rgb_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *,
        int ,
        int
#endif
);

extern int
read_rgb(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *,
        int,
        int
#endif
);

extern int
write_rgb_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_rgb(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);


/* rla.c */

extern int
read_rla_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_rla(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
decodeRLA(
#ifdef USE_PROTOTYPES
        byte *,
        byte *,
        int
#endif
);

extern int
write_rla_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_rla(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
encodeRLA(
#ifdef USE_PROTOTYPES
        byte *,
        byte *,
        int
#endif
);


/* rle.c */

extern int
read_rle_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_rle(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
write_rle_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_rle(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);


/* sgi.c */

extern int
read_sgi_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_sgi_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);


/* tga.c */

extern int
read_tga_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
read_tga(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *
#endif
);

extern int
read_tga24(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        byte *,
        byte *,
        tga_hdr *
#endif
);

extern int
read_tga_to24(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        byte *,
        byte *,
        tga_hdr *,
        byte *
#endif
);

extern int
read_tga8(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        tga_hdr *
#endif
);

extern int
read_tga_header(
#ifdef USE_PROTOTYPES
        FILE *,
        tga_hdr *
#endif
);

extern int
read_tga_data(
#ifdef USE_PROTOTYPES
        byte *,
        int ,
        FILE *
#endif
);

extern int
write_tga_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_rle_tga_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_tga(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *,
        int
#endif
);

extern int
write_tga_header(
#ifdef USE_PROTOTYPES
        FILE *,
        bitmap_hdr *, 
        int
#endif
);

extern int
write_tga_line24(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        byte *,
        byte *,
        int
#endif
);

extern int
write_tga_rle_line24(
#ifdef USE_PROTOTYPES
        FILE *,
        byte *,
        byte *,
        byte *,
        int
#endif
);


/* tiff.c */

extern int
read_tiff_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);

extern int
write_tiff_file(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);



/************************************************
 *
 * Internal library functions.
 *
 */

/* bitmap.c */

extern int
allocatebitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        int,
        int,
        int,
        int
#endif
);

extern int
freebitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *
#endif
);

extern int
copy_bitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);


/* error.c */

extern char *
lugerrmsg(
#ifdef USE_PROTOTYPES
        int
#endif
);

extern int
Error(
#ifdef USE_PROTOTYPES
        int
#endif
);

/* general.c */

extern int
no_bits(
#ifdef USE_PROTOTYPES
        int
#endif
);

extern char *
basename(
#ifdef USE_PROTOTYPES
        char *,
        char *
#endif
);

extern short *
bytetoshort(
#ifdef USE_PROTOTYPES
        byte *,
        short *,
        int
#endif
);

extern byte *
shorttobyte(
#ifdef USE_PROTOTYPES
        short *,
        byte *,
        int
#endif
);

extern byte *
floattobyte(
#ifdef USE_PROTOTYPES
        float *,
        byte *,
        int
#endif
);

extern float *
bytetofloat(
#ifdef USE_PROTOTYPES
        byte *,
        float *,
        int
#endif
);

extern int
Atoi(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern double
Atod(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern int
isnumber(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern int
Uncompress(
#ifdef USE_PROTOTYPES
        char *,
        char *
#endif
);

extern int
Compress(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern int
compute_levels(
#ifdef USE_PROTOTYPES
        int
#endif
);


/* in_out.c */

extern FILE *
Fopen(
#ifdef USE_PROTOTYPES
        char *,
        char *
#endif
);

extern int
exist_file(
#ifdef USE_PROTOTYPES
        char *
#endif
);

extern int
Fclose(
#ifdef USE_PROTOTYPES
        FILE *
#endif
);

extern char *
read_file(
#ifdef USE_PROTOTYPES
        FILE *,
        int *
#endif
);

long
filelen(
#ifdef USE_PROTOTYPES
        FILE *
#endif
);

extern int
getshortMSBF(
#ifdef USE_PROTOTYPES
        FILE *
#endif
);

extern int
getshortLSBF(
#ifdef USE_PROTOTYPES
        FILE *
#endif
);

extern int
putshortMSBF(
#ifdef USE_PROTOTYPES
        int,
        FILE *
#endif
);

extern int
putshortLSBF(
#ifdef USE_PROTOTYPES
        int,
        FILE *
#endif
);


/* memory.c */

extern char *
Malloc(
#ifdef USE_PROTOTYPES
        int
#endif
);

extern int
Free(
#ifdef USE_PROTOTYPES
        void *
#endif
);



/************************************************
 *
 * Functions to show images.
 *
 */

/* viewers */

extern int
show_bitmap(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *,
        int
#endif
);

extern int
write_vfr(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        int,
        char *
#endif
);

extern int
show_bitmap_x11(
#ifdef USE_PROTOTYPES
        char *,
        bitmap_hdr *
#endif
);



/************************************************
 *
 * Digital images editing functions.
 *
 */

/* blur.c */

extern int
blur_bitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

byte *
blur(
#ifdef USE_PROTOTYPES
        byte *,
        int,
        int
#endif
);

/* convolve.c */

extern int
convolve_bitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
	bitmap_hdr *,
	double *
#endif
);

byte *
convolve(
#ifdef USE_PROTOTYPES
        byte *,
        int,
	int,
	double *
#endif
);

/* cut.c */

extern int
cut_bitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        int,
        int,
        int,
        int
#endif
);

/* change.c */

extern int
changecolor(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        int,
        int,
        int,
        int,
        int,
        int,
        int
#endif
);


/* chroma.c */

extern int
chroma_bitmaps(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

extern int
chroma(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);


/* dither.c */

extern int
dither_image(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        int,
        double
#endif
);

void
create_dithermap(
#ifdef USE_PROTOTYPES
        int,
        double,
        byte [][3],
        int [256],
        int [256],
        int [16][16]
#endif
);

void
create_square(
#ifdef USE_PROTOTYPES
        double,
        int [256],
        int [256],
        int [16][16]
#endif
);

/* flip.c */

extern int
flip_image(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

byte *
flip(
#ifdef USE_PROTOTYPES
        byte *,
        int,
        int
#endif
);


/* gamma.c */

extern int
gamma_correction(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        double
#endif
);


/* histoequalization.c */


extern int
histogram_equalization(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);


/* hsl.c */

extern int
hsl_to_rgb_buffer(
#ifdef USE_PROTOTYPES
        double *,
        double *,
        double *,
        byte *,
        byte *,
        byte *,
        int
#endif
);

extern int
rgb_to_hsl_buffer(
#ifdef USE_PROTOTYPES
        byte *,
        byte *,
        byte *,
        double *,
        double *,
        double *,
        int
#endif
);

extern int
RGB_to_HSL(
#ifdef USE_PROTOTYPES
        byte,
        byte,
        byte,
        double *,
        double *,
        double *
#endif
);

extern int
HSL_to_RGB(
#ifdef USE_PROTOTYPES
        double,
        double,
        double,
        byte *,
        byte *,
        byte *
#endif
);


/* mask.c */

extern int
chroma_mask(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

extern int
chroma_shadow_mask(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

extern int
mask_change_color(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *,
        int,
        int,
        int
#endif
);

extern int
mask_change_to_bw(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

extern int
mask_darken_color(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

extern int
fade_mask(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *,
        bitmap_hdr *
#endif
);


/* fade.c*/

extern int
medianfilter(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

byte *
filternoise(
#ifdef USE_PROTOTYPES
        byte *,
        int,
        int
#endif
);

extern int
med3x3(
#ifdef USE_PROTOTYPES
        byte *,
        byte *,
        byte *
#endif
);


/* mirror.c */

extern int
mirror_image(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

byte *
mirror(
#ifdef USE_PROTOTYPES
        byte *,
        int,
        int
#endif
);


/* paste.c */

extern int
paste(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        int,
        int
#endif
);

extern int
center_image(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);


/* quantize.c */

extern int
quantize(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        int
#endif
);


/* rotate.c */

extern int
rotate_bitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        double
#endif
);

extern int
rotate90_image(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

byte *
rotate90(
#ifdef USE_PROTOTYPES
        byte *,
        int,
        int
#endif
);


/* sharpen.c */

extern int
sharpen_bitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);


/* slowzoom.c */

extern int
slow_adjust_bitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        int,
        int
#endif
);


/* solid.c */

extern int
create_solid_image(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        int,
        int,
        byte,
        byte,
        byte
#endif
);


/* to24 */

extern int
to24(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);


/* tobw.c */

extern int
tobw(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

extern int
to_raw_bw(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

byte *
create_bw_pallete();

byte *
cmap_to_bw(
#ifdef USE_PROTOTYPES
        bitmap_hdr *image
#endif
);

extern int
isagrayscaled(
#ifdef USE_PROTOTYPES
        bitmap_hdr *
#endif
);

extern int
rgbtobw(
#ifdef USE_PROTOTYPES
        byte *,
        byte *,
        byte *,
        byte *,
        int
#endif
);


/* tohalftone.c */

extern int
tohalftone(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

extern int
bw_to_halftone(
#ifdef USE_PROTOTYPES
        int,
        int,
        byte
#endif
);

byte *
create_halftone_pallete();

/* toinverse.c */

extern int
toinverse(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *
#endif
);

byte *
cmap_to_inverse(
#ifdef USE_PROTOTYPES
        bitmap_hdr *
#endif
);

extern int
inverse(
#ifdef USE_PROTOTYPES
        byte *,
        int
#endif
);


/* zoom.c */

extern int
adjust_bitmap(
#ifdef USE_PROTOTYPES
        bitmap_hdr *,
        bitmap_hdr *,
        int,
        int,
        int
#endif
);

byte *
zoom(
#ifdef USE_PROTOTYPES
        int,
        int,
        int,
        int,
        register byte *,
        int
#endif
);



#endif  /* MY_DEFINE_PROTOTYPES */
