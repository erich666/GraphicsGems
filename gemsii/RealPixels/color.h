/* Copyright (c) 1986 Regents of the University of California */

/* SCCSid "@(#)color.h 1.12 7/17/91 LBL" */

/*
 *  color.h - header for routines using pixel color values.
 *
 *     12/31/85
 *
 *  Two color representations are used, one for calculation and
 *  another for storage.  Calculation is done with three floats
 *  for speed.  Stored color values use 4 bytes which contain
 *  three single byte mantissas and a common exponent.
 */

#define  RED		0
#define  GRN		1
#define  BLU		2
#define  EXP		3
#define  COLXS		128	/* excess used for exponent */

typedef unsigned char  BYTE;	/* 8-bit unsigned integer */

typedef BYTE  COLR[4];		/* red, green, blue, exponent */

#define  copycolr(c1,c2)	(c1[0]=c2[0],c1[1]=c2[1], \
				c1[2]=c2[2],c1[3]=c2[3])

typedef float  COLOR[3];	/* red, green, blue */

#define  colval(col,pri)	((col)[pri])

#define  setcolor(col,r,g,b)	((col)[RED]=(r),(col)[GRN]=(g),(col)[BLU]=(b))

#define  copycolor(c1,c2)	((c1)[0]=(c2)[0],(c1)[1]=(c2)[1],(c1)[2]=(c2)[2])

#define  scalecolor(col,sf)	((col)[0]*=(sf),(col)[1]*=(sf),(col)[2]*=(sf))

#define  addcolor(c1,c2)	((c1)[0]+=(c2)[0],(c1)[1]+=(c2)[1],(c1)[2]+=(c2)[2])

#define  multcolor(c1,c2)	((c1)[0]*=(c2)[0],(c1)[1]*=(c2)[1],(c1)[2]*=(c2)[2])

#ifdef  NTSC
#define  bright(col)		(.295*(col)[RED]+.636*(col)[GRN]+.070*(col)[BLU])
#define  normbright(c)		(int)((74L*(c)[RED]+164L*(c)[GRN]+18L*(c)[BLU])/256)
#else
#define  bright(col)		(.263*(col)[RED]+.655*(col)[GRN]+.082*(col)[BLU])
#define  normbright(c)		(int)((67L*(c)[RED]+168L*(c)[GRN]+21L*(c)[BLU])/256)
#endif

#define  luminance(col)		(470. * bright(col))

#define  intens(col)		( (col)[0] > (col)[1] \
				? (col)[0] > (col)[2] ? (col)[0] : (col)[2] \
				: (col)[1] > (col)[2] ? (col)[1] : (col)[2] )

#define  colrval(c,p)		( (c)[EXP] ? \
				ldexp((c)[p]+.5,(int)(c)[EXP]-(COLXS+8)) : \
				0. )

#define  WHTCOLOR		{1.0,1.0,1.0}
#define  BLKCOLOR		{0.0,0.0,0.0}
#define  WHTCOLR		{128,128,128,COLXS+1}
#define  BLKCOLR		{0,0,0,0}

				/* definitions for resolution header */
#define  XDECR			1
#define  YDECR			2
#define  YMAJOR			4

				/* picture format identifier */
#define  COLRFMT		"32-bit_rle_rgbe"

				/* macros for exposures */
#define  EXPOSSTR		"EXPOSURE="
#define  LEXPOSSTR		9
#define  isexpos(hl)		(!strncmp(hl,EXPOSSTR,LEXPOSSTR))
#define  exposval(hl)		atof((hl)+LEXPOSSTR)
#define  fputexpos(ex,fp)	fprintf(fp,"%s%e\n",EXPOSSTR,ex)

				/* macros for pixel aspect ratios */
#define  ASPECTSTR		"PIXASPECT="
#define  LASPECTSTR		10
#define  isaspect(hl)		(!strncmp(hl,ASPECTSTR,LASPECTSTR))
#define  aspectval(hl)		atof((hl)+LASPECTSTR)
#define  fputaspect(pa,fp)	fprintf(fp,"%s%f\n",ASPECTSTR,pa)

				/* macros for color correction */
#define  COLCORSTR		"COLORCORR="
#define  LCOLCORSTR		10
#define  iscolcor(hl)		(!strncmp(hl,COLCORSTR,LCOLCORSTR))
#define  colcorval(cc,hl)	sscanf(hl+LCOLCORSTR,"%f %f %f", \
					&(cc)[RED],&(cc)[GRN],&(cc)[BLU])
#define  fputcolcor(cc,fp)	fprintf(fp,"%s %f %f %f\n",COLCORSTR, \
					(cc)[RED],(cc)[GRN],(cc)[BLU])

extern double  ldexp(), atof();
