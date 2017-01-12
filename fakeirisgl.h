#pragma once

// Some code here still uses the old IRIS GL
// (https://en.wikipedia.org/wiki/IRIS_GL).
// This is a fake implementation to allow the examples to compile.

typedef struct { int x; int y; int z; } Coord;
typedef int Device;

#ifndef TRUE
#define TRUE			1
#define FALSE			0
#endif

#define WINSHUT			777
#define MOUSEX			778
#define MOUSEY			779
#define LEFTMOUSE		780
#define LEFTCTRLKEY		781
#define RIGHTCTRLKEY		782
#define LEFTSHIFTKEY		783
#define RIGHTSHIFTKEY		784
#define CAPSLOCKKEY		785
#define INPUTCHANGE		787
#define REDRAW			788
#define HKEY			789
#define QKEY			790

#ifdef __cplusplus
extern "C" {
#endif

void bgnline();
void bgnpolygon();
void endpolygon();
void multmatrix(float m[4][4]);
void pushmatrix();
void popmatrix();
void loadmatrix();
void scale();
void ortho(float a, float b, float c, float d, float e, float f);
void ortho2(float, float, float, float);
void RGBcolor(int r, int g, int b);
void circ();
void v2d(double*);
void v2i(int*);
void v3f();
void endline();
void prefsize(int, int);
int winopen(const char*);
void doublebuffer();
void RGBmode();
void gconfig();
void cpack(int);
void clear();
void swapbuffers();
int getvaluator(int);
void getorigin(long*, long*);
void getsize(long*, long*);
void qdevice(int);
void qreset();
void qenter(int, short);
int qread(short*);
void winset(int);
void reshapeviewport();
void winclose(int);
void gexit();
void v4f();
//void drawbody(float m[4][4]);
void keepaspect(int x, int y);
void prefposition(int a, int b, int c, int d);
void translate(float a, float b, float c);
int qtest();
void foreground();

#ifdef __cplusplus
}
#endif
