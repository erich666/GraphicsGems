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
 * tga.c - interface with Targa format.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Wed Jan 22 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"
#include "lugfnts.h"

read_tga_file( name, bitmap )
char *name;
bitmap_hdr *bitmap;
{
  FILE *handle;

  /* Open the file descriptor */
  if ( name != NULL )
    handle = Fopen( name, "rb" );
  else handle = stdin;

  /* Read the bitmap */
  read_tga( handle, bitmap );
  rm_compress();

  /* Close the file */
  Fclose( handle );
}

read_tga(handle, image)
FILE *handle;
bitmap_hdr *image;
{
  register int i;
  tga_hdr tga;
  int allplanes = 0;
  int lessplanes = 0;
  int forceallplanes = 0;
  byte aux4[4];
  int totalsize;
  byte *ptr;
  byte *r, *g, *b;
  byte *aux;

  /* Read the Tagar header */
  read_tga_header( handle, &tga );

  /* Fill our header */
  image->magic = LUGUSED;
  image->xsize = tga.xsize;
  image->ysize = tga.ysize;
  totalsize = image->xsize * image->ysize;
  /*
   * Targa format can be mapped but with moer than 256 colors,
   * our mapped images only support 8 bits, so we need convert
   * these images to 24 bits.
   */
  switch ( tga.image_type ) {
    case TGA_RGB:
    case TGA_RLE_RGB:
                allplanes = 1;
                break;
    case TGA_MAPPED:
    case TGA_RLE_MAPPED:
                lessplanes = 1;
                if ( tga.cmap_type != 1 )
                  error( 10 );
                if ( tga.cmap_length > 256 )
                  forceallplanes = 1;
                break;
  }

  /*
   * Read the cmap info ( if exists ).
   */
  if ( tga.cmap_type ) {
/*    VPRINTF(stderr, "Reading cmap info\n"); */
    if ( allplanes ) {
      /* We only need skip the cmap block */
      for ( i = 0; i < tga.cmap_length; i++ )
        read_tga_data( aux4, tga.cmap_entry_size, handle );
    }else {
      ptr = image->cmap = (byte *) Malloc( 3 * tga.cmap_length );
      for ( i = 0; i < tga.cmap_length; i++) {
        read_tga_data( ptr, tga.cmap_entry_size, handle );
        ptr += 3;
      }
    }
  }

  /*
   * Allocate memory for the bitmap and fill our header.
   */
  image->xsize = tga.xsize;
  image->ysize = tga.ysize;
  totalsize = image->xsize * image->ysize;
  if ( allplanes || forceallplanes ) {
    r = image->r = (byte *) Malloc( totalsize );
    g = image->g = (byte *) Malloc( totalsize );
    b = image->b = (byte *) Malloc( totalsize );
    image->depth = 24;
  }else {
    r = image->r = (byte *) Malloc( totalsize );
    image->depth = no_bits( tga.cmap_length ) + 1;
  }
  image->colors = ( 1 << image->depth );

  /*
   * Read the raster information.
   */
  if ( allplanes ) {
    read_tga24( handle, r, g, b, &tga );

  }else if ( forceallplanes )
          read_tga_to24( handle, r, g, b, &tga, image->cmap );
        else read_tga8( handle, r, &tga );

/*  if ( TGA_FLIP( tga.image_descriptor ) ) {
    VPRINTF(stderr, "Flipping the image\n"); 
    aux = (byte *) flip( r, image->xsize, image->ysize );
    free( r );
    image->r = aux;
    if ( allplanes ) {

      aux = (byte *) flip( g, image->xsize, image->ysize );
      free( g );
      image->g = aux;
      aux = (byte *) flip( b, image->xsize, image->ysize );
      free( b );
      image->b = aux;
    }
} */
}

