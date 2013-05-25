/*
 *		Filtered Image Rescaling
 *
 *		  by Dale Schumacher
 *
 */

/*
	Additional changes by Ray Gardener, Daylon Graphics Ltd.
	December 4, 1999

	Summary:

		- Horizontal filter contributions are calculated on the fly,
		  as each column is mapped from src to dst image. This lets 
		  us omit having to allocate a temporary full horizontal stretch 
		  of the src image.

		- If none of the src pixels within a sampling region differ, 
		  then the output pixel is forced to equal (any of) the source pixel.
		  This ensures that filters do not corrupt areas of constant color.

		- Filter weight contribution results, after summing, are 
		  rounded to the nearest pixel color value instead of 
		  being casted to Pixel (usually an int or char). Otherwise, 
		  artifacting occurs. 

		- All memory allocations checked for failure; zoom() returns 
		  error code. new_image() returns NULL if unable to allocate 
		  pixel storage, even if Image struct can be allocated.
		  Some assertions added.

		- load_image(), save_image() take filenames, not file handles.

		- TGA bitmap format available. If you want to add a filetype, 
		  extend the gImageHandlers array, and factor in your load_image_xxx()
		  and save_image_xxx() functions. Search for string 'add your' 
		  to find appropriate code locations.

		- The 'input' and 'output' command-line arguments do not have 
		  to specify .bm files; any supported filetype is okay.

		- Added implementation of getopt() if compiling under Windows.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "GraphicsGems.h"

static char	_Program[] = "fzoom";
static char	_Version[] = "0.30";
static char	_Copyright[] = "Public Domain 1991 by Dale Schumacher. Mods by Ray Gardener";

#ifndef EXIT_SUCCESS
#define	EXIT_SUCCESS	(0)
#define	EXIT_FAILURE	(1)
#endif

/* M_PI was not in gems header ?? */
#ifndef M_PI
	#define M_PI	 3.14159265359
#endif

#ifdef WIN32

/* getopt() - Parses command line (argv) looking for switch chars 
	Returns option flag char if option found.
	Returns EOF if end of arglist or non-flag arg reached.
	Returns 0 if option arg doesn't match any switch. 
	
	rcg: No idea if this fully implements getopt()'s API;
		just provided so main() would compile.
	*/
int optind;
char *optarg;

int
getopt(argc, argv, pszSwitches)
int argc;
char* argv[];
char* pszSwitches;
{
	static int n = 1;
	char* pszToken;
	char*  arg = "- ";
	char* szSwitches;

	
	if(n >= argc || argv[n][0] != '-')
	{
		optind = n;
		n = 1;
		return EOF;
	}
	szSwitches = _strdup(pszSwitches);
	if(szSwitches == NULL)
		return EOF;

	/* Compare argv[n] with acceptable switches */
	pszToken = strtok(szSwitches, ":");
	if(pszToken == NULL)
	{
		free(szSwitches);
		return EOF;
	}

	while(pszToken)
	{
		if(strlen(argv[n]) > 1 && argv[n][0] == '-')
		{
			arg[1] = pszToken[0];
			if(memcmp(arg, argv[n], 2) == 0)
			{
				optind = n++;
				optarg = argv[n++];
				free(szSwitches);
				return arg[1];
			}
		}
		pszToken = strtok(NULL, ":");
	}
	n++;
	free(szSwitches);
	return 0;
}

#endif // WIN32

typedef	unsigned char	Pixel;
/* Note: if you define Pixel to something bigger than char, 
		you may need to add more support in bitmap file I/O functions.
*/

typedef struct
{
	int	xsize;		/* horizontal size of the image in Pixels */
	int	ysize;		/* vertical size of the image in Pixels */
	Pixel *	data;		/* pointer to first scanline of image */
	int	span;		/* byte offset between two scanlines */
} Image;

#define	WHITE_PIXEL	(255)
#define	BLACK_PIXEL	(0)

/*
 *	generic image access and i/o support routines
 */

static char *
next_token(f)
FILE *f;
{
	static char delim[] = " \t\r\n";
	static char *t = NULL;
	static char lnbuf[256];
	char *p;

	while(t == NULL) {			/* nothing in the buffer */
		if(fgets(lnbuf, sizeof(lnbuf), f)) {	/* read a line */
			if(p = strchr(lnbuf, '#')) {	/* clip any comment */
				*p = '\0';
			}
			t = strtok(lnbuf, delim);	/* get first token */
		} else {
			return(NULL);
		}
	}
	p = t;
	t = strtok((char *)NULL, delim);		/* get next token */
	return(p);
}



