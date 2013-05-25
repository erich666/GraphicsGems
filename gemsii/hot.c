/*
 * hot.c - Scan an image for pixels with RGB values that will give
 *	"unsafe" values of chrominance signal or composite signal
 *	amplitude when encoded into an NTSC or PAL colour signal.
 *	(This happens for certain high-intensity high-saturation colours
 *	that are rare in real scenes, but can easily be present
 *	in synthetic images.)
 *
 * 	Such pixels can be flagged so the user may then choose other
 *	colours.  Or, the offending pixels can be made "safe"
 *	in a manner that preserves hue.
 *
 *	There are two reasonable ways to make a pixel "safe":
 *	We can reduce its intensity (luminance) while leaving
 *	hue and saturation the same.  Or, we can reduce saturation
 *	while leaving hue and luminance the same.  A #define selects
 *	which strategy to use.
 *
 * Note to the user: You must add your own read_pixel() and write_pixel()
 *	routines.  You may have to modify pix_decode() and pix_encode().
 *	MAXPIX, WID, and HGT are likely to need modification.
 */

/*
 * Originally written as "ikNTSC.c" by Alan Wm Paeth,
 *	University of Waterloo, August, 1985
 * Updated by Dave Martindale, Imax Systems Corp., December 1990
 */

/*
 * Compile-time options.
 *
 * Define either NTSC or PAL as 1 to select the colour system.
 * Define the other one as zero, or leave it undefined.
 *
 * Define FLAG_HOT as 1 if you want "hot" pixels set to black
 *	to identify them.  Otherwise they will be made safe.
 *
 * Define REDUCE_SAT as 1 if you want hot pixels to be repaired by
 *	reducing their saturation.  By default, luminance is reduced.
 *
 * CHROMA_LIM is the limit (in IRE units) of the overall
 *	chrominance amplitude; it should be 50 or perhaps
 *	very slightly higher.
 * 
 * COMPOS_LIM is the maximum amplitude (in IRE units) allowed for
 *	the composite signal.  A value of 100 is the maximum
 *	monochrome white, and is always safe.  120 is the absolute
 *	limit for NTSC broadcasting, since the transmitter's carrier
 *	goes to zero with 120 IRE input signal.  Generally, 110
 *	is a good compromise - it allows somewhat brighter colours
 *	than 100, while staying safely away from the hard limit.
 */

#define	NTSC        1
#define	PAL         0
#define	FLAG_HOT    0
#define	REDUCE_SAT  0


#define	CHROMA_LIM      50.0		/* chroma amplitude limit */
#define	COMPOS_LIM      110.0		/* max IRE amplitude */

#if NTSC
/*
 * RGB to YIQ encoding matrix.
 */
double code_matrix[3][3] = {
	 0.2989,	 0.5866,	 0.1144,
	 0.5959,	-0.2741,	-0.3218,
	 0.2113,	-0.5227,	 0.3113,
};

#define	PEDESTAL	7.5		/* 7.5 IRE black pedestal */
#define	GAMMA		2.2
#endif /* NTSC */

#if PAL
/*
 * RGB to YUV encoding matrix.
 */
double code_matrix[3][3] = {
	 0.2989,	 0.5866,	 0.1144,
	-0.1473,	-0.2891,	 0.4364,
	 0.6149,	-0.5145,	-0.1004,
};

#define	PEDESTAL	0.0		/* no pedestal in PAL */
#define	GAMMA		2.8
#endif /* PAL */


#define SCALE	8192            /* scale factor: do floats with int math */
#define MAXPIX	 255            /* white value */
#define WID	 1024           /* FB dimensions */
#define HGT	 768

typedef struct {
	unsigned char	r, g, b;
} Pixel;

int	tab[3][3][MAXPIX+1];    /* multiply lookup table */
double	chroma_lim;             /* chroma limit */
double	compos_lim;             /* composite amplitude limit */
long	ichroma_lim2;           /* chroma limit squared (scaled integer) */
int	icompos_lim;            /* composite amplitude limit (scaled integer) */

double	pix_decode(), gc(), inv_gc();
int	pix_encode(), hot();

main()
{
	Pixel	p;
	int	row, col;

	build_tab();

	for (col=0; col<WID; col++) {
		for(row=0; row<HGT; row++) {
			read_pixel(row, col, &p);
			if (hot(&p)) {
				write_pixel(row, col, &p);
			}
		}
	}
}