read_tga24( handle, r, g, b, tga )
FILE *handle;
byte *r, *g, *b;
tga_hdr *tga;
{
  register int i, j;
  int aux;
  byte *ptr;
  byte *buffer;
  int totalsize;
  int count = 0;
  byte repeat;
  byte rbyte, gbyte, bbyte;
  int code;

  if ( tga->image_type < 9 ) {
    /* No RLE ! */
/*    VPRINTF(stderr, "Reading true color Targa image\n"); */
    buffer = (byte *) Malloc( tga->pixel_size * tga->xsize );
    for ( j = 0; j < tga->ysize; j++ ) {
#ifdef DEBUG
/*      VPRINTF(stderr, "Reading line %d\r", j); VFLUSH( stderr ); */
#endif
      fread( buffer, tga->xsize, tga->pixel_size, handle);
      ptr = buffer;
      if ( tga->pixel_size < 3 ) {
        for ( i = 0 ; i < tga->xsize; i++ ) {
          aux = ( ptr[1] << 8 ) | ptr[0];
          *r++ = (aux & 0x7c00) >> 7;
          *g++ = (aux & 0x03e0) >> 2;
          *b++ = (aux & 0x001F) << 3;
          ptr += 2;
        }
      }else {
        for ( i = 0 ; i < tga->xsize; i++ ) {
          *b++ = *ptr++;
          *g++ = *ptr++;
          *r++ = *ptr++;
          if ( tga->pixel_size == 4 )
            ptr++;
        }
      }
    }
#ifdef DEBUG
/*    VPRINTF( stderr, "\n" ); */
#endif
    free( buffer );
  }else {
/*    VPRINTF(stderr, "Reading true color Targa RLE image\n"); */
    /* RLE */
    buffer = (byte *) Malloc( 256 * tga->pixel_size );
    totalsize = tga->xsize * tga->ysize;
    while ( count < totalsize ) {
      Fread( &repeat, 1, 1, handle );
      code = (repeat & 127) + 1;
      count += code;
      if ( (repeat & 128) ) {
        Fread( buffer, tga->pixel_size, 1, handle );
        if ( tga->pixel_size < 3 ) {
          aux = ( buffer[1] << 8 ) | buffer[0];
          rbyte = (aux & 0x7c00) >> 7;
          gbyte = (aux & 0x03e0) >> 2;
          bbyte = (aux & 0x001F) << 3;
        }else {
          rbyte = buffer[2];
          gbyte = buffer[1];
          bbyte = buffer[0];
        }
        memset( r, rbyte, code ); r += code;
        memset( g, gbyte, code ); g += code;
        memset( b, bbyte, code ); b += code;
      }else {
        Fread( buffer, tga->pixel_size, code, handle );
        ptr = buffer;
        while ( code-- ) {
          if ( tga->pixel_size < 3 ) {
            aux = ( ptr[1] << 8 ) | ptr[0];
            *r++ = (aux & 0x7c00) >> 7;
            *g++ = (aux & 0x03e0) >> 2;
            *b++ = (aux & 0x001F) << 3;
            ptr += 2;
          }else {
            *b++ = *ptr++;
            *g++ = *ptr++;
            *r++ = *ptr++;
            if ( tga->pixel_size == 4 )
              ptr++;
          }
        }
      } /* if ( (repeat ... ) ) */
    } /* while ( count ... ) */
    free( buffer );
  }
}

read_tga_to24( handle, r, g, b, tga, cmap )
FILE *handle;
byte *r, *g, *b;
tga_hdr *tga;
byte *cmap;
{
  fprintf(stderr, "Converting to 24 planes\n");
  error( 99 );
}

read_tga8( handle, r, tga )
FILE *handle;
byte *r;
tga_hdr *tga;
{
  if ( tga->image_type == 1 ) {
/*    VPRINTF( stderr, "Reading mapped Targa image\n" ); */
    Fread( r, tga->xsize, tga->ysize, handle );
  }else {
    byte buffer, repeat;
    int totalsize = tga->xsize * tga->ysize;
    int count = 0;
    int code;

/*    VPRINTF( stderr, "Reading mapped RLE Targa image\n" ); */
    /* RLE */
    while ( count < totalsize ) {
      Fread( &repeat, 1, 1, handle );
      code = (repeat & 127) + 1;
      count += code;
      if ( (repeat & 128) ) {
        Fread( &buffer, 1, 1, handle );
        memset( r, buffer, code ); r += code;
      }else {
        Fread( r, code, 1, handle );
        r += code;
      }
    } /* while ( count ... ) */
  }
}

