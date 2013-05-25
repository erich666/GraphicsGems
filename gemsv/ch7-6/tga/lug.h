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
 * lug.h - main LUG include.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Fri Dec 27 1991
 * Copyright (c) 1991, Raul Rivero
 *
 */

#ifndef MY_LUG

#define MY_LUG

/*
 * Load the LUG config header ( CHECK THAT FILE !!! ).
 */
#include "lugconf.h"

/*
 * System includes.
 */
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#ifndef MSDOS
#       include <sys/wait.h>
#endif
#include <sys/stat.h>

#ifdef USE_STDLIB_H
#       include <stdlib.h>
#else
#       ifdef VOID_STAR
                extern void *malloc();
#       else
                extern char *malloc();
#       endif /* VOID_STAR */
        extern void free();
#endif /* USE_STDLIB_H */

#ifdef USE_STRINGS
#       include <strings.h>
#else
#       include <string.h>
#endif  /* USE_STRINGS */

#ifndef BSD
#       define bzero(s, n)             memset((s), 0, (n))
#       define bcopy(f, t, c)          memcpy((t), (f), (c))
#       define index(s, c)             strchr((s), (c))
#endif /* BSD */

/*
 * New data types.
 */
#define byte                    unsigned char
#define ushort                  unsigned short
#define word                    unsigned int
#define ulong                   unsigned long

/*
 * Flags.
 */
/*#define FALSE                   0
#define TRUE                    1 */
#define CHROMABORDER            20
#define CHROMAMASK              20
#define LUGUSED                 12345

/*
 * Supported file formats.
 */
#define GIF_FORMAT              00
#define HF_FORMAT               01
#define SUN_FORMAT              02
#define PBM_FORMAT              03
#define PGM_FORMAT              04
#define PPM_FORMAT              05
#define PCX_FORMAT              06
#define PIX_FORMAT              07
#define RAW_FORMAT              08
#define RGB_FORMAT              09
#define RLA_FORMAT              10
#define RLB_FORMAT              11
#define RLE_FORMAT              12
#define SGI_FORMAT              13
#define TGA_FORMAT              14
#define TIFF_FORMAT             15
/*
 * And the default file format is ...
 */
#define DEFAULT_FORMAT          PIX_FORMAT

/*
 * Conversion support defines.
 */
#define ONLY_RAW_PLANES         -8
#define ONLY_FLOAT_PLANES       -1
#define ALL_PLANES               0
#define ONLY_BINARY_PLANES       1
#define ONLY_8_PLANES            8
#define ONLY_24_PLANES          24

/*
 * Returned by exist_file.
 *
 * ...
 * FILE_IS_GZIP/COMPRESS  --> the name exist but is compressed
 * FILE_WITH_GZIP/COMPRESS -> the file doesn't exist, but if you
 *                            add a .gz/.Z suffix then ... voila!.
 */
#define FILE_NO_EXIST            0
#define FILE_EXIST               1
#define FILE_IS_GZIP             2
#define FILE_IS_COMPRESS         3
#define FILE_WITH_GZIP           4
#define FILE_WITH_COMPRESS       5

/*
 * Temporary directory.
 */
#ifndef MSDOS
#       define TMPDIR                  "/usr/tmp"
#else
#       define TMPDIR                  getenv("TEMP")
#endif

/*
 * Some define functions.
 */
#define MSB(a)                  ((byte) (((short)(a)) >> 8))
#define LSB(a)                  ((byte) (((short)(a)) & 0x00FF))
#define LUGABS(a)                  ((a) < 0 ? -(a) : (a)) 
#define FLOOR(a)                ((a) > 0 ? (int)(a) : -(int)(-a))
#define CEILING(a)              ((a) == (int)(a) ? (a) : (a) > 0 ? \
                                 1 + (int)(a) : -(1 + (int)(-a)))
#define ROUND(a)                ((a) > 0 ? (int)((a)+0.5) : -(int)(0.5-(a)))
#define SQR(a)                  ((a) * (a))
#define LUGMAX(a,b)                ((a) > (b) ? (a) : (b))
#define LUGMIN(a,b)                ((a) > (b) ? (b) : (a)) 
/* #define SWAP(a,b)               { a ^= b; b ^= a; a ^= b; } */
#define CORRECT(a)              ((a) < 0  ?  0  : (a) > 255 ? 255 : (a) )
#define CLAMP(v,l,h)            ((v) <(l) ? (l) : (v) > (h) ? (h) : v)
#define RGB_to_BW(r,g,b)        ((int) (30*(r) + 59*(g) + 11*(b)) / 100)

/*
 * I/O define funcitions.
 */
#define Fread(p, s, n, f)       if ( fread(p, s, n, f) != n ) error(3)
#define Fwrite(p, s, n, f)      if ( fwrite(p, s, n, f) != n ) error(4)
#define VPRINTF                 if (LUGverbose) fprintf
#define VFLUSH                  if (LUGverbose) fflush
#define setverbose(f)           LUGverbose = (f)
#define getverbose()            LUGverbose
#define setcmdname(n)           MY_NAME = (n)
#define getcmdname()            MY_NAME

/*
 * If you define ERROR_CALLS then the library functions
 * seems system calls ( and return -1 or NULL ), but this
 * needs that you check this.
 */
#ifndef ERROR_CALLS
#       define error(code)              Error(code)
#       define errornull(code)          Error(code)
#else
#       define error(code)              { Error(code); return -1; }
#       define errornull(code)          { Error(code); return NULL; }
#endif  /* ERROR_CALLS */

/*
 * Pointer to functions data type.
 */
typedef int (* ifunptr)();

/*
 * We'll use it when need a pointer to cmap.
 */
typedef byte color_map[3];

/*
 * I use a intermediate format ( a simple bitmap ) with
 * this format ...
 */
typedef struct {
        int xsize, ysize;       /* sizes */
        int depth;              /* # of colors */
        int colors;             /* # of colors */
        byte *r, *g, *b;        /* components or bitmap (planes < 8) */
        byte *cmap;             /* cmap if planes < 8 */
        int magic;              /* used ? */
} bitmap_hdr;

#endif /* MY_LUG */
