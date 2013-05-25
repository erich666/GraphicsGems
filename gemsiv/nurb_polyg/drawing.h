/*
 * These routines need to be provided by your rendering package
 */

extern void FastTriangle( SurfSample *, SurfSample *, SurfSample * );
extern void SlowTriangle( SurfSample *, SurfSample *, SurfSample * );

extern void (*DrawTriangle)( SurfSample *, SurfSample *, SurfSample * );

extern void ScreenProject( Point4 *, Point3 * );