Pixel
get_pixel(image, x, y)
Image *image;
int x, y;
{
	static Image *im = NULL;
	static int yy = -1;
	static Pixel *p = NULL;

	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) {
		return(0);
	}
	if((im != image) || (yy != y)) {
		im = image;
		yy = y;
		p = image->data + (y * image->span);
	}
	return(p[x]);
}

void
get_row(row, image, y)
Pixel *row;
Image *image;
int y;
{
	if((y < 0) || (y >= image->ysize)) {
		return;
	}
	memcpy(row,
		image->data + (y * image->span),
		(sizeof(Pixel) * image->xsize));
}

void
get_column(column, image, x)
Pixel *column;
Image *image;
int x;
{
	int i, d;
	Pixel *p;

	if((x < 0) || (x >= image->xsize)) {
		return;
	}
	d = image->span;
	for(i = image->ysize, p = image->data + x; i-- > 0; p += d) {
		*column++ = *p;
	}
}

Pixel
put_pixel(image, x, y, data)
Image *image;
int x, y;
Pixel data;
{
	static Image *im = NULL;
	static int yy = -1;
	static Pixel *p = NULL;

	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) {
		return(0);
	}
	if((im != image) || (yy != y)) {
		im = image;
		yy = y;
		p = image->data + (y * image->span);
	}
	return(p[x] = data);
}

Image *
new_image(xsize, ysize)	/* create a blank image */
int xsize, ysize;
{
	Image *image;
	ASSERT(xsize > 0 && ysize > 0);

	if(image = (Image *)malloc(sizeof(Image)))
	{
		if(image->data = (Pixel *)calloc(ysize, xsize))
		{
			image->xsize = xsize;
			image->ysize = ysize;
			image->span = xsize;
		}
		else
		{
			free(image);
			image = NULL;
		}
	}
	return image;
}

void
free_image(image)
Image *image;
{
	ASSERT(image && image->data);
	free(image->data);
	free(image);
}


/*
	fileextension()

	Returns type of file based on its name.
	Returns the empty string if filename has no extension 
*/
char*
fileextension(f)
char* f;
{
	/* Get a filename's extension string. */
	int i;
	for(i = strlen(f) - 1; i > 0; i--)
	{
		if(f[i] == '.')
			return f + i + 1;
	}
	/* No extension. Return the end of the string, 
	   which effectively returns a null string. */
	return f + strlen(f);
}




/* add your filetype loaders and savers here */

/* -- PXM bitmap support------------------------------------------------ */

Image *
load_image_bm(f)		/* read image from bm file */
FILE *f;
{
	char *p;
	int width, height;
	Image *image;

	if(((p = next_token(f)) && (strcmp(p, "Bm") == 0))
	&& ((p = next_token(f)) && ((width = atoi(p)) > 0))
	&& ((p = next_token(f)) && ((height = atoi(p)) > 0))
	&& ((p = next_token(f)) && (strcmp(p, "8") == 0))
	&& (image = new_image(width, height))
	&& (fread(image->data, (size_t)width, (size_t)height, f) == (size_t)height)) {
		return(image);		/* load successful */
	} else {
		return(NULL);		/* load failed */
	}
}

int
save_image_bm(f, image)	/* write image to bm file */
FILE *f;
Image *image;
{
	fprintf(f, "Bm # PXM 8-bit greyscale image\x0A");
	fprintf(f, "%d %d 8 # width height depth\x0A",
		image->xsize, image->ysize);
	if(fwrite(image->data, (size_t)image->xsize, (size_t)image->ysize, f) == (size_t)image->ysize) {
		return(0);		/* save successful */
	} else {
		return(-1);		/* save failed */
	}
}



/* -- TGA bitmap support------------------------------------------------ */

typedef struct
{
	unsigned char	lsb, msb;
} DoubleByte;

#define INT_TO_DB(n, db)	{ (db).lsb = (unsigned char)((n) & 0xFF);\
							  (db).msb = (unsigned char)((n) >> 8); }

#define DB_TO_INT(db)		(((int)((db).msb) << 8) + (db).lsb)

typedef struct
{
	unsigned char	IDfieldLen;
	unsigned char	colorMapType;
	unsigned char	imageType;
	DoubleByte		ColorMapOrigin;
	DoubleByte		ColorMapLen;
	unsigned char	ColorMapEntrySize; /* no. of bits */
	DoubleByte		Xorigin;
	DoubleByte		Yorigin;
	DoubleByte		Width;
	DoubleByte		Height;
	unsigned char	bpp;
	unsigned char	descriptor;
} TGAheader;


