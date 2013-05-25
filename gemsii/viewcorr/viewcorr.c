/* 
 * viewcorr.c - Iterate the view parameters.
 *
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

iterate_view_parms( datapts, view_parms, num_iterations )
ViewData *datapts;
ViewParms *view_parms;
int num_iterations;
{
    Matrix errors, jacobian, corrections;
    int i,j;
    double rootmeansqr, last_rootmeansqr;

    /* allocate Matrix stuff */
    errors = NewMatrix(datapts->numpts*2, 1);
    jacobian = NewMatrix(datapts->numpts*2, NUM_VIEW_PARMS);
    corrections = NewMatrix(NUM_VIEW_PARMS, 1);

    if (num_iterations <= 0) {
	num_iterations = 10000;
    }
    last_rootmeansqr = 0.0;
    for (i = 0; i < num_iterations; i++)
    {
	measure_errors( datapts, view_parms, errors, &rootmeansqr );
	if (ABS(rootmeansqr - last_rootmeansqr) < 1E-8) {
	    /* quit when rootmeansqr stays the same */
	    break;
	}
	last_rootmeansqr = rootmeansqr;
	if (rootmeansqr > (0.1 * view_parms->halfx)) 
	    /* When the error terms are large, the corrections become too
	     * extreme, and knock the whole thing into outer space.  Sooo,
	     * shrink the error terms, to cause the corrections to happen
	     * a small amount at a time.  Note: dividing by the rootmeansqr
	     * may be a little extreme, but it does slow down the erratic
	     * behaviour.
	     */
	    for (j = 0; j < datapts->numpts; j++)
	    {
		errors[0][j*2] /= rootmeansqr;
		errors[0][j*2+1] /= rootmeansqr;
	    }
	build_jacobian( datapts, view_parms, jacobian );
	find_corrections( datapts, jacobian, errors, corrections );
	apply_corrections( corrections, view_parms );
    }
    FreeMatrix(errors, 1);
    FreeMatrix(jacobian, NUM_VIEW_PARMS);
    FreeMatrix(corrections, 1);
}

measure_errors( datapts, view_parms, errors, rootmeansqr )
ViewData *datapts;
Matrix errors;
ViewParms *view_parms;
double *rootmeansqr;
{
    int i;
    double sqrs=0.0;
    Point2 screenpt;

    for (i = 0; i < datapts->numpts; i++)
    {
	screen_project( &datapts->pts[i], view_parms, &screenpt );
	errors[0][i*2 + 0] = screenpt.x - datapts->scrpts[i].x;
	errors[0][i*2 + 1] = screenpt.y - datapts->scrpts[i].y;
	sqrs += SQR(errors[0][i*2 + 0]) + SQR(errors[0][i*2 + 1]);
    }
    *rootmeansqr = sqrt( sqrs / (datapts->numpts*2.0) );
}

build_jacobian( datapts, view_parms, jacobian )
ViewData *datapts;
Matrix jacobian;
ViewParms *view_parms;
{
    int i;
    Point3 xyz, eR;

    /* The jacobian matrix has at least 10 columns (u and v for 5 pts) 
     * and 10 rows (10 iteration parameters).  The iteration parameters will
     * be ordered: eRx eRy eRz phi_x phi_y phi_z ds xcenter ycenter aspect
     * from the top down.
     */
    
    V3MulPointByMatrix(&view_parms->eye, &view_parms->view, &eR);
    for (i = 0; i < datapts->numpts; i++)
    {
	V3MulPointByMatrix(&datapts->pts[i], &view_parms->view, &xyz);

	store_u_partials( &xyz, &eR, view_parms, i, jacobian );
	store_v_partials( &xyz, &eR, view_parms, i, jacobian );
    }
}

