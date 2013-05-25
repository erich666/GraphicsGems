/*
 * C code from the article
 * "Fast Convolution with Packed Lookup Tables"
 * by George Wolberg and Henry Massalin,
 * (wolcc@cunyvm.cuny.edu and qua@microunity.com)
 * in "Graphics Gems IV", Academic Press, 1994
 *
 *	Compile: cc convolve.c -o convolve
 *	Execute: convolve in.bw kernel out.bw
 */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char	uchar;

typedef struct {		/* image data structure	 */
	int width;		/* image width	(# cols) */
	int height;		/* image height (# rows) */
	uchar *image;		/* pointer to image data */
} imageS, *imageP;

typedef struct {		/* packed lut structure	    */
	int lut0[256];		/* stage 0 for	5-pt kernel */
	int lut1[256];		/* stage 1 for 11-pt kernel */
	int lut2[256];		/* stage 2 for 17-pt kernel */
	int bias;		/* accumulated stage biases */
	int stages;		/* # of stages used: 1,2,3  */
} lutS, *lutP;

/* definitions */
#define MASK		0x3FF
#define ROUNDD		1
#define PACK(A,B,C)	(((A)<<20) + ((B)<<10) + (C))
#define INT(A)		((int) ((A)*262144+32768) >> 16)
#define CLAMP(A,L,H)	((A)<=(L) ? (L) : (A)<=(H) ? (A) : (H))
#define ABS(A)		((A) >= 0 ? (A) : -(A))

/* declarations for convolution functions */
void	convolve();
static void	initPackedLuts();
static void	fastconv();

/* declarations for image utility functions */
imageP	allocImage();
imageP	readImage();
int	saveImage();
void	freeImage();

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * main:
 *
 * Main function to collect input image and kernel values.
 * Pass them to convolve() and save result in output file.
 */