/*
 * build_tab: Build multiply lookup table.
 *
 * For each possible pixel value, decode value into floating-point
 * intensity.  Then do gamma correction required by the video
 * standard.  Scale the result by our fixed-point scale factor.
 * Then calculate 9 lookup table entries for this pixel value.
 *
 * We also calculate floating-point and scaled integer versions
 * of our limits here.  This prevents evaluating expressions every pixel
 * when the compiler is too stupid to evaluate constant-valued
 * floating-point expressions at compile time.
 *
 * For convenience, the limits are #defined using IRE units.
 * We must convert them here into the units in which YIQ
 * are measured.  The conversion from IRE to internal units
 * depends on the pedestal level in use, since as Y goes from
 * 0 to 1, the signal goes from the pedestal level to 100 IRE.
 * Chroma is always scaled to remain consistent with Y.
 */

build_tab()
{
	register double	f;
	register int	pv;

	for (pv = 0; pv <= MAXPIX; pv++) {
		f = SCALE * gc(pix_decode(pv));
		tab[0][0][pv] = (int)(f * code_matrix[0][0] + 0.5);
		tab[0][1][pv] = (int)(f * code_matrix[0][1] + 0.5);
		tab[0][2][pv] = (int)(f * code_matrix[0][2] + 0.5);
		tab[1][0][pv] = (int)(f * code_matrix[1][0] + 0.5);
		tab[1][1][pv] = (int)(f * code_matrix[1][1] + 0.5);
		tab[1][2][pv] = (int)(f * code_matrix[1][2] + 0.5);
		tab[2][0][pv] = (int)(f * code_matrix[2][0] + 0.5);
		tab[2][1][pv] = (int)(f * code_matrix[2][1] + 0.5);
		tab[2][2][pv] = (int)(f * code_matrix[2][2] + 0.5);
	}

	chroma_lim = (double)CHROMA_LIM / (100.0 - PEDESTAL);
	compos_lim = ((double)COMPOS_LIM - PEDESTAL) / (100.0 - PEDESTAL);

	ichroma_lim2 = (int)(chroma_lim * SCALE + 0.5);
	ichroma_lim2 *= ichroma_lim2;
	icompos_lim = (int)(compos_lim * SCALE + 0.5);
}