store_u_partials( xyz, eR, view_parms, ptnum, jacobian )
Point3 *xyz, *eR;
Matrix jacobian;
ViewParms *view_parms;
int ptnum;
{
    double x_min_eR, z_min_eR;
    int i2;

    i2 = ptnum*2;
    x_min_eR = xyz->x - eR->x;
    z_min_eR = xyz->z - eR->z;

    jacobian[0][i2] = view_parms->d_over_s * view_parms->halfx / z_min_eR;
    jacobian[1][i2] = 0.0;
    jacobian[2][i2] = - view_parms->d_over_s * view_parms->halfx * x_min_eR
	/ SQR( z_min_eR );

    jacobian[3][i2] = view_parms->d_over_s * view_parms->halfx * xyz->y 
	* x_min_eR / SQR( z_min_eR );
    jacobian[4][i2] = - (view_parms->d_over_s * view_parms->halfx * xyz->z 
			 / z_min_eR) 
	- (view_parms->d_over_s * view_parms->halfx * xyz->x * x_min_eR
	   / SQR( z_min_eR ));
    jacobian[5][i2] = view_parms->d_over_s * view_parms->halfx * xyz->y 
	/ z_min_eR;

    jacobian[6][i2] = - view_parms->halfx * x_min_eR / z_min_eR;
    jacobian[7][i2] = 1.0;
    jacobian[8][i2] = 0.0;
#ifdef ITERATE_ASPECT_RATIO
    jacobian[9][i2] = 0.0;
#endif
}

store_v_partials( xyz, eR, view_parms, ptnum, jacobian )
Point3 *xyz, *eR;
Matrix jacobian;
ViewParms *view_parms;
int ptnum;
{
    double y_min_eR, z_min_eR, d_over_s;
    int i2;

    i2 = ptnum*2 + 1;
    y_min_eR = xyz->y - eR->y;
    z_min_eR = xyz->z - eR->z;
    d_over_s = view_parms->d_over_s * view_parms->aspect;

    jacobian[0][i2] = 0.0;
    jacobian[1][i2] = d_over_s * view_parms->halfx / z_min_eR;
    jacobian[2][i2] = - d_over_s * view_parms->halfx * y_min_eR
	/ SQR( z_min_eR );

    jacobian[3][i2] = (d_over_s * view_parms->halfx * xyz->z
		       / z_min_eR)
	+ (d_over_s * view_parms->halfx * xyz->y * y_min_eR
	   / SQR( z_min_eR ));
    jacobian[4][i2] = - d_over_s * view_parms->halfx * xyz->x
	* y_min_eR / SQR( z_min_eR );
    jacobian[5][i2] = - d_over_s * view_parms->halfx * xyz->x
	/ z_min_eR;

    jacobian[6][i2] = - view_parms->aspect * view_parms->halfx * y_min_eR
	/ z_min_eR;
    jacobian[7][i2] = 0.0;
    jacobian[8][i2] = 1.0;
#ifdef ITERATE_ASPECT_RATIO
    jacobian[9][i2] = - view_parms->d_over_s * view_parms->halfx * y_min_eR
	/ z_min_eR;
#endif
}

find_corrections( datapts, jacobian, errors, corrections )
ViewData *datapts;
Matrix jacobian, errors, corrections;
{
    Matrix jacobian_transpose, combo_inverse, error_J_transpose;
    int i;

    /* The corrections matrix is the error matrix times the inverse
     * of the Jacobian.  Since the Jacobian may not be square, the
     * pseudo-inverse is used:
     *           -1        T      T -1
     *   C = E  J   =  E  J  (J  J )
     */
    for (i = 0; i < NUM_VIEW_PARMS; i++)
	corrections[0][i] = 0.0;

    jacobian_transpose = NewMatrix(NUM_VIEW_PARMS, datapts->numpts*2);
    combo_inverse = NewMatrix(NUM_VIEW_PARMS, NUM_VIEW_PARMS);

    TransposeMatrix( jacobian, jacobian_transpose, 
		     NUM_VIEW_PARMS, datapts->numpts*2 );
    MultMatrix( jacobian, jacobian_transpose, combo_inverse, 
	       NUM_VIEW_PARMS, datapts->numpts*2, NUM_VIEW_PARMS );
    if (InvertMatrix( combo_inverse, NUM_VIEW_PARMS ) == 0.0)
    {
	fprintf(stderr,"Could not invert matrix in iteration!!!\n");
	FreeMatrix(jacobian_transpose, datapts->numpts*2);
	FreeMatrix(combo_inverse, NUM_VIEW_PARMS);
	return;
    }

    error_J_transpose = NewMatrix(NUM_VIEW_PARMS, 1);
    MultMatrix( errors, jacobian_transpose, error_J_transpose, 1, 
	        datapts->numpts*2, NUM_VIEW_PARMS );
    MultMatrix( error_J_transpose, combo_inverse, corrections, 1, 
	       NUM_VIEW_PARMS, NUM_VIEW_PARMS );
    FreeMatrix(jacobian_transpose, datapts->numpts*2);
    FreeMatrix(combo_inverse, NUM_VIEW_PARMS);
    FreeMatrix(error_J_transpose, 1);
}