main(argc, argv)
int	  argc;
char	**argv;
{
	int	n;
	imageP	I1, I2;
	float	kernel[9];
	char	buf[80];
	FILE	*fp;

	/* make sure the user invokes this program properly */
	if(argc != 4) {
		fprintf(stderr, "Usage: convolve in.bw kernel out.bw\n");
		exit(1);
	}

	/* read input image */
	if((I1=readImage(argv[1])) == NULL) {
		fprintf(stderr, "Can't read input file %s\n", argv[1]);
		exit(1);
	}

	/* read kernel: n lines in file specify a (2n-1)-point kernel
	 * Don't exceed 9 kernel values (17-point symmetric kernel is limit)
	 */
	if((fp=fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "Can't read kernel file %s\n", argv[2]);
		exit(1);
	}
	for(n=0; n<9 && fgets(buf, 80, fp); n++) kernel[n] = atof(buf);

	/* convolve input I1 with fast convolver */
	I2 = allocImage(I1->width, I1->height);
	convolve(I1, kernel, n, I2);

	/* save output to a file */
	if(saveImage(I2, argv[3]) == NULL) {
		fprintf(stderr, "Can't save output file %s\n", argv[3]);
		exit(1);
	}
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * convolve:
 *
 * Convolve input image I1 with kernel, a (2n-1)-point symmetric filter
 * kernel containing n entries: h[i] = kernel[ |i| ] for -n < i < n.
 * Output is stored in I2.
 */
void
convolve(I1, kernel, n, I2)
imageP	 I1, I2;
float	*kernel;
int	 n;
{
	int	x, y, w, h;
	uchar	*src, *dst;
	imageP	II;
	lutS	luts;

	w = I1->width;				/* image width		*/
	h = I1->height;				/* image height		*/

	II = allocImage(w, h);			/* reserve tmp image	*/
	initPackedLuts(kernel, n, &luts);	/* init packed luts	*/

	for(y=0; y<h; y++) {			/* process all rows	*/
		src = I1->image + y*w;		/* ptr to input	 row	*/
		dst = II->image + y*w;		/* ptr to output row	*/
		fastconv(src, w, 1, &luts, dst);/* w pixels; stride=1	*/
	}

	for(x=0; x<w; x++) {			/* process all columns	*/
		src = II->image + x;		/* ptr to input	 column */
		dst = I2->image + x;		/* ptr to output column */
		fastconv(src, h, w, &luts, dst);/* h pixels; stride=w	*/
	}

	freeImage(II);				/* free temporary image */
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * initPackedLuts:
 *
 * Initialize scaled and packed lookup tables in lut.
 * Permit up to 3 cascaded stages for the following kernel sizes:
 *	stage 0:  5-point kernel
 *	stage 1: 11-point kernel
 *	stage 2: 17-point kernel
 * lut->lut0 <== packed entries (i*k2, i*k1, .5*i*k0), for i in [0, 255]
 * lut->lut1 <== packed entries (i*k5, i*k4,	i*k3), for i in [0, 255]
 * lut->lut2 <== packed entries (i*k8, i*k7,	i*k6), for i in [0, 255]
 * where k0,...k8 are taken in sequence from kernel[].
 *
 * Note that in lut0, k0 is halved since it corresponds to the center
 * pixel's kernel value and it appears in both fwd0 and rev0 (see gem).
 */
static void
initPackedLuts(kernel, n, luts)
float	*kernel;
int	 n;
lutP	 luts;
{
	int	i, k, s, *lut;
	int	b1, b2, b3;
	float	k1, k2, k3;
	float	sum;

	/* enforce flat-field response constraint: sum of kernel values = 1 */
	sum = kernel[0];
	for(i=1; i<n; i++) sum += 2*kernel[i];	/* account for symmetry */
	if(ABS(sum - 1) > .001)
		fprintf(stderr, "Warning: filter sum != 1 (=%f)\n", sum);

	/* init bias added to fields to avoid negative numbers (underflow) */
	luts->bias = 0;

	/* set up lut stages, 3 kernel values at a time */
	for(k=s=0; k<n; s++) {			/* init lut (stage s)	*/
		k1 = (k < n) ? kernel[k++] : 0;
		k2 = (k < n) ? kernel[k++] : 0;
		k3 = (k < n) ? kernel[k++] : 0;
		if(k <= 3) k1 *= .5;		/* kernel[0]: halve k0	*/

		/* select proper array in lut structure based on stage s */
		switch(s) {
		case 0: lut = luts->lut0;	break;
		case 1: lut = luts->lut1;	break;
		case 2: lut = luts->lut2;	break;
		}

		/* check k1,k2,k3 to avoid overflow in 10-bit fields */
		if(ABS(k1) + ABS(k2) + ABS(k3) > 1) {
			fprintf(stderr, "|%f|+|%f|+|%f| > 1\n", k1, k2, k3);
			exit(1);
		}

		/* compute bias for each field to avoid underflow */
		b1 = b2 = b3 = 0;
		if(k1 < 0) b1 = -k1 * 1024;
		if(k2 < 0) b2 = -k2 * 1024;
		if(k3 < 0) b3 = -k3 * 1024;

		/* luts->bias will be subtracted in convolve() after adding
		 * stages; multiply by 2 because of combined effect of fwd
		 * and rev terms
		 */
		luts->bias += 2*(b1 + b2 + b3);

		/* scale and pack kernel values in lut */
		for(i=0; i<256; i++) {
			/*
			 * INT(A) forms fixed point field:
			 * (A*(1<<18)+(1<<15)) >> 16
			 */
			lut[i] = PACK(	INT(i*k3) + b3,
					INT(i*k2) + b2 + ROUNDD,
					INT(i*k1) + b1 );
		}
	}
	luts->stages = s;
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * fastconv:
 *
 * Fast 1D convolver.
 * Convolve len input samples in src with a symmetric kernel packed in luts,
 * a lookup table that is created by initPackedLuts() from kernel values.
 * The output goes into dst.
 */
static void
fastconv(src, len, offst, luts, dst)
int	len, offst;
uchar	*src, *dst;
lutP	luts;
{
	int	 x, padlen, val, bias;
	int	 fwd0, fwd1, fwd2;
	int	 rev0, rev1, rev2;
	int	*lut0, *lut1, *lut2;
	uchar	*p1, *p2, *ip, *op;
	uchar	 buf[1024];

	/* copy and pad src into buf with padlen elements on each end */
	padlen = 3*(luts->stages) - 1;
	p1 = src;		/* pointer to row (or column) of input	*/
	p2 = buf;		/* pointer to row of padded buffer	*/
	for(x=0; x<padlen; x++) /* pad left side: replicate first pixel */
		*p2++ = *p1;
	for(x=0; x<len; x++) {	/* copy input row (or column)		*/
		*p2++ = *p1;
		 p1  +=	 offst;
	}
	p1 -= offst;		/* point to last valid input pixel	*/
	for(x=0; x<padlen; x++) /* pad right side: replicate last pixel */
		*p2++ = *p1;

	/* initialize input and output pointers, ip and op, respectively */
	ip = buf;
	op = dst;

	/* bias was added to lut entries to deal with negative kernel values */
	bias = luts->bias;

	switch(luts->stages) {
	case 1:		/* 5-pt kernel */
		lut0 = luts->lut0;

		ip  += 2;	/* ip[0] is center pixel */
		fwd0 = (lut0[ip[-2]] >> 10) + lut0[ip[-1]];
		rev0 = (lut0[ip[ 0]] << 10) + lut0[ip[ 1]];

		while(len--) {
			fwd0 = (fwd0 >> 10) + lut0[ip[0]];
			rev0 = (rev0 << 10) + lut0[ip[2]];
			val = ((fwd0 & MASK) + ((rev0 >> 20) & MASK) - bias)
				>> 2;
			*op = CLAMP(val, 0, 255);

			ip++;
			op += offst;
		}
		break;
	case 2:		/* 11-pt kernel */
		lut0 = luts->lut0;
		lut1 = luts->lut1;

		ip  += 5;	/* ip[0] is center pixel */
		fwd0 = (lut0[ip[-2]] >> 10) + lut0[ip[-1]];
		rev0 = (lut0[ip[ 0]] << 10) + lut0[ip[ 1]];

		fwd1 = (lut1[ip[-5]] >> 10) + lut1[ip[-4]];
		rev1 = (lut1[ip[ 3]] << 10) + lut1[ip[ 4]];

		while(len--) {
			fwd0 = (fwd0 >> 10) + lut0[ip[0]];
			rev0 = (rev0 << 10) + lut0[ip[2]];

			fwd1 = (fwd1 >> 10) + lut1[ip[-3]];
			rev1 = (rev1 << 10) + lut1[ip[ 5]];

			val  =	((fwd0 & MASK) + ((rev0 >> 20) & MASK)
				+(fwd1 & MASK) + ((rev1 >> 20) & MASK) - bias)
				>> 2;
			*op = CLAMP(val, 0, 255);

			ip++;
			op += offst;
		}
		break;
	case 3:		/* 17-pt kernel */
		lut0 = luts->lut0;
		lut1 = luts->lut1;
		lut2 = luts->lut2;

		ip  += 8;	/* ip[0] is center pixel */
		fwd0 = (lut0[ip[-2]] >> 10) + lut0[ip[-1]];
		rev0 = (lut0[ip[ 0]] << 10) + lut0[ip[ 1]];

		fwd1 = (lut1[ip[-5]] >> 10) + lut1[ip[-4]];
		rev1 = (lut1[ip[ 3]] << 10) + lut1[ip[ 4]];

		fwd2 = (lut2[ip[-8]] >> 10) + lut2[ip[-7]];
		rev2 = (lut2[ip[ 6]] << 10) + lut2[ip[ 7]];

		while(len--) {
			fwd0 = (fwd0 >> 10) + lut0[ip[0]];
			rev0 = (rev0 << 10) + lut0[ip[2]];

			fwd1 = (fwd1 >> 10) + lut1[ip[-3]];
			rev1 = (rev1 << 10) + lut1[ip[ 5]];

			fwd2 = (fwd2 >> 10) + lut2[ip[-6]];
			rev2 = (rev2 << 10) + lut2[ip[ 8]];

			val  =	((fwd0 & MASK) + ((rev0 >> 20) & MASK)
				+(fwd1 & MASK) + ((rev1 >> 20) & MASK)
				+(fwd2 & MASK) + ((rev2 >> 20) & MASK) - bias)
				>> 2;
			*op = CLAMP(val, 0, 255);

			ip++;
			op += offst;
		}
		break;
	}
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * readImage:
 *
 * Read an image from file.
 * Format: two integers to specify width and height, followed by uchar data.
 * Return image structure pointer.
 */
imageP
readImage(file)
char	*file;
{
	int	 sz[2];
	FILE	*fp;
	imageP	 I = NULL;

	/* open file for reading */
	if((fp = fopen(file, "r")) != NULL) {	/* open file for read	*/
		fread(sz, sizeof(int), 2, fp);	/* read image dimensions*/
		I = allocImage( sz[0],sz[1]);	/* init image structure */
		fread(I->image, sz[0],sz[1],fp);/* read data into I	*/
		fclose(fp);			/* close image file	*/
	}
	return(I);				/* return image pointer */
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * saveImage:
 *
 * Save image I into file.
 * Return NULL for failure, 1 for success.
 */
int
saveImage(I, file)
imageP	 I;
char	*file;
{
	int	 sz[2], status = NULL;
	FILE	*fp;

	if((fp = fopen(file, "w")) != NULL) {	/* open file for save	*/
		sz[0] = I->width;
		sz[1] = I->height;
		fwrite(sz, sizeof(int), 2, fp); /* write dimensions	*/
		fwrite(I->image,sz[0],sz[1],fp);/* write image data	*/
		fclose(fp);			/* close image file	*/
		status = 1;			/* register success	*/
	}
	return(status);
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * allocImage:
 *
 * Allocate space for an uchar image of width w and height h.
 * Return image structure pointer.
 */
imageP
allocImage(w, h)
int w, h;
{
	imageP	 I;

	/* allocate memory for image data structure */
	if((I = (imageP) malloc(sizeof(imageS))) != NULL) {
		I->width  = w;			/* init width		*/
		I->height = h;			/* init height		*/
		I->image  =(uchar*) malloc(w*h);/* init data pointer	*/
	}
	return(I);				/* return image pointer */
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * freeImage:
 *
 * Free image memory.
 */
void
freeImage(I)
imageP I;
{
	free((char *) I->image);
	free((char *) I);
}
