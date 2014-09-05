/*
	file:		utah.h
	description:	interface to Utah RLE toolkit
	author:		A. T. Campbell
	date:		October 30, 1989
*/

#ifndef UTAH_H
#define UTAH_H

/******************************************************************************/

/* include files */
/* You will have to get these from the ancient Utah Raster Toolkit,
   http://www.cs.utah.edu/gdc/projects/urt/ - best of luck! */
#include "rle.h"

/******************************************************************************/

/* type definitions */
typedef rle_hdr UTAH_FILE;

/******************************************************************************/

/* return values */
extern int		utah_read_close();
extern UTAH_FILE	*utah_read_init();
extern int		utah_read_pixels();
extern int		utah_read_rgb();
extern int		utah_write_close();
extern UTAH_FILE	*utah_write_init();
extern int		utah_write_pixels();
extern int		utah_write_rgb();

/******************************************************************************/

#endif UTAH_H