int
hot(p)
Pixel	*p;
{
	register int	r, g, b;
	register int	y, i, q;
	register long	y2, c2;
	double		pr, pg, pb;
#if REDUCE_SAT
	double		py;
#endif
	register double	fy, fc, t, scale;
#if !FLAG_HOT
	static int	prev_r = 0, prev_g = 0, prev_b = 0;
	static int	new_r, new_g, new_b;
#endif
	extern double	pow(), hypot();

	r = p->r;
	g = p->g;
	b = p->b;

	/*
	 * Pixel decoding, gamma correction, and matrix multiplication
	 * all done by lookup table.
	 *
	 * "i" and "q" are the two chrominance components;
	 * they are I and Q for NTSC.
	 * For PAL, "i" is U (scaled B-Y) and "q" is V (scaled R-Y).
	 * Since we only care about the length of the chroma vector,
	 * not its angle, we don't care which is which.
	 */
	y = tab[0][0][r] + tab[0][1][g] + tab[0][2][b];
	i = tab[1][0][r] + tab[1][1][g] + tab[1][2][b];
	q = tab[2][0][r] + tab[2][1][g] + tab[2][2][b];

	/*
	 * Check to see if the chrominance vector is too long or the
	 * composite waveform amplitude is too large.
	 *
	 * Chrominance is too large if
	 *
	 *	sqrt(i^2, q^2)  >  chroma_lim.
	 *
	 * The composite signal amplitude is too large if
	 *
	 *	y + sqrt(i^2, q^2)  >  compos_lim.
	 *
	 * We avoid doing the sqrt by checking
	 *
	 *	i^2 + q^2  >  chroma_lim^2
	 * and
	 *	y + sqrt(i^2 + q^2)  >  compos_lim
	 *	sqrt(i^2 + q^2)  >  compos_lim - y
	 *	i^2 + q^2  >  (compos_lim - y)^2
	 *
	 */

	c2 = (long)i * i + (long)q * q;
	y2 = (long)icompos_lim - y;
	y2 *= y2;
	if (c2 <= ichroma_lim2 && c2 <= y2)	/* no problems */
		return 0;

	/*
	 * Pixel is hot, choose desired (compilation time controlled) strategy
	 */
#if FLAG_HOT
	/*
	 * Set the hot pixel to black to identify it.
	 */
	p->r = p->g = p->b = 0;
#else /* FLAG_HOT */
	/*
	 * Optimization: cache the last-computed hot pixel.
	 */
	if (r == prev_r && g == prev_g && b == prev_b) {
		p->r = new_r;
		p->g = new_g;
		p->b = new_b;
		return 1;
	}
	prev_r = r;
	prev_g = g;
	prev_b = b;

	/*
	 * Get Y and chroma amplitudes in floating point.
	 *
	 * If your C library doesn't have hypot(), just use
	 * hypot(a,b) = sqrt(a*a, b*b);
	 *
	 * Then extract linear (un-gamma-corrected) floating-point
	 * pixel RGB values.
	 */
	fy = (double)y / SCALE;
	fc = hypot((double)i / SCALE, (double)q / SCALE);

	pr = pix_decode(r);
	pg = pix_decode(g);
	pb = pix_decode(b);

	/*
	 * Reducing overall pixel intensity by scaling
	 * R, G, and B reduces Y, I, and Q by the same factor.
	 * This changes luminance but not saturation, since saturation
	 * is determined by the chroma/luminance ratio.
	 *
	 * On the other hand, by linearly interpolating between the
	 * original pixel value and a grey pixel with the same
	 * luminance (R=G=B=Y), we change saturation without
	 * affecting luminance.
	 */

#if !REDUCE_SAT
	/*
	 * Calculate a scale factor that will bring the pixel
	 * within both chroma and composite limits, if we scale
	 * luminance and chroma simultaneously.
	 *
	 * The calculated chrominance reduction applies to the
	 * gamma-corrected RGB values that are the input to
	 * the RGB-to-YIQ operation.  Multiplying the
	 * original un-gamma-corrected pixel values by
	 * the scaling factor raised to the "gamma" power
	 * is equivalent, and avoids calling gc() and inv_gc()
	 * three times each.
	 */
	scale = chroma_lim / fc;
	t = compos_lim / (fy + fc);
	if (t < scale)
		scale = t;
	scale = pow(scale, GAMMA);

	r = pix_encode(scale * pr);
	g = pix_encode(scale * pg);
	b = pix_encode(scale * pb);
#else /* REDUCE_SAT */
	/*
	 * Calculate a scale factor that will bring the pixel
	 * within both chroma and composite limits, if we scale
	 * chroma while leaving luminance unchanged.
	 *
	 * We have to interpolate gamma-corrected RGB values,
	 * so we must convert from linear to gamma-corrected
	 * before interpolation and then back to linear afterwards.
	 */
	scale = chroma_lim / fc;
	t = (compos_lim - fy) / fc;
	if (t < scale)
		scale = t;

	pr = gc(pr);
	pg = gc(pg);
	pb = gc(pb);
	py = pr * code_matrix[0][0] + pg * code_matrix[0][1]
		+ pb * code_matrix[0][2];
	r = pix_encode(inv_gc(py + scale * (pr - py)));
	g = pix_encode(inv_gc(py + scale * (pg - py)));
	b = pix_encode(inv_gc(py + scale * (pb - py)));
#endif /* REDUCE_SAT */

	p->r = new_r = r;
	p->g = new_g = g;
	p->b = new_b = b;
#endif /* FLAG_HOT */
	return 1;
}

/*
 * gc: apply the gamma correction specified for this video standard.
 * inv_gc: inverse function of gc.
 *
 * These are generally just a call to pow(), but be careful!
 * Future standards may use more complex functions.
 * (e.g. SMPTE 240M's "electro-optic transfer characteristic").
 */

double
gc(x)
double	x;
{
	extern double	pow();

	return pow(x, 1.0 / GAMMA);
}

double
inv_gc(x)
double	x;
{
	extern double	pow();

	return pow(x, GAMMA);
}

/*
 * pix_decode: decode an integer pixel value into a floating-point
 *	intensity in the range [0, 1].
 *
 * pix_encode: encode a floating-point intensity into an integer
 *	pixel value.
 *
 * The code given here assumes simple linear encoding; you must change
 * these routines if you use a different pixel encoding technique.
 */

double
pix_decode(v)
int	v;
{
	return (double)v / MAXPIX;
}

int
pix_encode(v)
double	v;
{
	return (int)(v * MAXPIX + 0.5);
}

