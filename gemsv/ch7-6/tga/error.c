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
 * error.c - error function.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Jan 4 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"

char *MY_NAME = "liblug";

int lugerrno = 0;
int stop_on_error = 1;

char *lug_errlist[] = {
        /*  0 */        "Bad format of usage\n",
        /*  1 */        "Cannot open file\n",
        /*  2 */        "Out of memory\n",
        /*  3 */        "Error while reading input file\n",
        /*  4 */        "Error while writing output file\n",
        /*  5 */        "Unkown input file type\n",
        /*  6 */        "File corrupt ( uncompress too bytes )\n",
        /*  7 */        "File is not a RGB image or gray scaled\n",
        /*  8 */        "Uncompress failed or compressed file don't exist\n",
        /*  9 */        "Unkown encoding type\n",
        /* 10 */        "Incorrect number of planes\n",
        /* 11 */        "Incorrect number of levels for dither\n",
        /* 12 */        "Incorrect size of images\n",
        /* 13 */        "Mapped image without color map ( ?! )\n",
        /* 14 */        "Incorrect image bits ( only 16, 24 or 32 )\n",
        /* 15 */        "File is not a mapped image\n",
        /* 16 */        "Cannot open graphics display\n",
        /* 17 */        "Interlazed image\n",
        /* 18 */        "File contains an uncomplete image\n",
        /* 19 */        "Using an image with MAGIC unset\n",
        /* 20 */        "Standard input not avaible with this format\n",
        /* 21 */        "Cannot get a Tag from the TIFF file\n",
        /* 22 */        "Incorrect resampling values\n",
        /* 23 */        "Image is not RGB/Gray scaled image\n",
                        NULL
};

char *lugerrmsg( code )
{
  return lug_errlist[ code ];
}

Error(code)
int code;
{
  int static last = 0;

  lugerrno = code;

  if ( !last ) {
    /*
     * Search, the first time, the last
     * error message.
     */
    for ( ; lug_errlist[last]; last++);
  }

  if ( stop_on_error ) {
    /*
     * Write the error message and
     * stop the program.
     */
    fprintf(stderr, "%s: ", MY_NAME);
    switch ( code ) {
      case 99:
             fprintf(stderr, "Not ready\n");
             break;
      default:
             if ( code >= 0 && code <= last ) {
               fputs( lug_errlist[code], stderr );
             }else {
                fprintf( stderr, "Unknown error code (%d)\n", code);
             }
             break;
    }
    exit( 1 );
  }
}
