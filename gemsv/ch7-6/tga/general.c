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
 * general.c - general ( non clasified ) functions.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Jan 4 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"
#include "lugfnts.h"

int LUGverbose = 0;

/*
 * Return the number of bits ( -1 ) to represent a given
 * number of colors ( ex: 256 colors => 7 ).
 */
no_bits( colors )
int colors;
{
  register int bits= 0;

  colors--;
  while ( colors >> bits )
    bits++;

  return (bits-1);
}

/*
 * Like basename command.
 */
char *basename(string, suffix)
char *string;
char *suffix;
{
  char *st_ptr= string;
  char *su_ptr= suffix;

  /*
   * Pointers to the end of strings.
   */
  while (*st_ptr)
    st_ptr++;
  while (*su_ptr)
    su_ptr++;

  /*
   * Ahora los vamos retrasando hasta que sean distintas
   * ( y en el string metemos un cero ).
   */
  while ( *st_ptr == *su_ptr )
    st_ptr--, su_ptr--;

  /* The end of the string */
  *(++st_ptr)= 0;

  return( string );
}

short *bytetoshort( in, out, size )
byte *in;
short *out;
int size;
{
  short *ptr;

  /*
   * If the output buffer is NULL then we need allocate
   * memory, so ...
   */
  if ( out == NULL ) {
    ptr = out = (short *) Malloc( size * sizeof(short) );
  }else ptr = out;

  while ( size-- ) {
    *ptr++ = *in++;
  }

  return out;
}

byte *shorttobyte( in, out, size )
short *in;
byte *out;
int size;
{
  byte *ptr;

  /*
   * If the output buffer is NULL then we need allocate
   * memory, so ...
   */
  if ( out == NULL ) {
    ptr = out = (byte *) Malloc( size );
  }else ptr = out;

  while ( size-- ) {
    *ptr++ = *in++;
  }

  return out;
}

byte *floattobyte( in, out, size )
float *in;
byte *out;
int size;
{
  byte *ptr;

  /*
   * If the output buffer is NULL then we need allocate
   * memory, so ...
   */
  if ( out == NULL ) {
    ptr = out = (byte *) Malloc( size );
  }else ptr = out;

  while ( size-- ) {
    *ptr++ = (byte) (255. * *in++);
  }

  return out;
}

float *bytetofloat( in, out, size )
float *out;
byte *in;
int size;
{
  float *ptr;

  /*
   * If the output buffer is NULL then we need allocate
   * memory, so ...
   */
  if ( out == NULL ) {
    ptr = out = (float *) Malloc( size * sizeof(float));
  }

  while ( size-- ) {
    *ptr++ = ((float)*in++) / 255.;
  }

  return out;
}

Atoi( string )
char *string;
{
  int aux = -1234;

  sscanf( string, "%d", &aux );
  if ( aux == -1234 )
    error( 0 );

  return aux;
}

double
Atod( string )
char *string;
{
  double aux = -1234.;

  sscanf( string, "%lf", &aux );
  if ( aux == -1234. )
    error( 0 );

  return aux;
}

isnumber( c )
char *c;
{
  while ( *c )
    if ( !isdigit(*c++) )
      return 0;

  return 1;
}


/*
 * Compress and uncompress subroutines.
 */

Uncompress(name, aux_file)
char *name, *aux_file;
{
#ifndef MSDOS
  int pid, handle;

  if ( pid = fork() ) {
    /*
     * Parent wait until the REAL 'end of days'
     * of the child.
     */
    do{
    }while( pid != wait(NULL) );
  }else {
    handle = creat( aux_file, 0644 );
    close( 1 );
    dup( handle );
#ifdef USE_GNU_GZIP
    execlp( "gzip", "gzip", "-dc", name, 0 );
#else
    execlp( "compress", "compress", "-dc", name, 0 );
#endif  /* USE_GNU_GZIP */
  }
#else  /* MSDOS */
  int handle;
  char cmd[132];
  
  handle = creat( aux_file, 0644 );
  close( 1 );
#ifdef USE_GNU_GZIP
  sprintf( cmd, "gzip -dc %s", aux_file );
#else
  sprintf( cmd, "compress -dc %s", aux_file );
#endif  /* USE_GNU_GZIP */
  dup( handle );
  system( cmd );
#endif  /* MSDOS */
}

Compress(name)
char *name;
{
#ifndef MSDOS
  int pid;

  if ( pid = fork() ) {
    /*
     * Parent wait until the REAL 'end of days'
     * of the child.
     */
    do{
    }while( pid != wait(NULL) );
  }else {
    /*
     * Lets go baby ...
     */
#ifdef USE_GNU_GZIP
    execlp( "gzip", "gzip", "-9", name, 0 );
#else
    execlp( "compress", "compress", name, 0 );
#endif  /* USE_GNU_GZIP */
  }
#else  /* MSDOS */
  char cmd[132];

#ifdef USE_GNU_GZIP
  sprintf( cmd, "gzip -9 %s", name );
#else
  sprintf( cmd, "compress %s", name );
#endif  /* USE_GNU_GZIP */
  system( cmd );
#endif  /* MSDOS */
}

compute_levels( no_colors )
int no_colors;
{
  int n = 7;

  do {
    n--;
  }while ( n*n*n > no_colors );

  return n;
}