apply_corrections( corrections, view_parms )
Matrix corrections;
ViewParms *view_parms;
{
    ViewParms current_parms;
    Point3 eR;
    Matrix3 inc_rotate;

    current_parms = *view_parms;

    build_rotate(&inc_rotate, 
		 -corrections[0][3], -corrections[0][4], -corrections[0][5]);
    V2MatMul(&current_parms.view, &inc_rotate, &view_parms->view);
    propagate_rotate_change( view_parms );

    V3MulPointByMatrix(&current_parms.eye, &current_parms.view, &eR);
    eR.x -= corrections[0][0];
    eR.y -= corrections[0][1];
    eR.z -= corrections[0][2];
    V3MulPointByMatrix(&eR, &view_parms->viewinv, &view_parms->eye);
    view_parms->d_over_s -= corrections[0][6];
    view_parms->xcenter -= corrections[0][7];
    view_parms->ycenter -= corrections[0][8];
#ifdef ITERATE_ASPECT_RATIO
    view_parms->aspect -= corrections[0][9];
#endif
    return;
}

propagate_rotate_change( view_parms )
ViewParms *view_parms;
{
    /* inverse is just the transpose of a rotate matrix */
    TransposeMatrix3(&view_parms->view, &view_parms->viewinv);
}

rotate_mat(m, rot_angle, pos_sin_index, neg_sin_index)
Matrix3 *m;
double rot_angle;
int pos_sin_index, neg_sin_index;
{
    double cos_theta, sin_theta;
    int i,j;

    cos_theta = cos( rot_angle );    sin_theta = sin( rot_angle );
    for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++)
	    m->element[i][j] = (i==j) ? 1.0 : 0.0;
    m->element[pos_sin_index][pos_sin_index] = cos_theta;
    m->element[neg_sin_index][neg_sin_index] = cos_theta;
    m->element[pos_sin_index][neg_sin_index] = sin_theta;
    m->element[neg_sin_index][pos_sin_index] = -sin_theta;
}

build_rotate( m, rot_x, rot_y, rot_z )
Matrix3 *m;
double rot_x, rot_y, rot_z;
{
    Matrix3 tmpmat, rotate;

    /* m = Xrotate
     * tmpmat = [m] [Yrotate]
     * m = [tmpmat] [Zrotate]
     */

    rotate_mat(m, rot_x, 1, 2);
    rotate_mat(&rotate, rot_y, 2, 0);
    V2MatMul(m, &rotate, &tmpmat);
    rotate_mat(&rotate, rot_z, 0, 1);
    V2MatMul(&tmpmat, &rotate, m);
}

screen_project(datapt, view_parms, screenpt)
Point3 *datapt;
ViewParms *view_parms;
Point2 *screenpt;
{
    Point3 xyz, data_minus_eye;

    V3Sub(datapt, &view_parms->eye, &data_minus_eye);
    V3MulPointByMatrix(&data_minus_eye, &view_parms->view, &xyz);
    screenpt->x = - (view_parms->d_over_s * view_parms->halfx * xyz.x 
		     / xyz.z) + view_parms->xcenter;
    screenpt->y = - (view_parms->aspect * view_parms->d_over_s
		     * view_parms->halfx * xyz.y 
		     / xyz.z) + view_parms->ycenter;
}