read_tga_header(handle, tga)
FILE *handle;
tga_hdr *tga;
{
  byte buffer[18];

  Fread( buffer, 18, 1, handle );

  /*
   * Bytes are in reverse order so ...
   */
  tga->num_id = buffer[0];
  tga->cmap_type = buffer[1];
  tga->image_type = buffer[2];
  tga->cmap_orign = ( buffer[4] << 8 ) | buffer[3];
  tga->cmap_length = ( buffer[6] << 8 ) | buffer[5];
  tga->cmap_entry_size = buffer[7] / 8;
  tga->xorig = ( buffer[9] << 8 ) | buffer[8];
  tga->yorig = ( buffer[11] << 8 ) | buffer[10];
  tga->xsize = ( buffer[13] << 8 ) | buffer[12];
  tga->ysize = ( buffer[15] << 8 ) | buffer[14];
  tga->pixel_size = buffer[16] / 8;     /* we'll use it directly */
  tga->image_descriptor = buffer[17];

/*  VPRINTF(stderr, "Image type %d\n", tga->image_type);
  VPRINTF(stderr, "%s color map\n", (tga->cmap_type ? "With" : "Without"));
  VPRINTF(stderr, "Cmap origin: %d\n", tga->cmap_orign );
  VPRINTF(stderr, "Cmap length: %d\n", tga->cmap_length );
  VPRINTF(stderr, "Cmap entry size: %d\n", tga->cmap_entry_size );
  VPRINTF(stderr, "Image size: %dx%d\n", tga->xsize, tga->ysize );
  VPRINTF(stderr, "Pixel size: %d\n", tga->pixel_size ); */
/*  if ( TGA_INTERLACED(tga->image_descriptor) )
    VPRINTF(stderr, "Interlace image\n"); 
  if ( TGA_FLIP(tga->image_descriptor) )
    VPRINTF(stderr, "Flipped image\n"); */

  /* Interlaced ? */
  if ( TGA_INTERLACED(tga->image_descriptor) ) {
    fprintf( stderr, "Interlaced image\n");
    error( 99 );
  }

  /*
   * Skip the identification.
   */
  if ( tga->num_id ) {
/*    VPRINTF( stderr, "Image identification: "); */
    while ( tga->num_id ) {
      (void) getc( handle );
      tga->num_id--;
    }
/*    VPRINTF( stderr, "\n" ); */
  }

  /*
   * Check the image type.
   */
  switch ( tga->image_type ) {
    case TGA_RGB:
    case TGA_RLE_RGB:
    case TGA_MAPPED:
    case TGA_RLE_MAPPED:
                break;
    default:
                fprintf(stderr, "Image type %d\n", tga->image_type);
                error( 99 );
                break;
  }
}

read_tga_data( buffer, no_bytes, handle )
byte *buffer;
int no_bytes;
FILE *handle;
{
  int aux1;
  byte aux[4];

  Fread( aux, no_bytes, 1, handle );

  if ( no_bytes < 3 ) {
    aux1 = ( aux[1] << 8 ) | aux[0];
    *buffer++ = (aux1 & 0x7c00) >> 7;
    *buffer++ = (aux1 & 0x03e0) >> 2;
    *buffer++ = (aux1 & 0x001F) << 3;
  }else {
     *buffer++ = aux[2];
     *buffer++ = aux[1];
     *buffer++ = aux[0];
  }
}

write_tga_file( name, image )
char *name;
bitmap_hdr *image;
{
  FILE *handle;

  /* Open the file descriptor */
  if ( name != NULL )
    handle = Fopen( name, "wb" );
  else handle = stdout;

  /* Write the bitmap */
  write_tga( handle, image, 0 );

  /* Close the file */
  Fclose( handle );
}

write_rle_tga_file( name, image )
char *name;
bitmap_hdr *image;
{
  FILE *handle;

  /* Open the file descriptor */
  if ( name != NULL )
    handle = Fopen( name, "wb" );
  else handle = stdout;

  /* Write the bitmap */
  write_tga( handle, image, 1 );

  /* Close the file */
  Fclose( handle );
}

