/***** Demo.c *****/
/* Ken Shoemake, 1993 */
#include <gl/gl.h>
#include <gl/device.h>

#include "BallAux.h"
#include "Body.h"
#include "Ball.h"

typedef struct {long x, y;} Place;

#define RADIUS	  (0.75)
#define CNTRLDN	  1
#define SHIFTDN	  2

void main(void)
{
    int gid;
    short active;    /* TRUE if window is attached */
    Device dev;
    short val;
    Place winsize, winorig;
    Place mouseNow, mouseDown;
    int keysDown;
    HVect vNow;
    BallData ball;

    keepaspect(1, 1);
    prefposition(50, 950, 50, 950);
    gid = winopen("Arcball Demo");
    doublebuffer();
    RGBmode();
    gconfig();
    qdevice(WINSHUT);
    qdevice(MOUSEX); qdevice(MOUSEY); qdevice(LEFTMOUSE);
    qdevice(LEFTCTRLKEY); qdevice(RIGHTCTRLKEY);
    qdevice(LEFTSHIFTKEY); qdevice(RIGHTSHIFTKEY);
    qdevice(CAPSLOCKKEY);
    /* perspective(400, 1.0, 0.001, 100000.0); */
    ortho(-1.0, 1.0, -1.0, 1.0, 0.001, 100000.0);
    translate(0.0, 0.0, -3.0);
    active = 0;

    getsize(&winsize.x, &winsize.y);
    getorigin(&winorig.x, &winorig.y);
    keysDown = 0;
    Ball_Init(&ball);
    Ball_Place(&ball, qOne, RADIUS);

    while (TRUE) {
	while (qtest()) {	 /* process queued events */
	    dev = qread(&val);
	    switch (dev) {
		case WINSHUT:	 /* exit program */
		    gexit();
		    exit(0);
		    break;
		case INPUTCHANGE:
		    active = val;
		    break;
		case REDRAW:
		    reshapeviewport();
		    getsize(&winsize.x, &winsize.y);
		    getorigin(&winorig.x, &winorig.y);
		    break;
		case MOUSEX:
		    mouseNow.x = val;
		    vNow.x = 2.0*(mouseNow.x - winorig.x)/winsize.x - 1.0;
		    break;
		case MOUSEY:
		    mouseNow.y = val;
		    vNow.y = 2.0*(mouseNow.y - winorig.y)/winsize.y - 1.0;
		    break;
		case LEFTMOUSE:
		    if (val) Ball_BeginDrag(&ball);
		    else     Ball_EndDrag(&ball);
		    break;
		case LEFTCTRLKEY: case RIGHTCTRLKEY:
		    keysDown = (keysDown&~CNTRLDN)|(val? CNTRLDN : 0);
		    break;
		case LEFTSHIFTKEY: case RIGHTSHIFTKEY:
		    keysDown = (keysDown&~SHIFTDN)|(val? SHIFTDN : 0);
		    break;
		case CAPSLOCKKEY:
		    if (val) Ball_ShowResult(&ball);
		    else     Ball_HideResult(&ball);
		    break;
		default:
		    break;
	    }			 /* end of switch */
	    Ball_Mouse(&ball, vNow);
	    switch (keysDown) {
		case CNTRLDN+SHIFTDN: Ball_UseSet(&ball, OtherAxes);  break;
		case CNTRLDN:	      Ball_UseSet(&ball, BodyAxes);   break;
		case	     SHIFTDN: Ball_UseSet(&ball, CameraAxes); break;
		default:	      Ball_UseSet(&ball, NoAxes);     break;
	    }
	}			 /* end of while on qtest */
	Ball_Update(&ball);
	scene_Draw(&ball);	 /* draw into the back buffer */
	swapbuffers();		 /* and show it in the front buffer */
    }
    /* NOT REACHED */
}

/* Draw whole window, including controller. */
void scene_Draw(BallData *ball)
{
    RGBcolor(0, 0, 0);
    clear();
    body_Draw(ball);
    Ball_Draw(ball);
}

/* Draw the object being controlled. */
void body_Draw(BallData *ball)
{
    HMatrix mNow;
    Ball_Value(ball, mNow);
    pushmatrix();
    multmatrix(mNow);
    scale(RADIUS, RADIUS, RADIUS);
    drawbody(mNow);
    popmatrix();
}
