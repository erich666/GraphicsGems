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
 * lugconf.h - machine depend configuration.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Wed 15 Jul 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */


#ifndef MY_CONFIGURE_LUG

#define MY_CONFIGURE_LUG

/* 
 * If your machine is not bigendian then
 * you'll need define the NO_BIGENDIAN macro.
 *
 * The DEC and PC machines needs this. 
 *
 * If your compiler defines the MSDOS macro ( ex: the
 * djgcc ), the definition of BIG_ENDIAN is done 
 * automaticly.
 *
 */
#ifdef MSDOS 
#       define NO_BIGENDIAN
#else
#       undef NO_BIGENDIAN
#endif

/*
 * If you has an ANSI compiler ( ex: gcc ), define
 * the USE_PROTOTYPES macro.
 */
#undef USE_PROTOTYPES


/*
 * Define the USE_STDLIB_H macro if your machine has
 * the stdlib.h header.
 */
#define USE_STDLIB_H


/*
 * Define the VOID_STAR macro if your malloc definition
 * needs void* instead of char*.
 *
 * ( Only used if USE_STDLIB_H hasn't been defined. )
 */
#ifndef USE_STDLIB_H
#       define USE_STDLIB_H
#endif


/*
 * Define USE_STRINGS if you have the strings.h
 * header ( and not the standard string.h ).
 */
#undef USE_STRINGS


/*
 * If your library has bcopy, bzero, index, ...
 * then you'll need define BSD ( else LUG will use
 * memcpy, memset, ... ).
 */
#undef BSD

/*
 * If you have installed the GNU's GZIP and you wanna
 * use it instead of COMPRESS then define the USE_GNU_GZIP
 * macro.
 */
#define USE_GNU_GZIP


#endif  /* MY_CONFIGURE_LUG */

