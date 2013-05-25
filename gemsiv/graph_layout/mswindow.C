/***************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    MODUL: class library to interface MS-WINDOWS
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/
#include "mswindow.hxx"

static AppWindow * pwindow;

long   FAR PASCAL _export WndProc( unsigned int, unsigned int, unsigned int, long );


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
	fprintf( stderr, "ERROR: %s\n", message );
}

//-------------------------------------------------------------------
void App :: Quit( )
//-------------------------------------------------------------------
{
	fprintf( stderr, "Bye ( Graph )\n" );
	exit( -1 );
}

static HANDLE hInstance;
static HANDLE hPrevInstance;
static int    nCmdShow;

//-------------------------------------------------------------------
AppWindow ::  AppWindow( int argc, char * argv[] )
//-------------------------------------------------------------------
	   : canvas( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT )
{
	WNDCLASS wndclass;

	pwindow = this;

	strcpy(szClassName, "GRAPH");

	if ( ! hPrevInstance ) {
		wndclass.style	       = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc   = ::WndProc;
		wndclass.cbClsExtra    = 0;
		wndclass.cbWndExtra    = 0;
		wndclass.hInstance     = hInstance;
		wndclass.hIcon	       = LoadIcon( hInstance, IDI_APPLICATION );
		wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
		wndclass.hbrBackground = GetStockObject( WHITE_BRUSH );
		wndclass.lpszMenuName  = NULL;
		wndclass.lpszClassName = szClassName;

		if ( ! RegisterClass( &wndclass ) )   exit( -1 );
	}

	hWnd = CreateWindow( szClassName,		// window class name
			     "Graph layout",		// window caption
			     WS_OVERLAPPEDWINDOW,	// window style
			     CW_USEDEFAULT,		// initial x pos
			     CW_USEDEFAULT,		// initial y pos
			     CW_USEDEFAULT,		// initial x size
			     CW_USEDEFAULT,		// initial x size
			     NULL,			// parent window handle
			     NULL,			// window menu handle
			     hInstance,			// program instance handle
			     NULL );			// creation params

	if ( ! hWnd ) exit( -1 );
	ShowWindow( hWnd, nCmdShow );
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
	InvalidateRect( hWnd, NULL, TRUE );  // WM_PAINT message
}

//-------------------------------------------------------------------
void AppWindow :: Text( char * text, Point p)
//-------------------------------------------------------------------
{
	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );

	TextOut(hdc,
		p.X() - tm.tmMaxCharWidth / 2,
		p.Y() - tm.tmHeight / 2,
		text, strlen( text ));
}

//-------------------------------------------------------------------
void AppWindow :: MoveTo( Point p )
//-------------------------------------------------------------------
{
	::MoveTo(hdc, p.X(), p.Y() );
}

//-------------------------------------------------------------------
void AppWindow :: LineTo( Point p )
//-------------------------------------------------------------------
{
	::LineTo(hdc, p.X(), p.Y());
}

//-------------------------------------------------------------------
void AppWindow :: DrawRectangle( RectAngle& rect )
//-------------------------------------------------------------------
{
	RECT r;
	r.left = rect.HorPos();
	r.top = rect.VerPos();
	r.right = r.left + rect.Width();
	r.bottom = r.top + rect.Height();
	FillRect( hdc, &r, GetStockObject( BLACK_BRUSH ) );
}

//-------------------------------------------------------------------
void AppWindow :: MessageLoop()
//-------------------------------------------------------------------
{
	MSG msg;

	while( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

//-------------------------------------------------------------------
// FORWARD EVENT TO EVENT HANDLERS
//
long AppWindow::WindowProc( HWND hwnd, WORD wmsg, WORD wParam, LONG lParam )
//-------------------------------------------------------------------
{
    hWnd = hwnd;

    switch ( wmsg ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	case WM_PAINT: {				// REPAINT WINDOW
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			RECT	rect;
			GetClientRect( hWnd, &rect );
			canvas = RectAngle( rect.left, rect.top,
					    rect.right - rect.left,
					    rect.bottom - rect.top );
			ExposeEvent evt( &canvas );
			hdc = BeginPaint(hWnd, &ps);
			ExposeAll( &evt );
			EndPaint( hWnd, &ps );
	    }
	    break;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	case WM_CHAR: {					// KEYBOARD EVENT
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			KeyEvent evt(wParam, lParam);
			hdc = GetDC( hWnd );
			KeyPressed( &evt );
			ReleaseDC( hWnd, hdc );
	    }
	    break;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	case WM_DESTROY:				// CLOSE WINDOW
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    PostQuitMessage( 0 );
	    break;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    default:						// ALL OTHER EVENTS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    return DefWindowProc( hwnd, wmsg, wParam, lParam );
    }

    return 0;
}

/********************************************************************
*								    *
*  MS-WINDOWS INTERFACE						    *
********************************************************************/
//-------------------------------------------------------------------
// EVENT HANDLER, CALLED BY WINDOWS
//
long FAR PASCAL _export WndProc( unsigned int hWnd,
				 unsigned int message,
				 unsigned int wParam,
				 long lParam )
//-------------------------------------------------------------------
{
	return pwindow->WindowProc( hWnd, message, wParam, lParam );
}

//-------------------------------------------------------------------
//	APPLICATION OBJECT
//-------------------------------------------------------------------
App app;

//-------------------------------------------------------------------
// PROGRAM ENTRY POINT, CALLED BY WINDOWS
//
int PASCAL WinMain( HANDLE hInstance,
		    HANDLE hPrevInstance,
		    LPSTR lpszCmdLine,
		    int nCmdShow )
//-------------------------------------------------------------------
{
	::hInstance = hInstance;
	::hPrevInstance = hPrevInstance;
	::nCmdShow = nCmdShow;

/*
*   MAKE argc, argv FROM lpsCmdLine
*/
	char * argv[20];
	argv[0] = new char[ strlen( "graph" ) + 1 ];
	strcpy( argv[0], "graph" );

	int argc = 1;
	char far * start = lpszCmdLine;
	for( char far * ps = lpszCmdLine; *ps != '\0'; ps++ ) {
		if ( *ps != ' ' && *ps != '\t' ) continue;
		else if ( ps != start ) {
			argv[ argc ] = ( char * ) malloc( ps - start + 1);
			for(int i = 0; i < ps - start; i++ )
				argv[argc][i] = start[i];
			argv[argc++][i] = '\0';
		}

	}
	if ( ps != start ) {
		argv[ argc ] = ( char * )malloc( ps - start + 1);
		for(int i = 0; i < ps - start; i++ )
			argv[argc][i] = start[i];
		argv[argc++][i] = '\0';
	}
/*
*   CALL APPLICATION DEPENDENT ENTRY
*/
	app.Start( argc, argv );
	return 0;
}
