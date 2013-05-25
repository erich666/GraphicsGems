/*
 * Panoramic virtual screen implementation code fragment.
 *
 * Copyright (C) 1991, F. Kenton Musgrave 
 * All rights reserved.
 *
 * This code is an extension of Rayshade 4.0
 * Copyright (C) 1989, 1991, Craig E. Kolb, Rod G. Bogart
 * All rights reserved.
 */

/* Various declarations and vector routines are a part of Rayshade; most are
 * easy enough to do yourself.
 */

RSViewing()
{
	Float magnitude;
	RSMatrix trans;
	Vector eyeOffset;
	int x, y;

	VecSub(Camera.lookp, Camera.pos, &Camera.dir);
	Screen.firstray = Camera.dir;

	Camera.lookdist = VecNormalize(&Camera.dir);
	if (VecNormCross(&Camera.dir, &Camera.up, &Screen.scrni) == 0.)
		RLerror(RL_PANIC,
			"The view and up directions are identical?\n");
	(void)VecNormCross(&Screen.scrni, &Camera.dir, &Screen.scrnj);

	/* construct screen "x" (horizontal) direction vector */
	if (!Options.panorama) {
		/* standard virtual screen setup */
		magnitude = 2.*Camera.lookdist * tan(deg2rad(0.5*Camera.hfov)) /
					Screen.xres;
		VecScale(magnitude, Screen.scrni, &Screen.scrnx);
	} else {
		/* For panorama option, we need an array of screen "x" vectors
		 * which we will build later in the code.  At this point, we
		 * just construct the required rotation matrix (rotations being
		 * about the "up" vector) and point the "scrnx" vector to the
		 * edge of the screen.
		 */
		Camera.lookdist = 1.;
		magnitude = -deg2rad(Camera.hfov);
		Screen.hincr = magnitude / Screen.xres;
		Screen.scrnx = Camera.dir;
	        RotationMatrix( Camera.up.x, Camera.up.y, Camera.up.z,
				-0.5*magnitude, &trans );
		VecTransform( &Screen.scrnx, &trans );
	}

	/* construct screen "y" (vertical) direction vector */
	magnitude = 2.*Camera.lookdist * tan(deg2rad(0.5*Camera.vfov)) /
				Screen.yres;
	VecScale(magnitude, Screen.scrnj, &Screen.scrny);

	if (!Options.panorama) {
		/* Construct ray direction for standard virtual screen */
		Screen.firstray.x -= 0.5*(Screen.xres*Screen.scrnx.x +
				     	  Screen.yres*Screen.scrny.x);
		Screen.firstray.y -= 0.5*(Screen.xres*Screen.scrnx.y +
				     	  Screen.yres*Screen.scrny.y);
		Screen.firstray.z -= 0.5*(Screen.xres*Screen.scrnx.z +
				     	  Screen.yres*Screen.scrny.z);
	} else {
		/* Panorama option: requires that we allocate & fill horizontal
		 * and vertical direction arrays.
		 */
		Screen.horizdir = (Vector *)Malloc((Screen.xres+1) *
						   sizeof(Vector));

		/* Set eye separation for stereo rendering */
		if (Options.stereo) {
			if (Options.eyesep == UNSET)
				RLerror(RL_PANIC,
				 	 "No eye separation specified.\n");
			Screen.eyepts = (Vector *)Malloc((Screen.xres+1) *
						   	 sizeof(Vector));
			if (Options.stereo == LEFT)
				magnitude = .5 * Options.eyesep;
			else
				magnitude =  -.5 * Options.eyesep;
			eyeOffset.x = magnitude * Screen.scrni.x;
			eyeOffset.y = magnitude * Screen.scrni.y;
			eyeOffset.z = magnitude * Screen.scrni.z;
		}

		/* Fill the array of horizontal directions and, if stereo
		 * rendering, eyepoints.
		 * The horizontal ("x") direction array contains rotations of
		 * "scrnx".  Each entry requires construction of an appropriate
		 * rotation matrix; rotation again being around the "up" vector.
		 */
		for ( x=0; x<=Screen.xres; x++ ) {
			Screen.horizdir[x] = Screen.scrnx;
	        	RotationMatrix( Camera.up.x, Camera.up.y, Camera.up.z,
					x*Screen.hincr, &trans );
			VecTransform( &Screen.horizdir[x], &trans );
			/* Offset the eyepoints for stereo panorama */
			if (Options.stereo) {
				Screen.eyepts[x] = eyeOffset;
				VecTransform( &Screen.eyepts[x], &trans );
				VecAdd( Screen.eyepts[x], Camera.pos,
					&Screen.eyepts[x] );
			}
		}

		/* The vertical ("y") array varies as the tangent of "scrny". */
		Screen.vertdir = (Vector *)Malloc((Screen.yres+1) *
						  sizeof(Vector));
		for ( y=0; y<=Screen.yres; y++ ) {
			Screen.vertdir[y] = Screen.scrny;
			magnitude = 0.5*Camera.vfov -
				    Camera.vfov * ((Float)y/Screen.yres);
			magnitude = tan(deg2rad(magnitude));
			VecScale(-magnitude, Screen.scrnj, &Screen.vertdir[y]);
		}
	}

} /* RSViewing() */