int
save_image_tga(f, image)	/* save image to TGA file */
FILE* f;
Image* image;
{
	int x, y;
	int n, j;
	TGAheader	header;
	boolean bOK = TRUE; /* assume success */

	/* Grayscale images only. */
	ASSERT(sizeof(Pixel) == 1);
		
	/* Write header */
	memset(&header, 0, sizeof(header));
	header.colorMapType = 1;
	header.imageType = 1;
	INT_TO_DB(256, header.ColorMapLen);
	header.ColorMapEntrySize = 24;
	INT_TO_DB(image->xsize, header.Width);
	INT_TO_DB(image->ysize, header.Height);
	header.bpp = 8;
	header.descriptor = 0x20; /* top down */

	bOK = (fwrite(&header, sizeof(header), 1, f) == 1);  

	/* Write palette. */
	for(n = 0; n < 256 && bOK; n++)
	{
		for(j = 0; j < 3 && bOK; j++)
			bOK = fputc(n, f) != EOF;
	}
		
	/* Write pixel index values. */
	for(y = 0; y < image->ysize && bOK; y++)
	{
		for(x = 0; x < image->xsize && bOK; x++)
			bOK = (EOF != fputc(get_pixel(image, x, y), f));
	}
	return bOK ? 0 : -1;
} /* save_image_tga */



Image *
load_image_tga(f)		/* read image from TGA file */
FILE *f;
{
	Image* image = NULL;
	boolean bTopDown;
	boolean bOK = FALSE;
	TGAheader	header;
	int	width, height, c, x, y, yy;

	/* Grayscale images only. */
	ASSERT(sizeof(Pixel) == 1);

	if(fread(&header, sizeof(header), 1, f) == 1 && header.bpp == 8)
	{
		bTopDown = (header.descriptor & 0x20);
		width = DB_TO_INT(header.Width);
		height = DB_TO_INT(header.Height);

		image = new_image(width, height);
		if(image != NULL)
		{
			/* Skip palette */
			bOK = 0 == fseek(f, DB_TO_INT(header.ColorMapLen) * header.ColorMapEntrySize/8, SEEK_CUR);

			/* Read pixels */
			for(y = 0; y < height && bOK; y++)
			{
				yy = bTopDown ? y : (height-1) - y;

				for(x = 0; x < width && bOK; x++)
				{
					bOK = ((c = fgetc(f)) != EOF);
					if(bOK)
						put_pixel(image, x, yy, (Pixel)c);
				}
			}
		}
	}
	if(!bOK && image != NULL)
	{
		free_image(image);
		image = NULL;
	}

	return image;
}

/* -- End TGA bitmap support ------------------------------------------- */


/*
	ImageHandler stuff.
	An ImageHandler stores an image file's extension, 
	and pointers to functions that read and write the image format.
*/
typedef struct
{
	char* filetype;
	Image* (*reader)();
	int (*writer)();
} ImageHandler;

ImageHandler gImageHandlers[] =
{
	{ "bm", load_image_bm, save_image_bm },
	{ "tga", load_image_tga, save_image_tga }
	/* add your image handlers here */
};


/*
	find_imagehandler()

	Given a filename, return an image handler for it. 
	Return NULL if no handler available.
*/
ImageHandler*
find_imagehandler(f)
char* f;
{
	int i;
	for(i = 0; i < sizeof(gImageHandlers) / sizeof(gImageHandlers[0]); i++)
	{
		if(stricmp(gImageHandlers[i].filetype, fileextension(f)) == 0)
			return &gImageHandlers[i];
	}
	return NULL;
}


/*
	load_image()

	Given a filename, hands off to appropriate image loader.
	Returns pointer to loaded Image, NULL if it can't.
*/
Image *
load_image(f)		/* read image from file */
char *f;
{
	Image *image;
	FILE* fp;
	ImageHandler* handler;

	ASSERT(f);	

	fp = fopen(f, "rb");
	if(fp == NULL)
		return NULL;

	if(handler = find_imagehandler(f))
		image = handler->reader(fp);

	fclose(fp);
	return image;
}


