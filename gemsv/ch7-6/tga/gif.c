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
 * gif.c - interface with Compuserve's GIF format.
 *
 * Authors:     Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Wed Jan 8 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"
#include "lugfnts.h"

#define GIFHEADER               "GIF87a"
#define NEWGIFHEADER            "GIF89a"
#define GIFIMGSEPAR             ','
#define ENDGIF                  ';'
#define GIFEXISTCOLOR           (1 << 7)
#define GIFINTERLAZE            (1 << 6)

byte *read_gif_screen_hdr();
byte *read_gif_image_hdr();
byte *unblock();

/*int oldread;*/
byte *ptr_image;
int image_size;
int lug_read_code_position;

extern int LUGverbose;

read_gif_file( name, bitmap )
char *name;
bitmap_hdr *bitmap;
{
  FILE *handle;

  /* Open the file descriptor */
  if ( name != NULL )
    handle = Fopen( name, "rb" );
  else error( 20 );

  lug_read_code_position = 0;

  /* Read the bitmap */
  read_gif( handle, bitmap );
  rm_compress();

  /* Close the file */
  Fclose( handle );
}

read_gif( handle, image )
FILE *handle;
bitmap_hdr *image;
{
  int xsize, ysize;
  byte *globalcmap, *localcmap;
  int mask;
  int codesize;
  int totalsize;
  int globalcolors, localcolors;
  int localmask, globalmask;

  /*
   * Read GIF headers ( we skip many parameters ).
   */
  read_gif_hdr( handle );
  globalcmap = read_gif_screen_hdr( handle, &globalcolors, &globalmask );
  localcmap = read_gif_image_hdr( handle, &localcolors, &localmask,
                                  &xsize, &ysize );

  /*
   * We only use one color map.
   */
  if ( localcolors ) {
    mask = localmask;
    image->cmap = localcmap;
    if ( globalcolors )
      free( globalcmap );
  }else
    if ( globalcolors ) {
      mask = globalmask;
      image->cmap = globalcmap;
    }else error( 13 );

  /*
   *  Original ( root ) codesize.
   */
  codesize= fgetc( handle );

  /*
   * Fill our header.
   */
  totalsize = xsize * ysize;
  image->magic = LUGUSED;
  image->xsize = xsize;
  image->ysize = ysize;
  image->colors = ( localcolors ? localcolors : globalcolors );
  image->depth = no_bits( image->colors ) + 1;
  image->r = (byte *) Malloc( totalsize );
  uncode_gif( handle, codesize, mask, image );
}

#define READCODE()      { code = read_code( ptrblocks, datamask,    \
                                            &offset, codesize );    \
                          ptrblocks += offset; }

uncode_gif( handle, codesize, mask, image )
FILE *handle;
int codesize, mask;
bitmap_hdr *image;
{
  int endblock;
  int clearcode;
  int offset;
  int freecode;
  int code;
  int current, old;
  int datamask;
  int codetop;
  int orig_codesize;
  short *sufix, *prefix;
  byte *blocks, *ptrblocks;
  int count = 0;
  byte *stack;
  int k;
  int totalsize;
  int position;

  /*
   * Allocate memory for internal buffers.
   */
  stack  = (byte *) Malloc( 4096 );
  sufix  = (short *) Malloc( 4096 * sizeof(short) );
  prefix = (short *) Malloc( 4096 * sizeof(short) );

  /*
   * Compute predefined values for uncompress.
   */
  clearcode = (1 << codesize);
  endblock = clearcode + 1;
  freecode = clearcode + 2;
  orig_codesize = ++codesize;
  codetop = 1 << codesize;
  datamask = codetop - 1;

  /*
   * Unblock the raster information.
   */
  ptrblocks = blocks = (byte *) unblock( handle );

  totalsize = image->xsize * image->ysize;

  /*
   * and now, ... UNPACK !!!.
   */
  VPRINTF( stderr, "Uncompressing GIF raster information\n" );
  READCODE();  /* read first code */
  while ( code != endblock ) {
    if ( code == clearcode ) {
      /*
       * Current code is clear so we reinitialize our tables.
       */
      codesize = orig_codesize;
      freecode = clearcode + 2;
      codetop = 1 << codesize;
      datamask = codetop - 1;
      READCODE();
      old = current = code;
      /* Next code to 'clear' is a root code */
      k = code & mask;
      position = push_gif( image->r, k );
    }else {
      /*
       * We have a normal code.
       */
      current = code;
      /*
       * If it's a new code then we need add the last translation.
       */
      if( !(code < freecode) ) {
        current = old;
        stack[count++] = (byte) k;
      }

      /*
       * mask equals to 'last_root_code' so while current
       * code greater [than mask] we haven't a root code
       * and continue adding values into ioutput.
       */
      while ( current > mask ) {
        stack[count++] = sufix[current];
        current = prefix[current];
      }
      k = current & mask;
      stack[count] = k;
      for ( ; count >= 0; count-- )
        position = push_gif( image->r, (int) stack[count] );
      count = 0;
      prefix[freecode  ] = old;
      sufix [freecode++] = k;
      old = code;
      /*
       * Check if we use all posibles values ( for this
       * codesize ).
       */
      if ( freecode >= codetop && codesize != 12 ) {
        codesize++;
        codetop = 1 << codesize;
        datamask = codetop - 1;
      }
    }
    READCODE();
    if ( position > totalsize )
      /*error(6)*/ break;
  }

  /*
   * Free original GIF raster information and
   * internal buffers.
   */
  free( blocks );
  free( prefix );
  free( sufix );
  free( stack );
}