SampleScreen(x, y, ray, color)
Float x, y;		/* Screen position to sample */
Ray *ray;		/* ray, with origin and medium properly set */
Pixel *color;		/* resulting color */
{
	Float dist;
	HitList hitlist;
	Color ctmp, fullintens;
	extern void ShadeRay();
	int ix, iy;

	/*
	 * Calculate ray direction.
	 */
	Stats.EyeRays++;
	if (Options.panorama) {
		/* Construct ray direction from vectors in tables, 
		 * using linear interpolation for jittering.
		 */
		ix = (int)x;
		iy = (int)y;
		if (Options.stereo)
			ray->origin = Screen.eyepts[ix];
		ray->dir.x = Screen.horizdir[ix].x +
			     (Screen.horizdir[ix+1].x - Screen.horizdir[ix].x)
			     * (x-ix) +
			     Screen.vertdir[iy].x +
			     (Screen.horizdir[iy+1].x - Screen.horizdir[iy].x)
			     * (y-iy);
		ray->dir.y = Screen.horizdir[ix].y +
			     (Screen.horizdir[ix+1].y - Screen.horizdir[ix].y)
			     * (x-ix) +
			     Screen.vertdir[iy].y +
			     (Screen.horizdir[iy+1].y - Screen.horizdir[iy].y)
			     * (y-iy);
		ray->dir.z = Screen.horizdir[ix].z +
			     (Screen.horizdir[ix+1].z - Screen.horizdir[ix].z)
			     * (x-ix) +
			     Screen.vertdir[iy].z +
			     (Screen.horizdir[iy+1].z - Screen.horizdir[iy].z)
			     * (y-iy);
	} else {
		ray->dir.x = Screen.firstray.x + x*Screen.scrnx.x +
					y*Screen.scrny.x;
		ray->dir.y = Screen.firstray.y + x*Screen.scrnx.y +
					y*Screen.scrny.y;
		ray->dir.z = Screen.firstray.z + x*Screen.scrnx.z +
					y*Screen.scrny.z;
	} 

	(void)VecNormalize(&ray->dir);

	/*
	 * Do the actual ray trace.
	 */
	fullintens.r = fullintens.g = fullintens.b = 1.;
	dist = FAR_AWAY;
	hitlist.nodes = 0;
	(void)TraceRay(ray, &hitlist, EPSILON, &dist);
	ShadeRay(&hitlist, ray, dist, &Screen.background, &ctmp, &fullintens);
	color->r = ctmp.r;
	color->g = ctmp.g;
	color->b = ctmp.b;

} /* SampleScreen() */