/*
	save_image()

	Given a filename and an Image, hands off to appropriate image saver.
	Returns -1 on error, 0 on success.
*/
int
save_image(f, image)
char *f;
Image* image;
{
	FILE* fp;
	ImageHandler* handler;
	int nRet = -1; /* assume failure */

	ASSERT(f && image);
		
	fp = fopen(f, "wb");
	if(fp != NULL)
	{
		if(handler = find_imagehandler(f))
			nRet = handler->writer(fp, image);

		fclose(fp);
	}
	return nRet;
}




/*
 *	filter function definitions
 */

#define	filter_support		(1.0)

double
filter(t)
double t;
{
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if(t < 0.0) t = -t;
	if(t < 1.0) return((2.0 * t - 3.0) * t * t + 1.0);
	return(0.0);
}

#define	box_support		(0.5)

double
box_filter(t)
double t;
{
	if((t > -0.5) && (t <= 0.5)) return(1.0);
	return(0.0);
}

#define	triangle_support	(1.0)

double
triangle_filter(t)
double t;
{
	if(t < 0.0) t = -t;
	if(t < 1.0) return(1.0 - t);
	return(0.0);
}

#define	bell_support		(1.5)

double
bell_filter(t)		/* box (*) box (*) box */
double t;
{
	if(t < 0) t = -t;
	if(t < .5) return(.75 - (t * t));
	if(t < 1.5) {
		t = (t - 1.5);
		return(.5 * (t * t));
	}
	return(0.0);
}

#define	B_spline_support	(2.0)

double
B_spline_filter(t)	/* box (*) box (*) box (*) box */
double t;
{
	double tt;

	if(t < 0) t = -t;
	if(t < 1) {
		tt = t * t;
		return((.5 * tt * t) - tt + (2.0 / 3.0));
	} else if(t < 2) {
		t = 2 - t;
		return((1.0 / 6.0) * (t * t * t));
	}
	return(0.0);
}

double
sinc(x)
double x;
{
	x *= M_PI;
	if(x != 0) return(sin(x) / x);
	return(1.0);
}

#define	Lanczos3_support	(3.0)

double
Lanczos3_filter(t)
double t;
{
	if(t < 0) t = -t;
	if(t < 3.0) return(sinc(t) * sinc(t/3.0));
	return(0.0);
}

#define	Mitchell_support	(2.0)

#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

double
Mitchell_filter(t)
double t;
{
	double tt;

	tt = t * t;
	if(t < 0) t = -t;
	if(t < 1.0) {
		t = (((12.0 - 9.0 * B - 6.0 * C) * (t * tt))
		   + ((-18.0 + 12.0 * B + 6.0 * C) * tt)
		   + (6.0 - 2 * B));
		return(t / 6.0);
	} else if(t < 2.0) {
		t = (((-1.0 * B - 6.0 * C) * (t * tt))
		   + ((6.0 * B + 30.0 * C) * tt)
		   + ((-12.0 * B - 48.0 * C) * t)
		   + (8.0 * B + 24 * C));
		return(t / 6.0);
	}
	return(0.0);
}

/*
 *	image rescaling routine
 */

typedef struct {
	int	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	int	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
} CLIST;

CLIST	*contrib;		/* array of contribution lists */


/*
	roundcloser()

	Round an FP value to its closest int representation.
	General routine; ideally belongs in general math lib file.
*/	
int roundcloser(double d)
{
	/* Untested potential one-liner, but smacks of call overhead */
	/* return fabs(ceil(d)-d) <= 0.5 ? ceil(d) : floor(d); */

	/* Untested potential optimized ceil() usage */
/*	double cd = ceil(d);
	int ncd = (int)cd;
	if(fabs(cd - d) > 0.5)
		ncd--;
	return ncd;
*/

	/* Version that uses no function calls at all. */
	int n = (int) d;
	double diff = d - (double)n;
	if(diff < 0)
		diff = -diff;
	if(diff >= 0.5)
	{
		if(d < 0)
			n--;
		else
			n++;
	}
	return n;
} /* roundcloser */