read_code( buffer, mask, offset, codesize )
byte *buffer;
int mask;
int *offset;
int codesize;
{
  int aux = 0;
  int moveptr;

  aux  = *buffer++;
  aux |= ( *buffer++ << 8 );
  if ( codesize > 7 )
    aux |= ( *buffer++ << 16 );
  aux >>= lug_read_code_position;
  lug_read_code_position += codesize;
  moveptr = lug_read_code_position / 8;
  if ( moveptr ) {
    *offset = moveptr;
    lug_read_code_position %= 8;
  }else *offset = 0;

  return( aux & mask );
}

push_gif(buffer, indexx)
byte *buffer;
int indexx;
{
  static int offset;
  static byte *ptr;
  static byte *baseptr;

  if ( baseptr != buffer ) {
    ptr = baseptr = buffer;
    offset = 0;
  }

  *ptr++ = (byte) indexx;
  return ++offset;
}

read_gif_hdr( handle )
FILE *handle;
{
  char buffer[6];

  Fread( buffer, 6, 1, handle);
  if ( strncmp( buffer, GIFHEADER, 6 ) && 
       strncmp( buffer, NEWGIFHEADER, 6 ) )
    error( 5 );
}

byte *read_gif_screen_hdr( handle, colors, mask )
FILE *handle;
int *colors;
int *mask;
{
  /* int swidth, sheight; */
  byte buffer[7];
  byte *cmap;
  int cmapflag;
  int bitsperpixel;
  /* int background; */

  VPRINTF( stderr, "Reading GIF header\n" );

  Fread( buffer, 7, 1, handle);

  /* Screen size */
  /* swidth   = (buffer[1] << 8) | buffer[0]; */
  /* sheight  = (buffer[3] << 8) | buffer[2]; */

  cmapflag = buffer[4] & GIFEXISTCOLOR;
  bitsperpixel = (buffer[4] & 7) + 1;
  *colors = 1 << bitsperpixel;
  *mask = *colors - 1;
  /* background= buffer[5]; */

  if ( buffer[6] )
    errornull( 6 );

  if ( cmapflag ) {
    cmap = (byte *) Malloc( 3 * *colors );
    Fread( cmap, *colors, 3, handle );
  }

  return cmap;
}

byte *read_gif_image_hdr( handle, colors, mask, width, height )
FILE *handle;
int *colors;
int *mask;
int *width, *height;
{
  /* int left, right; */
  byte buffer[10];
  byte *cmap;
  int cmapflag;
  int interlace;
  int bitsperpixel;

  VPRINTF( stderr, "Reading GIF image header\n" );

  Fread( buffer, 10, 1, handle );

  if ( buffer[0] != GIFIMGSEPAR) {
    if (buffer[0] == '!') {
      int c;

      while ((c=getc(handle)) > 0) {
      }
      if (c == EOF)
        errornull( 5 );
      Fread( buffer, 10, 1, handle );
      if ( buffer[0] != GIFIMGSEPAR)
        errornull( 5 );
    }
  }

  /* left    = (buffer[2] << 8) | buffer[1]; */
  /* right   = (buffer[4] << 8) | buffer[3]; */
  *width  = (buffer[6] << 8) | buffer[5];
  *height = (buffer[8] << 8) | buffer[7];
  cmapflag  = buffer[9] & GIFEXISTCOLOR;
  interlace = buffer[9] & GIFINTERLAZE;

  if ( interlace )
    errornull( 17 );

  if ( cmapflag ) {
    bitsperpixel = ( buffer[9] & 7 ) + 1;
    *colors = 1 << bitsperpixel;
    *mask = *colors - 1;
    cmap = (byte *) Malloc( 3 * *colors );
    Fread( cmap, *colors, 3, handle );
  }else *colors = 0;

  return cmap;
}

