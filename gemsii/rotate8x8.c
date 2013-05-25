/*
**	Rotate an 8x8 tile clockwise by table lookup
**	and write to destination directly.
**	Large bitmaps can be rotated an 8x8 tile at a time.
**	The extraction is done a nybble at a time to reduce the
**	size of the tables.
**
**	Input parameters:
**	src		starting address of source 8x8 tile
**	srcstep		difference in byte address between
**			adjacent rows in source bitmap
**	dst		starting address of destination 8x8 tile
**	dststep		difference in byte address between
**			adjacent rows in destination bitmap
**
**	Ken Yap (Centre for Spatial Information Systems, CSIRO DIT, Australia)
**	after an idea suggested by Alan Paeth (U of Waterloo).
*/

typedef	long	bit32;

#define	table(name,n)\
	static bit32 name[16] =\
	{\
		0x00000000<<n, 0x00000001<<n, 0x00000100<<n, 0x00000101<<n,\
		0x00010000<<n, 0x00010001<<n, 0x00010100<<n, 0x00010101<<n,\
		0x01000000<<n, 0x01000001<<n, 0x01000100<<n, 0x01000101<<n,\
		0x01010000<<n, 0x01010001<<n, 0x01010100<<n, 0x01010101<<n,\
	};

table(ltab0,7)
table(ltab1,6)
table(ltab2,5)
table(ltab3,4)
table(ltab4,3)
table(ltab5,2)
table(ltab6,1)
table(ltab7,0)

void rotate8x8(src, srcstep, dst, dststep)
	unsigned char	*src, *dst;
	int		srcstep, dststep;
{
	register unsigned char	*p;
	register int	pstep, lownyb, hinyb;
	register bit32	low, hi;

	low = hi = 0;

#define	extract(d,t)\
	lownyb = *d & 0xf; hinyb = *d >> 4;\
	low |= t[lownyb]; hi |= t[hinyb]; d += pstep;

	p = src; pstep = srcstep;
	extract(p,ltab0) extract(p,ltab1) extract(p,ltab2) extract(p,ltab3)
	extract(p,ltab4) extract(p,ltab5) extract(p,ltab6) extract(p,ltab7)

#define	unpack(d,w)\
	*d = w & 0xff;		d += pstep;\
	*d = (w >> 8) & 0xff;	d += pstep;\
	*d = (w >> 16) & 0xff;	d += pstep;\
	*d = (w >> 24) & 0xff;

	p = dst; pstep = dststep;
	unpack(p,low) p += pstep; unpack(p,hi)
}