/* 
	calc_x_contrib()
	
	Calculates the filter weights for a single target column.
	contribX->p must be freed afterwards.

	Returns -1 if error, 0 otherwise.
*/
int calc_x_contrib(contribX, xscale, fwidth, dstwidth, srcwidth, filterf, i)
CLIST* contribX;			/* Receiver of contrib info */
double xscale;				/* Horizontal zooming scale */
double fwidth;				/* Filter sampling width */
int dstwidth;				/* Target bitmap width */
int srcwidth;				/* Source bitmap width */
double (*filterf)(double);	/* Filter proc */
int i;						/* Pixel column in source bitmap being processed */
{
	double width;
	double fscale;
	double center, left, right;
	double weight;
	int j, k, n;

	if(xscale < 1.0)
	{
		/* Shrinking image */
		width = fwidth / xscale;
		fscale = 1.0 / xscale;

		contribX->n = 0;
		contribX->p = (CONTRIB *)calloc((int) (width * 2 + 1),
				sizeof(CONTRIB));
		if(contribX->p == NULL)
			return -1;

		center = (double) i / xscale;
		left = ceil(center - width);
		right = floor(center + width);
		for(j = (int)left; j <= right; ++j)
		{
			weight = center - (double) j;
			weight = (*filterf)(weight / fscale) / fscale;
			if(j < 0)
				n = -j;
			else if(j >= srcwidth)
				n = (srcwidth - j) + srcwidth - 1;
			else
				n = j;
			
			k = contribX->n++;
			contribX->p[k].pixel = n;
			contribX->p[k].weight = weight;
		}
	
	}
	else
	{
		/* Expanding image */
		contribX->n = 0;
		contribX->p = (CONTRIB *)calloc((int) (fwidth * 2 + 1),
				sizeof(CONTRIB));
		if(contribX->p == NULL)
			return -1;
		center = (double) i / xscale;
		left = ceil(center - fwidth);
		right = floor(center + fwidth);

		for(j = (int)left; j <= right; ++j)
		{
			weight = center - (double) j;
			weight = (*filterf)(weight);
			if(j < 0) {
				n = -j;
			} else if(j >= srcwidth) {
				n = (srcwidth - j) + srcwidth - 1;
			} else {
				n = j;
			}
			k = contribX->n++;
			contribX->p[k].pixel = n;
			contribX->p[k].weight = weight;
		}
	}
	return 0;
} /* calc_x_contrib */


/*
	zoom()

	Resizes bitmaps while resampling them.
	Returns -1 if error, 0 if success.
*/
int
zoom(dst, src, filterf, fwidth)
Image* dst;
Image* src;
double (*filterf)(double);
double fwidth;
{
	Pixel* tmp;
	double xscale, yscale;		/* zoom scale factors */
	int xx;
	int i, j, k;			/* loop variables */
	int n;				/* pixel number */
	double center, left, right;	/* filter calculation variables */
	double width, fscale, weight;	/* filter calculation variables */
	Pixel pel, pel2;
	int bPelDelta;
	CLIST	*contribY;		/* array of contribution lists */
	CLIST	contribX;
	int		nRet = -1;

	/* create intermediate column to hold horizontal dst column zoom */
	tmp = (Pixel*)malloc(src->ysize * sizeof(Pixel));
	if(tmp == NULL)
		return 0;

	xscale = (double) dst->xsize / (double) src->xsize;

	/* Build y weights */
	/* pre-calculate filter contributions for a column */
	contribY = (CLIST *)calloc(dst->ysize, sizeof(CLIST));
	if(contribY == NULL)
	{
		free(tmp);
		return -1;
	}

	yscale = (double) dst->ysize / (double) src->ysize;

	if(yscale < 1.0)
	{
		width = fwidth / yscale;
		fscale = 1.0 / yscale;
		for(i = 0; i < dst->ysize; ++i)
		{
			contribY[i].n = 0;
			contribY[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			if(contribY[i].p == NULL)
			{
				free(tmp);
				free(contribY);
				return -1;
			}
			center = (double) i / yscale;
			left = ceil(center - width);
			right = floor(center + width);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= src->ysize) {
					n = (src->ysize - j) + src->ysize - 1;
				} else {
					n = j;
				}
				k = contribY[i].n++;
				contribY[i].p[k].pixel = n;
				contribY[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->ysize; ++i) {
			contribY[i].n = 0;
			contribY[i].p = (CONTRIB *)calloc((int) (fwidth * 2 + 1),
					sizeof(CONTRIB));
			if(contribY[i].p == NULL)
			{
				free(tmp);
				free(contribY);
				return -1;
			}
			center = (double) i / yscale;
			left = ceil(center - fwidth);
			right = floor(center + fwidth);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= src->ysize) {
					n = (src->ysize - j) + src->ysize - 1;
				} else {
					n = j;
				}
				k = contribY[i].n++;
				contribY[i].p[k].pixel = n;
				contribY[i].p[k].weight = weight;
			}
		}
	}


	for(xx = 0; xx < dst->xsize; xx++)
	{
		if(0 != calc_x_contrib(&contribX, xscale, fwidth, 
								dst->xsize, src->xsize, filterf, xx))
		{
			goto __zoom_cleanup;
		}
		/* Apply horz filter to make dst column in tmp. */
		for(k = 0; k < src->ysize; ++k)
		{
			weight = 0.0;
			bPelDelta = FALSE;
			pel = get_pixel(src, contribX.p[0].pixel, k);
			for(j = 0; j < contribX.n; ++j)
			{
				pel2 = get_pixel(src, contribX.p[j].pixel, k);
				if(pel2 != pel)
					bPelDelta = TRUE;
				weight += pel2 * contribX.p[j].weight;
			}
			weight = bPelDelta ? roundcloser(weight) : pel;

			tmp[k] = (Pixel)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL);
		} /* next row in temp column */

		free(contribX.p);

		/* The temp column has been built. Now stretch it 
		 vertically into dst column. */
		for(i = 0; i < dst->ysize; ++i)
		{
			weight = 0.0;
			bPelDelta = FALSE;
			pel = tmp[contribY[i].p[0].pixel];

			for(j = 0; j < contribY[i].n; ++j)
			{
				pel2 = tmp[contribY[i].p[j].pixel];
				if(pel2 != pel)
					bPelDelta = TRUE;
				weight += pel2 * contribY[i].p[j].weight;
			}
			weight = bPelDelta ? roundcloser(weight) : pel;
			put_pixel(dst, xx, i,
				(Pixel)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL));
		} /* next dst row */
	} /* next dst column */
	nRet = 0; /* success */