byte *unblock( handle )
FILE *handle;
{
  long position;
  int totalsize;
  byte *out, *ptr;
  int size;
  int count = 0;

  VPRINTF( stderr, "Unblocking GIF file\n" );
  /*
   * We need read the raster information and strip the block
   * size marks. GIF files can store more than one image but
   * this is not usually ( so we move handle to the end and
   * supose that size ).
   */
  position = ftell( handle );
  fseek( handle, 0L, 2 );
  totalsize = ftell( handle ) - position;
  fseek( handle, position, 0 );

  ptr = out = (byte *) Malloc( totalsize );

  /*
   * Format is:  <size><...block...><size><...block...>[...]<0>
   */
  size = fgetc( handle );
  while ( size ) {
    /* Check if no problems with space */
    count += size;
    if ( count > totalsize )
      errornull( 7 );
    /* No problems => read the block */
    Fread( ptr, size, 1, handle );
    ptr += size;
    size = fgetc( handle );
  }

  return out;
}

write_gif_file( name, image )
char *name;
bitmap_hdr *image;
{
  FILE *handle;

  /* Open the file descriptor */
  if ( name != NULL )
    handle = Fopen( name, "wb" );
  else handle = stdout;

  /* Write the bitmap */
  write_gif( handle, image );

  /* Close the file */
  Fclose( handle );

}

write_gif(handle, image)
FILE *handle;
bitmap_hdr *image;
{
  int codesize;
  int read_pixel();

  if ( image->magic != LUGUSED )
    error( 19 );

  /* GIF only support mapped images */
  if ( image->depth > 8 )
    error( 15 );

  VPRINTF(stderr, "Writing GIF headers\n");
  /*
   * Write GIF headers.
   */
  write_gif_hdr( handle );
  write_gif_screen_hdr( handle, image );
  write_gif_cmap( handle, image );
  write_gif_image_hdr( handle, image );

  /*
   * Now store the original code size.
   */
  codesize = image->depth;
  fputc( codesize, handle );

  /*
   * Image size.
   */
  image_size = image->xsize * image->ysize;
  /* el primer codigo libre sera de +1 bits */
  codesize++;
  /*
   * Compress the image.
   */
  VPRINTF(stdout, "Compressing raster information\n");
  ptr_image = image->r;
  compress( codesize, handle, read_pixel );

  /*
   * Block with a size of 0 bytes.
   */
  fputc( 0, handle );

  /*
   * End of gif file.
   */
  fputc( ENDGIF, handle );      /* fin del fichero GIF */
}

write_gif_hdr(handle)
FILE *handle;
{
  Fwrite( GIFHEADER, 6, 1, handle );
}

write_gif_screen_hdr(handle, image)
FILE *handle;
bitmap_hdr *image;
{
  byte buffer[7];
  byte *ptr;

  ptr = buffer;
  /* Width ( of screen ) */
  *ptr++ = LSB(image->xsize);
  *ptr++ = MSB(image->xsize);
  /* Height ( of screen ) */
  *ptr++ = LSB(image->ysize);
  *ptr++ = MSB(image->ysize);
  /* Colors */
  *ptr++ = (1 << 7) | (no_bits(image->colors) << 4) | (no_bits(image->colors));
  /* Background color */
  *ptr++ = 0;
  /* End of block */
  *ptr++ = 0;

  Fwrite( buffer, 7, 1, handle);
}

write_gif_cmap(handle, image)
FILE *handle;
bitmap_hdr *image;
{
  Fwrite(image->cmap, 3, image->colors, handle);
}

write_gif_image_hdr(handle, image)
FILE *handle;
bitmap_hdr *image;
{
  byte buffer[10];
  byte *ptr;

  ptr = buffer;
  /* Separator */
  *ptr++ = GIFIMGSEPAR;
  /* Left adjust */
  *ptr++ = 0;
  *ptr++ = 0;
  /* Right adjust */
  *ptr++ = 0;
  *ptr++ = 0;
  /* Width ( of image ) */
  *ptr++ = LSB(image->xsize);
  *ptr++ = MSB(image->xsize);
  /* Height ( of image ) */
  *ptr++ = LSB(image->ysize);
  *ptr++ = MSB(image->ysize);
  /* Word of definition ( without colormap, ... ) */
  *ptr++ = 7;

  Fwrite( buffer, 10, 1, handle);
}

read_pixel()
{
  static int size;
  static byte *base, *ptr;

  if ( base != ptr_image ) {
    /*
     * It's a new pointer, so we need stored.
     */
    base = ptr = ptr_image;
    size = image_size;
  }

  if ( size ) {
    /*
     * We have pixels ...
     */
    size--;
    return( (int)(*ptr++) );
  }else {
    return EOF; /* no more pixels, this is the end */
  }
}
