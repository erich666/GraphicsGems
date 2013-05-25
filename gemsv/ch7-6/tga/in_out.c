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
 * in_out.c - input and output subroutines.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Jan 4 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

/*
 * Subrutinas para entrada y salida de ficheros.
 */

#include "lug.h"
#include "lugfnts.h"

extern int getpid();
extern char *getenv();

static char aux_file[80];      /* temporal file for uncompressing files */

/*
 * Fread and Fwrite functions are defined in lug.h.
 */

FILE *Fopen(name, mode)
char *name;
char *mode;
{
  static byte first_fopen = 0;
  FILE *handle;
  int flag;
  struct stat statbuf;

  if ( mode[0] == 'r' ) {
    if ( !(flag = exist_file(name)) )
      error( 1 );   /* the file doesn't exist */

    if ( flag == FILE_EXIST ) {
      /* The file exist with the <name> name :-), so open it !*/
      if ( (handle = fopen( name, mode )) == NULL )
	error( 1 );
      return handle;
    }else {
      /*
       * The file exist, but compressed!. So we uncompressed over
       * a tmp file and return a handle for that file.
       */
      if ( !first_fopen ) {     /* is the first time ? */
        first_fopen++;
        sprintf( aux_file, "%s/lug%d", TMPDIR, getpid() );
      }
      /* Uncompress it */
      Uncompress( name, aux_file );
      /* Open it */
      if ( ( handle = fopen(aux_file, mode) ) == NULL) {
        fputs( aux_file, stderr );
        error(1);
      }else {
	stat( aux_file, &statbuf );
        if ( statbuf.st_size < 1 )
          error( 1 );
      }
      return handle;
    }
  }else {
    /*
     * Ask for writing ( we don't need check for
     * compressed files ).
     */
    if ( (handle = fopen(name, mode)) == NULL)
      error(1);
  }

  return handle;
}

exist_file( name )
char *name;
{
  int len = strlen(name) - 1;
  char aux[132];

  /* No characters ?! */
  if ( len < 0 )
    return FILE_NO_EXIST;

  /* Exists this file ? */
  if ( !access( name, 0 ) ) {
    /*
     * Ok, exist, but is a compressed file ?.
     */
    if ( !strcmp(&(name[len-2]), ".gz") )
      return FILE_IS_GZIP;   /* was a .gz file ==> a GZIP file */
    else if ( !strcmp(&(name[len-1]), ".Z") )
	   return FILE_IS_COMPRESS;   /* was a .Z file ==> a COMPRESS file */

    return FILE_EXIST;   /* well, a normal file :-) */
  }

  /*
   * The file doesn't exist, so check compressed versions.
   */
  sprintf( aux, "%s.gz", name );
  if ( !access( aux, 0 ) )
    return FILE_WITH_GZIP;   /* we have a <name>.gz file */
  sprintf( aux, "%s.Z", name );
  if ( !access( aux, 0 ) )
    return FILE_WITH_COMPRESS;   /* we have a <name>.Z file */

  /* Oopppsssss!, no file */
  return FILE_NO_EXIST;
}

Fclose( handle )
FILE *handle;
{
  if ( handle != stdout && handle != stdin )
    return fclose( handle );
  else return 0;
}

rm_compress()
{
  if ( access( aux_file, 0 ) == 0 )
    unlink( aux_file );
}

char *read_file(handle, bytes)
FILE *handle;
int *bytes;
{
  char *buffer;

  /* Get size of file and allocate memory */
  *bytes = (int) filelen(handle);
  if ( *bytes > 0 ) {           /* Ok, it's a regular file. */
    buffer = (char *) Malloc(*bytes);

    /* Read it */
    Fread(buffer, (int) (*bytes), 1, handle);
  }else {                       /* Oops!  It's a pipe. */
    int n = 0, bufsize = 0;

    /* Read in chunks of BUFSIZ. */
    buffer = (char *) Malloc( BUFSIZ );
    while ( (n = fread( buffer + bufsize, 1, BUFSIZ, handle )) == BUFSIZ ) {
      bufsize += BUFSIZ;
      buffer = (char *) realloc( buffer, bufsize + BUFSIZ );
    }
    if ( n >= 0 )
      n += bufsize;
    else
      n = bufsize;
    *bytes = n;
  }

  /* Return the buffer */
  return buffer;
}

long filelen(handle)
FILE *handle;
{
  long current_pos;
  long len;

  /* Save current position */
  current_pos= ftell(handle);

  /* Get len of file */
  fseek(handle, 0, 2);
  len= ftell(handle);

  /* Restore position */
  fseek(handle, current_pos, 0);

  return len;
}

putshortMSBF( value, handle )
int value;
FILE *handle;
{
  putc( MSB(value), handle );
  putc( LSB(value), handle );
}

putshortLSBF( value, handle )
int value;
FILE *handle;
{
  putc( LSB(value), handle );
  putc( MSB(value), handle );
}

getshortMSBF( handle )
FILE *handle;
{
  int aux;

  aux = getc( handle ) << 8;
  aux |= getc( handle );

  return aux;
}

getshortLSBF( handle )
FILE *handle;
{
  int aux;

  aux = getc( handle );
  aux |= (getc( handle ) << 8);

  return aux;
}

