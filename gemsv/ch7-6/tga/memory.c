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
 * memory.c - allocate mamory.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Jan 4 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"

extern int stop_on_error;

char *Malloc(size)
int size;
{
  char *ptr;

  if ((ptr = (char *) malloc(size)) == NULL) {
    stop_on_error = 1;
    Error( 2 );
  }

  /*
   * Usually compilers fill buffers with zeros,
   * but ...
   */
  bzero( ptr, size );
  return ptr;
}

Free( ptr )
void *ptr;
{
  if ( ptr != NULL )
    free( ptr );
}
