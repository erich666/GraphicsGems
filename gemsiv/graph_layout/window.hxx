/******************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    HEADER - C++ X-WINDOW class library based on Xlib
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//******************************************************************
//	CONSTANTS
//******************************************************************
#define WINDOW_WIDTH	400
#define WINDOW_HEIGHT	400
#define FALSE		0
#define TRUE		1

//******************************************************************
//	TYPES
//******************************************************************
typedef short  CoOrd;
typedef char * pchar;
typedef char   BOOL;

//===================================================================
class App {
//===================================================================
public:
	void		Start( int argc, char * argv[] );
	void		Error( char * mess, int line = -1 );
	void		Warning( char * mess );
	void		Quit( void );
};

extern App app;

//===================================================================
class Point {
//===================================================================
private:
    CoOrd x;
    CoOrd y;
public:
		    Point( )			{ x = y = 0;	    }
		    Point(CoOrd x0, CoOrd y0)	{ x = x0; y = y0;   }
    CoOrd&  X()					{ return x;	    }
    CoOrd&  Y()					{ return y;	    }
};

//===================================================================
class RectAngle {
//===================================================================
private:
    CoOrd hor_pos;
    CoOrd ver_pos;
    CoOrd width;
    CoOrd height;

public:
	    RectAngle(CoOrd hp, CoOrd vp, CoOrd w, CoOrd h)
		  { hor_pos = hp; ver_pos = vp; width = w; height = h;	}
    CoOrd&  HorPos()	{ return hor_pos;		}
    CoOrd&  VerPos()	{ return ver_pos;		}
    CoOrd&  Width()	{ return width;			}
    CoOrd&  Height()	{ return height;		}
};

//===================================================================
class KeyEvent {
//===================================================================
    KeySym		key_sym;
    XComposeStatus	status;
    char		ascii;
public:
	    KeyEvent( XKeyEvent * xe )
		{ XLookupString( xe, &ascii, 1, &key_sym, &status ); }
    int	    GetASCII( void ) { return ascii; }
};

//===================================================================
class ExposeEvent {
//===================================================================
    RectAngle  area;
public:
	       ExposeEvent( XExposeEvent * xe )
		  :area(xe->x, xe->y, xe->width, xe->height) { }
    RectAngle& GetExposedArea( )	{ return area;	}
};

//===================================================================
class AppWindow {
//===================================================================
	GC		gc, gc_inv;
	Window		win;
	Display *	dpy;
	int		screen;
	RectAngle	canvas;
	Point		beam;
protected:
	virtual void	KeyPressed( KeyEvent * ) {}
	virtual void	ExposeAll( ExposeEvent * ) {}

	RectAngle	Canvas( void );
	void		RePaint( void );
	void		Text( char * text, Point position );
	void		MoveTo( Point to );
	void		LineTo( Point to );
	void		DrawRectangle( RectAngle& rect );

public:
			AppWindow( int argc, char * argv[] );
	void		MessageLoop( );
};

