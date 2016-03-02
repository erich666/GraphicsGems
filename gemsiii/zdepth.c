/*
 *	Name (person to blame): Andrew Woo
 *	Gem: Article 7.1, "The Shadow Depth Map Revisited"
 *	Affiliation: Alias Research, Style! Division
 */
#include	<math.h>


/***********************************************
 Reeves' SampleShadow code using BIAS
************************************************/

#define REEVES_APPROACH {													\
	inshadow = 0;															\
	for (i = 0, s = smin; i < ns; i++, s += ds) {							\
		for (j = 0, t = tmin; j < nt; j++, t += dt) {						\
			iu = s + Rand()*js;                      /* jitter s, t */		\
			iv = t + Rand()*jt;												\
																			\
			bias = Rand() * (Bias1 - Bias0) + Bias0; /* pick bias */		\
			if (iu >= bbox->r_umin && iu <= bbox->r_umax && 				\
				iv >= bbox->r_vmin && iv <= bbox->r_vmax)					\
				if (z > depthMap[iu][iv] + bias) inshadow++;				\
		}																	\
	}																		\
}



/***********************************************
 New SampleShadow code with no BIAS and doing
 integer comparisons for depth values first.
************************************************/

#define NEW_APPROACH {														\
	register int integerZ = (int) z, intDepth;								\
	register float depthValue;												\
	inshadow = 0;															\
																			\
	for (i = 0, s = smin; i < ns; i++, s += ds) {							\
		for (j = 0, t = tmin; j < nt; j++, t += dt) {						\
			iu = (int)(s + Rand()*js);										\
			iv = (int)(t + Rand()*jt);												\
			if (iu >= bbox->r_umin && iu <= bbox->r_umax && 				\
				iv >= bbox->r_vmin && iv <= bbox->r_vmax) {					\
				/* do integer comparison first */							\
				depthValue = depthMap[iu][iv];								\
				intDepth = (int) depthValue;								\
				if ((integerZ > intDepth) || 								\
					(integerZ == intDepth && z > depthValue))				\
					inshadow++;												\
			} else {														\
				/* boundary case error */									\
				inshadow++;													\
			}																\
		}																	\
	}																		\
}



/* Start of Reeves' code in Siggraph 87 paper */

float ResFactor = 3.f;
float MinSize = 0.f;
float Bias0 = 0.3f;
float Bias1 = 0.4f;
int NumSamples = 16;
int MinSamples = 1;

#define MAPRES 				256		/* I tested the code at 256x256 */
float depthMap[MAPRES][MAPRES];

#define CLAMP(a,min,max)	(a<min?min:(a>max?max:a))
float Rand();

typedef struct {
	int r_umin, r_umax;
	int r_vmin, r_vmax;
} TextureRect;


float SampleShadow (float s, float t, float z, float sres, float tres, TextureRect* bbox)
{
	int i, j, inshadow, iu, iv, ns, nt, lu, hu, lv, hv;
	float smin, tmin, ds, dt, js, jt;
	
	/* convert to coordinates of depth map */
	sres = MAPRES * sres * ResFactor;
	tres = MAPRES * tres * ResFactor;
	if (sres < MinSize) sres = MinSize;
	if (tres < MinSize) tres = MinSize;
	s *= MAPRES; t *= MAPRES;
	
	/* cull if outside bounding box */
	lu = (int)floorf (s-sres); hu = (int)ceilf (s+sres);
	lv = (int)floorf (t-tres); hv = (int)ceilf (t+tres);
	if (lu > bbox->r_umax || hu < bbox->r_umin ||
		lv > bbox->r_vmax || hv < bbox->r_vmin) 
		return (1.0);	/* error in Reeves' code at boundary cases */
		
	/* calculate number of samples */
    if (sres*tres*4 < NumSamples) {
        ns = (int)(sres + sres + 0.5f);
        ns = CLAMP(ns, MinSamples, NumSamples);
        nt = (int)(tres + tres + 0.5f);
        nt = CLAMP(nt, MinSamples, NumSamples);
    }
    else {
        nt = (int)(sqrtf(tres*NumSamples/sres) + 0.5f);
        nt = CLAMP(nt, MinSamples, NumSamples);
        ns = (int)(((float)NumSamples)/nt + 0.5f);
        ns = CLAMP(ns, MinSamples, NumSamples);
    }
	
	/* setup jitter variables */
	ds = 2*sres/ns; dt = 2*tres/nt;
	js = ds * 0.5f; jt = dt * 0.5f;
	smin = s - sres + js; tmin = t - tres + jt;
	
	/* decide which version you want... */
#ifdef OLD_WAY
	REEVES_APPROACH;
#else
	NEW_APPROACH;
#endif

	return (((float) inshadow) / (ns*nt));
}


