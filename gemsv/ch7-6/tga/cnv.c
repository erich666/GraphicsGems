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
 * cnv.c - select the read/write function using the file name ( !!! ).
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Feb 6 1993
 * Copyright (c) 1993, Raul Rivero
 *
 */
 /*
  * The algorithm is not too acuratte, but this is to me a
  * curiosity not a vital part of LUG.
  *
  * If u wanna fix the problems, please do it ( and thank u ).
  */

#include "lug.h"
#include "lugfnts.h"

/*
 * Ok, we recognize these extensions ... 
 */
static char *lug_ff_ext[] = {
                        ".tga"
};

/*
 * Read file functions.
 */
static int (*read_lug_fnts[])() = {
                        read_tga_file
};

read_lug_file( name, bitmap )
char *name;
bitmap_hdr *bitmap;
{
  ifunptr read_file;

  /*
   * Get the correct funtion to read that file ( please, remember
   * tha we use the file extension and this is not perfect ).
   */
  read_file = get_readlug_function( name );

  /* Ok, do it. */
  read_file( name, bitmap );
}



ifunptr get_readlug_function( str )
char *str;
{
  ifunptr ptr;
  int value;

  /* Get the correct function for reading */
  ptr = read_lug_fnts[ value = get_index_function(str) ];
  return ptr;
}

get_index_function( str )
char *str;
{
  char dup[132];
  char *ptr;
  int len = strlen( str );
  int indexx;

  /* Duplicates the string */
  strcpy( dup, str );

  /* Check if exists the '.Z' extension ( and discard it ) */
  if ( dup[len-2] == '.' && dup[len-1] == 'Z' )
    dup[len-2] = 0;

  while ( ptr = strrchr( dup, '.' ) ) {
    /*
     * If the extension exists then we return the index
     * to the read/write function else continue ( and
     * disard the extension ).
     */
    if ( (indexx = get_real_index_function( ptr )) != -1 )
      return indexx;
    else ptr[0] = 0;
  }

  return DEFAULT_FORMAT;
}

get_real_index_function( str )
char *str;
{
  register int i = 0;

  while ( lug_ff_ext[i] ) {
    if ( strstr( str, lug_ff_ext[i] ) )
      return i;
    i++;
  }

  return -1;
}
