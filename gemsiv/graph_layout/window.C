/***************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    MODUL: C++ X-WINDOW class library based on Xlib
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
****************************************************************************/
#include "window.hxx"

//-------------------------------------------------------------------
void App :: Error( char * message, int line )
//-------------------------------------------------------------------
{
	fprintf( stderr, "ERROR: %s", message );
	if ( line >= 0 ) fprintf( stderr, " in line %d", line );
	fprintf( stderr, "\n" );
	Quit( );
}

//-------------------------------------------------------------------
void App :: Warning( char * message )
//-------------------------------------------------------------------
{
	fprintf( stderr, "WARNING: %s\n", message );
}

//-------------------------------------------------------------------
void App :: Quit( )
//-------------------------------------------------------------------
{
	fprintf( stderr, "Bye ( Graph )\n" );
	exit( -1 );
}

//-------------------------------------------------------------------
AppWindow ::  AppWindow( int argc, char * argv[] )
//-------------------------------------------------------------------
	   : canvas( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT ),
	     beam( 0, 0 )
{
	if ( !(dpy = XOpenDisplay (""))) {
		fprintf (stderr, "Error:  Can't open display\n");
		exit (1);
	}

	screen = DefaultScreen (dpy);
	win = XCreateSimpleWindow(dpy,
				  DefaultRootWindow (dpy),
				  0, 0,
				  WINDOW_WIDTH, WINDOW_HEIGHT,
				  1,
				  WhitePixel (dpy, screen),
				  BlackPixel (dpy, screen));

	XSelectInput (dpy, win, StructureNotifyMask |
				ExposureMask |
				KeyPressMask );

	XStoreName (dpy, win, argv[0] );
	XMapWindow (dpy, win);
/*
*  White/Black graphics context
*/
	gc = XCreateGC (dpy, win, 0L, (XGCValues *) 0);
	XSetBackground (dpy, gc, BlackPixel (dpy, screen));
	XSetForeground (dpy, gc, WhitePixel (dpy, screen));
/*
*  Black/White graphics context
*/
	gc_inv = XCreateGC (dpy, win, 0L, (XGCValues *) 0);
	XSetForeground (dpy, gc_inv, BlackPixel (dpy, screen));
	XSetBackground (dpy, gc_inv, WhitePixel (dpy, screen));
}

//-------------------------------------------------------------------
void AppWindow :: MessageLoop()
//-------------------------------------------------------------------
{
	for ( ; ; ) {
		XEvent event;
		XNextEvent (dpy, &event);

		switch (event.type) {
		case ConfigureNotify:
			canvas = RectAngle( 0, 0, event.xconfigure.width, event.xconfigure.height );
			break;
		case Expose:
			{
				XClearWindow(dpy, win);
				ExposeEvent evt( &event.xexpose );
				ExposeAll( &evt );
			}
			break;
		case KeyPress:
			{
				KeyEvent evt( &event.xkey );
				KeyPressed( &evt );
			}
			break;
	    }
      }
}

//-------------------------------------------------------------------
RectAngle AppWindow :: Canvas()
//-------------------------------------------------------------------
{
	return canvas;
}

//-------------------------------------------------------------------
void AppWindow :: RePaint()
//-------------------------------------------------------------------
{
	XClearWindow(dpy, win);
	ExposeAll( NULL );
}

//-------------------------------------------------------------------
void AppWindow :: Text( char * text, Point p)
//-------------------------------------------------------------------
{
	XDrawString( dpy, win, gc, p.X(), p.Y(), text, strlen(text) );
}

//-------------------------------------------------------------------
void AppWindow :: MoveTo( Point p )
//-------------------------------------------------------------------
{
	beam = p;
}

//-------------------------------------------------------------------
void AppWindow :: LineTo( Point p )
//-------------------------------------------------------------------
{
	XDrawLine( dpy, win, gc, beam.X(), beam.Y(), p.X(), p.Y() );
	beam = p;
}

//-------------------------------------------------------------------
void AppWindow :: DrawRectangle( RectAngle& rect )
//-------------------------------------------------------------------
{
	XFillRectangle( dpy, win, gc,
			rect.HorPos(), rect.VerPos(),
			rect.Width(), rect.Height() );
	XFillRectangle( dpy, win, gc_inv,
			rect.HorPos() + 2, rect.VerPos() + 2,
			rect.Width() - 4, rect.Height() - 4 );
}

/************************************************************************/
/*  PROGRAM ENTRY POINT							*/
/************************************************************************/
App	app;	// Application program object

int main( int argc, char * argv[] )
{
/*
*   CALL APPLICATION DEPENDENT ENTRY
*/
	app.Start( argc, argv );
	app.Quit( );
}

