/* TEST PROGRAM FOR DELAUNAY */

#include <stdlib.h>
#include <stream.h>
#include <string.h>
#include <gl.h>
#include <device.h>
#include <quadedge.h>

void getArguments(int, char**);
void InsertPoints(Subdivision&);
void display(Subdivision&, Real, Real, Real, Real);

char *program;
int num = 20;

main(int argc, char** argv)
{
	getArguments(argc, argv);

	// Construct a triangle containing the unit square:
	Point2d p1(-1,-1), p2(2,-1), p3(0.5,3);
	Subdivision mesh(p1, p2, p3);

	InsertPoints(mesh);
	display(mesh, -.1, -.1, 1.1, 1.1);

	exit(0);
}

static void usage()
{
	cerr << "usage: " << program << " [ -n number_of_points ]\n";
}

static void errmsg(char *msg)
{
	cerr << program << ": " << msg << endl;
	usage();
	exit(1);
}

void getArguments(int argc, char** argv)
{
	program = argv[0];

	if(argc == 2 && strcmp(argv[1], "-h") == 0) {
		usage();
		exit(0);
	}

	for (int i = 1; i < argc && argv[i][0] == '-'; i++)
		if (strcmp(argv[i], "-n") == 0) {
			if(++i < argc)
				num = atoi(argv[i]);
			else
				errmsg("option ``-n'': missing parameter");
		} else
			errmsg("unknown option");

	if(argc - i > 0)
		errmsg("too many parameters");
}

void InsertPoints(Subdivision& mesh)
{
	for (int i = 0; i < num; i++) {
		double u = drand48();
		double v = drand48();
		mesh.InsertSite(Point2d(u,v));
	}
}

long createWindow()
{
	prefsize(512, 512);
	long w = winopen("Delaunay Triangulation");
	doublebuffer();
	RGBmode();
	gconfig();
	return w;
}

void draw(Subdivision& mesh)
{
	cpack(0);
	clear();
	cpack(0xffffffff);

	/* draw your stuff here */
	mesh.Draw();

	swapbuffers();
}

void setView(float cx, float cy, float zoom, float xsize, float ysize)
{
	float dx = 0.5 * xsize / zoom;
	float dy = 0.5 * ysize / zoom;

	ortho2(cx - dx, cx + dx, cy - dy, cy + dy);
}

void printHelp()
{
	cerr << "<left>	   insert a new point\n"
		 << "  <h>     print help message\n"
		 << "  <q>     quit\n";
}

void getMouse (long,
		float cx, float cy, float zoom,
		float xsize, float ysize,
		float& x, float& y)
{
	long sx, sy, ox, oy, sxsize, sysize;
	Coord p[3], v[3];

	sx = getvaluator(MOUSEX);
	sy = getvaluator(MOUSEY);
	getorigin(&ox, &oy);
	sx -= ox;
	sy -= oy;
	getsize(&sxsize, &sysize);
	xsize /= zoom;
	ysize /= zoom;
	x = (float(sx) / float(sxsize)) * xsize - 0.5 * xsize + cx;
	y = (float(sy) / float(sysize)) * ysize - 0.5 * ysize + cy;
}

void display(Subdivision& mesh, Real left, Real bottom, Real right, Real top)
{
	long wid;
	float cx, cy, zoom, xsize, ysize;

	wid = createWindow();

	cx = 0.5 * (left + right);
	cy = 0.5 * (bottom + top);
	zoom = 1;
	xsize = 1.1 * (right - left);
	ysize = 1.1 * (top - bottom);

	setView(cx, cy, zoom, xsize, ysize);

	qdevice(LEFTMOUSE);
	qdevice(HKEY);
	qdevice(QKEY);

	qreset();
	qenter(REDRAW, (short)wid);

	for (int done = FALSE; !done; ) {
		short val;

		switch(qread(&val)) {
		case REDRAW:
			winset(wid);
			reshapeviewport();
			setView(cx, cy, zoom, xsize, ysize);
			draw(mesh);
			break;
		case LEFTMOUSE:
			if(val) {
				float x, y;
				getMouse(wid, cx, cy, zoom, xsize, ysize, x, y);
				cerr << "Mouse at (" << x << "," << y << ")\n";
				x = (x < 0) ? 0 : (x > 1) ? 1 : x;
				y = (y < 0) ? 0 : (y > 1) ? 1 : y;
				mesh.InsertSite(Point2d(x, y));
				qreset();
				draw(mesh);
			}
			break;
		case HKEY:
			if(val)
				printHelp();
			break;
		case QKEY:
			if(val)
				done = TRUE;
			break;
		}
	}

	winclose(wid);
	gexit();
}