write_tga(handle, image, rle)
FILE *handle;
bitmap_hdr *image;
int rle;
{
  register int i;
  byte *r, *g, *b;

  if ( image->magic != LUGUSED )
    error( 19 );

  if ( image->depth <= 8 )
    error( 7 );

  /* Write the header */
  write_tga_header( handle, image, rle );

  /* Set the pointers */
  r = image->r;
  g = image->g;
  b = image->b;

  /* Write each line */
/*  VPRINTF(stderr, "Writing Targa %s raster data\n", (rle ? "RLE" : "")); */
  for ( i = 0; i < image->ysize; i++ ) {
    if ( !rle )
      write_tga_line24( handle, r, g, b, image->xsize );
    else write_tga_rle_line24( handle, r, g, b, image->xsize );
    r += image->xsize;
    g += image->xsize;
    b += image->xsize;

  }
}

write_tga_header(handle, image, rle)
FILE *handle;
bitmap_hdr *image;
int rle;
{
  byte buffer[ sizeof(tga_hdr) ];

/*  VPRINTF(stderr, "Writing Targa header\n"); */
  /* First we fill with zero, then skip too asigments */
  bzero( buffer, sizeof(tga_hdr) );

  /* image_type */
  if ( image->depth > 8 ) {
    if ( rle )
      buffer[2] = TGA_RLE_RGB;
    else buffer[2] = TGA_RGB;
  }else {
    if ( rle )
      buffer[2] = TGA_RLE_MAPPED;
    else buffer[2] = TGA_MAPPED;
  }

  buffer[12] = LSB( image->xsize );
  buffer[13] = MSB( image->xsize );     /* the weigth */
  buffer[14] = LSB( image->ysize );
  buffer[15] = MSB( image->ysize );     /* the height */
  buffer[16] = 24;                      /* Targa 24 => RGB */
  buffer[17] = 32;              /* flipped */

  /* Write the header */
  Fwrite( buffer, 18, 1, handle );
}

write_tga_line24(handle, r, g, b, xsize)
FILE *handle;
byte *r, *g, *b;
int xsize;
{
  static byte buffer[3*2560];
  byte *ptr;
  byte *end;

  end = r + xsize;
  ptr = buffer;

  /* Do a single buffer */
  while ( r < end ) {
    *ptr++ = *b++;
    *ptr++ = *g++;
    *ptr++ = *r++;
  }

  /* Write it ! */
  Fwrite( buffer, xsize, 3, handle );
}

write_tga_rle_line24(handle, r, g, b, xsize)
FILE *handle;
byte *r, *g, *b;
int xsize;
{
  byte *end;
  byte cr, cb, cg;
  short repeat= 1;
  short diferent= 0;
  static byte salida[4*2560];
  byte bufaux[4*256];
  byte *ptr, *aux;

  ptr = salida;
  end = r + xsize + 1;
  cr = *r++;
  cg = *g++;
  cb = *b++;
  while ( r < end ) {
    while ( cr == *r  && cg == *g  && cb == *b  &&
            repeat < 127 && r < end) {
      r++, g++, b++;
      repeat++;
    }
    if (repeat > 1 ) {
      /* Put this information on our buffer */
      *ptr++ = 128 | (repeat - 1);
      *ptr++ = cb;
      *ptr++ = cg;
      *ptr++ = cr;
      repeat = 1;
      cr = *r++;
      cg = *g++;
      cb = *b++;
    }
    aux= bufaux;
    /*
     * If there are diferents bytes, then we need
     * know how many.
     */
    while ( (cr != *r  || cg != *g  || cb != *b)  &&
            diferent< 127 && r < end) {
      *aux++ = cb;
      *aux++ = cg;
      *aux++ = cr;
      cr = *r++;
      cg = *g++;
      cb = *b++;
      diferent++;
    }
    if (diferent) {
      *ptr++ = diferent - 1;
      bcopy( bufaux, ptr, 3*diferent );
      ptr += 3*diferent;
      diferent = 0;
    }
  }

  Fwrite(salida, ptr-salida, 1, handle);
}