__zoom_cleanup:
	free(tmp);

	/* free the memory allocated for vertical filter weights */
	for(i = 0; i < dst->ysize; ++i)
		free(contribY[i].p);
	free(contribY);

	return nRet;
} /* zoom */




/*
 *	command line interface
 */

void
usage()
{
	fprintf(stderr, "usage: %s [-options] input output\n", _Program);
	fprintf(stderr, "\
options:\n\
	-x xsize		output x size\n\
	-y ysize		output y size\n\
	-f filter		filter type\n\
{b=box, t=triangle, q=bell, B=B-spline, h=hermite, l=Lanczos3, m=Mitchell}\n\
	input, output	files to read/write. Use BM or TGA extension.\n\
");
	exit(1);
}

void
banner()
{
	printf("%s v%s -- %s\n", _Program, _Version, _Copyright);
}

int
main(argc, argv)
int argc;
char *argv[];
{
	register int c;
#ifndef WIN32
	extern int optind;
	extern char *optarg;
#endif
	int xsize = 0, ysize = 0;
	double (*f)() = filter;
	double s = filter_support;
	char *dstfile, *srcfile;
	Image *dst, *src;

	while((c = getopt(argc, argv, "x:y:f:V")) != EOF) {
		switch(c) {
		case 'x': xsize = atoi(optarg); break;
		case 'y': ysize = atoi(optarg); break;
		case 'f':
			switch(*optarg) {
			case 'b': f=box_filter; s=box_support; break;
			case 't': f=triangle_filter; s=triangle_support; break;
			case 'q': f=bell_filter; s=bell_support; break;
			case 'B': f=B_spline_filter; s=B_spline_support; break;
			case 'h': f=filter; s=filter_support; break;
			case 'l': f=Lanczos3_filter; s=Lanczos3_support; break;
			case 'm': f=Mitchell_filter; s=Mitchell_support; break;
			default: usage();
			}
			break;
		case 'V': banner(); exit(EXIT_SUCCESS);
		case '?': usage();
		default:  usage();
		}
	}

	if((argc - optind) != 2) usage();
	srcfile = argv[optind];
	dstfile = argv[optind + 1];

	if((src = load_image(srcfile)) == NULL)
	{
		fprintf(stderr, "%s: can't load source image '%s'\n",
			_Program, srcfile);
		exit(EXIT_FAILURE);
	}

	if(xsize <= 0) xsize = src->xsize;
	if(ysize <= 0) ysize = src->ysize;

	dst = new_image(xsize, ysize);

	if(zoom(dst, src, f, s) != 0)
	{
		fprintf(stderr, "%s: can't process image '%s'\n", 
			_Program, srcfile);
		exit(EXIT_FAILURE);
	}
	else
	{
		if(save_image(dstfile, dst) != 0)
		{
			fprintf(stderr, "%s: can't save destination image '%s'\n",
				_Program, dstfile);
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
	return 0;
}

