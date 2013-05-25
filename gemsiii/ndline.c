/* Badouel / Wuthrich "Face Connected Line Segment Generation in an 
   N-dimensional Space" 
*/
/******************************************************************************
Data structure for n-dimensional line segment generation. Initialized by the 
procedure Init() and used by the procedure Incr().
******************************************************************************/

#define DIM   4                  /* number of dimensions                     */

typedef struct {
  int  D[DIM];                   /* counter for each of the N dimensions     */
  int  N[DIM];                   /* increment for each of the N dimensions   */
  int  S[DIM];                   /* orientation for each of the N dimensions */
  int  cm;                       /* common multiple                          */
} Nline;

/******************************************************************************
Init(): initializes the data structure in order to generate the discrete
path between the point P and the point Q in an n-dimensional space.

This procedure should be called once before using Incr().

Entry:	P    - origin point
        Q    - destination point
        line - line segment data structure
******************************************************************************/

void Init (P, Q, line)
     int   *P, *Q;
     Nline *line;
{
  int i, v;

  line->cm = 1;
  for (i=0; i<DIM; i++) {
    v = Q[i] - P[i];
    if (v < 0) {
      line->S[i] = -1; line->N[i] = -v;
    } else {
      line->S[i] =  1; line->N[i] =  v;
    }
    if (line->N[i] != 0) line->cm *= line->N[i];
  }
  for (i=0; i<DIM; i++) {
    if (line->N[i] == 0) 
      line->D[i] = 2*line->cm;
    else {
      line->D[i] = line->cm/line->N[i];
      line->N[i] = 2*line->D[i];
    }
  }
}

/******************************************************************************
Incr(): generates one step of a discrete segment line in an n-dimensional 
space. Indicate the end of the generation with the returned value -1 for 
the direction.

The procedure Init() must be called once before using Incr(). 

Entry:	line - line segment data structure
Exit:   d    - current step direction
        s    - current step orientation
******************************************************************************/

int Incr (line, d, s)
     Nline *line;
     int   *d, *s;
{
  int i, v = 2*line->cm;

  *d = -1;
  for (i=0; i<DIM; i++) {
    if (line->D[i] < v) {
      v = line->D[i];
      *d = i;
    }
  }
  line->D[*d] += line->N[*d];
  *s = line->S[*d];
  return *d;
}


