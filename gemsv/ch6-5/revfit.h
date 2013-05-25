/* revfit.h: definitions for reversible straight line reconstruction routines */
#include "../ch7-7/GG4D/GGems.h"
#define HRZ             1
#define VRT             2
/* Watch out for the precision of `int' type. Make sure that the max */
/* coordinate value * SUBPIXRES can be stored in an `int'.           */
#define SUBPIXRES       32

/************************************************************************/
/* typedef for Edgelist: the list of edges where lines are to be fitted */
/************************************************************************/
typedef struct {
 int x,y;               /* in bitmap resolution        +ve         */
 int dir;               /*           ---  <-- H edge    ^          */
} PixelEdge;            /* (x,y)-->   * | <-- V edge    | --> +ve  */

typedef struct {
 int Nedges;            /* number of edges in the list             */
 int current;           /* current edge being visited              */
 PixelEdge *list;       /* the list of edges found from the pixmap */
} Edgelist;

int fitlines(Edgelist el, boolean Pretest, boolean TryAllEndPts,
             IntPoint2 *lines, int MaxLines);
void linestojagged(int Nlines, IntPoint2 *lines);
