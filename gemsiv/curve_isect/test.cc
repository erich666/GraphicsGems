#include "Bezier.h"
#include <stdio.h>

#define H 1200.0
#define STEPS 3
main()
    {
    point Origin = point( 0, 0 );
    point p0 = Origin;
    point p1 = Origin + vector( H, H/2 );
    point p2 = Origin + vector( -7*H/10,  2*H/10 );
    point p3 = Origin + vector(  4*H/10,  0 );
    point q0 = Origin + vector( 5*H/100,  0 );
    point q1 = Origin + vector( 35*H/100, H );
    point q2 = Origin + vector(  25*H/100, -7*H/10 );
    point q3 = Origin + vector(  0, 4*H/10 );
    puts( "%!\n0 setlinewidth\n" );
    puts( "0 1 0 setrgbcolor\n" );
    int rg = 0;
    Bezier A = Bezier( &p0, &p1, &p2, &p3 );
    Bezier B = Bezier( &q0, &q1, &q2, &q3 );
    printf( "%g %g scale\n", 72 * 8.0 / H , 72 * 10.0/H );
    printf( "%g %g translate\n", H*0.1, H*0.1 );
    printf( "/rad 5 def\n" );
    Bezier **curves = B.Intersect( A );
    for( int i = 0; i < 10; i++ )
	{
	if( curves[0][i].p0 == NULL )
	    {
	    break;
	    }
	printf( "%d %d 0 setrgbcolor\n", rg, 1-rg );
	rg = 1-rg;
	printf( "%15.9f %15.9f moveto\n", 
	    curves[0][i].p0->x, curves[0][i].p0->y );
	printf( "%15.9f %15.9f %15.9f %15.9f %15.9f %15.9f curveto stroke\n",
	    curves[0][i].p1->x,  curves[0][i].p1->y,
	    curves[0][i].p2->x,  curves[0][i].p2->y,  
	    curves[0][i].p3->x,  curves[0][i].p3->y );
	printf( "%15.9f %15.9f moveto\n", 
	    curves[1][i].p0->x, curves[1][i].p0->y );
	printf( "%15.9f %15.9f %15.9f %15.9f %15.9f %15.9f curveto stroke\n",
	    curves[1][i].p1->x,  curves[1][i].p1->y,
	    curves[1][i].p2->x,  curves[1][i].p2->y,  
	    curves[1][i].p3->x,  curves[1][i].p3->y );
	}
    puts( "showpage\n" );
    
#   ifdef PROFILE
    for( i = 0; i < STEPS; i++ )
	{
	p0 = p0 + vector(  H/STEPS, H/STEPS );
	for( int j = 0; j < STEPS; j++ )
	    {
	    p1 = p1 + vector( -5*H/STEPS, H/(3*STEPS) );
	    for( int k = 0; k < STEPS; k++ )
		{
		p2 = p2 + vector( 5*H/STEPS, -H/(3*STEPS) );
		for( int l = 0; l < STEPS; l++ )
		    {
		    p3 = p3 + vector(  -H/STEPS, -H/STEPS );
		    for( int m = 0; m < STEPS; m++ )
			{
			q0 = q0 + vector( H/STEPS, H/STEPS );
			for( int n = 0; n < STEPS; n++ )
			    {
			    q1 = q1 + vector( H/(3*STEPS), -2*H/STEPS );
			    for( int o = 0; o < STEPS; o++ )
				{
				q2 = q2 + vector( -H/(3*STEPS), -2*H/STEPS );
				curves = A.Intersect( B );
				}
			    }
			}
		    }
		}
	    }
	}
#   endif
    }
