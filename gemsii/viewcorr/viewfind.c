/* 
 * viewfind.c - Simple program to read view and screen data, then run view
 * correlation on it, and dump the results suitable for a raytracing program.
 * 
 * Author:	Rod G. Bogart
 * Date:	Oct 15 1990
 * Copyright (c) 1990, University of Michigan
 * 
 */
#include <stdio.h>
#include <math.h>
#include "GraphicsGems.h"
#include "matrix.h"
#include "viewcorr.h"

main(argc, argv)
char **argv;
{
    ViewData datapts;
    ViewParms view_parms;
    int num_iterations = 0;

    if (argc >= 2) 
	num_iterations = atoi(argv[1]);
    read_points_and_view(stdin, &datapts, &view_parms );
    iterate_view_parms( &datapts, &view_parms, num_iterations );
    dump_points_and_view(stdout, &datapts, &view_parms );
    dump_rayshade_parms(stdout, &view_parms);
}

read_points_and_view( infile, datapts, view_parms )
FILE * infile;
ViewData *datapts;
ViewParms *view_parms;
{
    int i;
    Matrix3 *vm;
    Point3 lookp, up;
    Point3 xvec, yvec, zvec;

    /* read viewparms first, then data points */

    fscanf(infile,"%lf %lf %lf",&(view_parms->eye.x),&(view_parms->eye.y),
	   &(view_parms->eye.z));
    fscanf(infile,"%lf %lf %lf",&(lookp.x),&(lookp.y),&(lookp.z));
    fscanf(infile,"%lf %lf %lf",&(up.x),&(up.y),&(up.z));

    /* make coordinate frame unit vectors from eye, lookp, and up */
    V3Sub(&view_parms->eye, &lookp, &zvec);
    V3Normalize( &zvec );
    V3Normalize( &up );
    V3Cross(&up, &zvec, &xvec);
    V3Cross(&zvec, &xvec, &yvec);
    V3Normalize( &xvec );
    V3Normalize( &yvec );

    /* Store the coordinate frame unit vectors as columns to create
     * a rotation matrix
     */
    vm = &(view_parms->view);
    vm->element[0][0] = xvec.x;
    vm->element[0][1] = yvec.x;
    vm->element[0][2] = zvec.x;
    vm->element[1][0] = xvec.y;
    vm->element[1][1] = yvec.y;
    vm->element[1][2] = zvec.y;
    vm->element[2][0] = xvec.z;
    vm->element[2][1] = yvec.z;
    vm->element[2][2] = zvec.z;

    propagate_rotate_change( view_parms );

    fscanf(infile,"%lf %lf",&(view_parms->d_over_s),&(view_parms->aspect));
    fscanf(infile,"%lf %lf %lf %lf",&(view_parms->halfx),&(view_parms->halfy),
	   &(view_parms->xcenter),&(view_parms->ycenter));

    fscanf(infile,"%d",&datapts->numpts);
    datapts->pts = (Point3 *) malloc(datapts->numpts * sizeof(Point3));
    datapts->scrpts = (Point2 *) malloc(datapts->numpts * sizeof(Point2));
    for(i=0; i < datapts->numpts; i++)
    {
	fscanf(infile,"%lf %lf %lf",&datapts->pts[i].x,&datapts->pts[i].y,
	       &datapts->pts[i].z);
	fscanf(infile,"%lf %lf",&datapts->scrpts[i].x,&datapts->scrpts[i].y);
    }
}

dump_points_and_view( dumpfile, datapts, view_parms )
FILE * dumpfile;
ViewData *datapts;
ViewParms *view_parms;
{
    int i;
    Point3 dov, tmp, up;

    tmp.x = 0.0;
    tmp.y = 0.0;
    tmp.z = -1.0;
    V3MulPointByMatrix(&tmp, &view_parms->viewinv, &dov);
    tmp.x = 0.0;
    tmp.y = 1.0;
    tmp.z = 0.0;
    V3MulPointByMatrix(&tmp, &view_parms->viewinv, &up);

    fprintf(dumpfile,"%lf %lf %lf\n",view_parms->eye.x,view_parms->eye.y,
	   view_parms->eye.z);

    fprintf(dumpfile,"%lf %lf %lf\n",
	    view_parms->eye.x + dov.x, 
	    view_parms->eye.y + dov.y, 
	    view_parms->eye.z + dov.z);
    fprintf(dumpfile,"%lf %lf %lf\n", up.x, up.y, up.z);

    fprintf(dumpfile,"%lf %lf\n",view_parms->d_over_s,view_parms->aspect);
    fprintf(dumpfile,"%lf %lf %lf %lf\n",view_parms->halfx, view_parms->halfy,
	    view_parms->xcenter, view_parms->ycenter);

    fprintf(dumpfile,"%d\n",datapts->numpts);
    for(i=0; i < datapts->numpts; i++)
    {
	fprintf(dumpfile,"%lf %lf %lf ",datapts->pts[i].x,datapts->pts[i].y,
		datapts->pts[i].z);
	fprintf(dumpfile,"%lf %lf\n",datapts->scrpts[i].x,
		datapts->scrpts[i].y);
    }
}

dump_rayshade_parms( dumpfile, view_parms )
FILE * dumpfile;
ViewParms *view_parms;
{
    double ds;
    int halfx, halfy;
    Point3 dov, tmp, up;

    tmp.x = 0.0;
    tmp.y = 0.0;
    tmp.z = -1.0;
    V3MulPointByMatrix(&tmp, &view_parms->viewinv, &dov);
    tmp.x = 0.0;
    tmp.y = 1.0;
    tmp.z = 0.0;
    V3MulPointByMatrix(&tmp, &view_parms->viewinv, &up);

    if (view_parms->halfx > view_parms->xcenter) {
	halfx = (int) (2.0*view_parms->halfx - view_parms->xcenter);
    }
    else
	halfx = view_parms->xcenter;
    if (view_parms->halfy > view_parms->ycenter) {
	halfy = (int) (2.0*view_parms->halfy - view_parms->ycenter);
    }
    else
	halfy = view_parms->ycenter;
    ds = view_parms->d_over_s;
    if (ds < 0.0) {
	V3Negate(&up);
	ds = -ds;
    }
    fprintf(dumpfile,"screen %d %d\n", halfx * 2, halfy * 2);
    fprintf(dumpfile,"window %d %d %d %d\n",
	    (int) (halfx - view_parms->xcenter),
	    (int) (halfy - view_parms->ycenter),
	    (int) (halfx - view_parms->xcenter) + 
	    (int) (view_parms->halfx*2 - 1),
	    (int) (halfy - view_parms->ycenter) + 
	    (int) (view_parms->halfy*2 - 1));
    fprintf(dumpfile,"eyep %lf %lf %lf\n", view_parms->eye.x, 
	    view_parms->eye.y, view_parms->eye.z);
    fprintf(dumpfile,"lookp %lf %lf %lf\n",
	    view_parms->eye.x + dov.x, 
	    view_parms->eye.y + dov.y, 
	    view_parms->eye.z + dov.z);
    fprintf(dumpfile,"up %lf %lf %lf\n", up.x, up.y, up.z);
    fprintf(dumpfile,"fov %lf %lf\n",
	    atan(((double) halfx / view_parms->halfx) * 
		 (1.0/ds)) * RTOD * 2.0,
	    atan(((double) halfy / view_parms->halfx) * 
		 (1.0/(ds * view_parms->aspect))) * RTOD * 2.0);
}

