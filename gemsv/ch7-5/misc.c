#include "basic.h"
#include <sys/time.h>
#include <math.h>

/* If you do not have log2() on your machine, use this macro:
#define log2(x) (log((double)x)/log(2.0))
*/

#ifdef __STDC__
extern double log2(double);
#else
extern double log2();
#endif

static int choose_idx;
static int permute[SEGSIZE];

/* Generate a random permutation of the segments 1..n */
int generate_random_ordering(n)
     int n;
{
  struct timeval tval;
  struct timezone tzone;
  register int i;
  int m, st[SEGSIZE], *p;
  
  choose_idx = 1;
  gettimeofday(&tval, &tzone);
  srand48(tval.tv_sec);

  for (i = 0; i <= n; i++)
    st[i] = i;

  p = st;
  for (i = 1; i <= n; i++, p++)
    {
      m = lrand48() % (n + 1 - i) + 1;
      permute[i] = p[m];
      if (m != 1)
	p[m] = p[1];
    }
  return 0;
}

  
/* Return the next segment in the generated random ordering of all the */
/* segments in S */
int choose_segment()
{
  int i;
/*  
#ifdef DEBUG
  fprintf(stderr, "choose_segment: %d\n", permute[choose_idx]);
#endif 
  return permute[choose_idx++];
*/

#ifdef CHOOSE_MANUAL
  printf("Enter seg: ");
  scanf("%d", &i);
  return i;
#else
  
#ifdef DEBUG
  fprintf(stderr, "choose_segment: %d\n", permute[choose_idx]);
#endif 
  return permute[choose_idx++];
#endif
}


int inserted(segnum, whichpt)
     int segnum;
     int whichpt;
{
  int n1, n2;

  n1 = segnum % global.nseg + 1; /* next seg. */
  n2 = (segnum - 1 + global.nseg - 1) % global.nseg + 1; /* prev. */ 

  if (whichpt == FIRSTPT)
    return seg[n2].is_inserted;
  else
    return seg[n1].is_inserted;
}


#ifdef STANDALONE

/* Read in the list of vertices from infile */
int read_segments(infile)
     FILE *infile;
{
  int nseg;
  register int i;

  memset((void *)seg, 0, sizeof(seg));
  i = 1; 
  nseg = 0;
  while (fscanf(infile, "%lf%lf", &seg[i].v0.x, &seg[i].v0.y) == 2)
    {
      seg[i - 1].v1.x = seg[i].v0.x;
      seg[i - 1].v1.y = seg[i].v0.y;
      seg[i].is_inserted = FALSE;
      i++;
      nseg++;
    }
  seg[nseg].v1.x = seg[1].v0.x;
  seg[nseg].v1.y = seg[1].v0.y;
  
  global.nseg = nseg;
  return nseg;
}

#endif


/* Get log*n for given n */
int math_logstar_n(n)
     int n;
{
  register int i;
  double v;
  
  for (i = 0, v = (double) n; v >= 1; i++)
    v = log2(v);
  
  return (i - 1);
}
  

int math_N(n, h)
     int n;
     int h;
{
  register int i;
  double v;

  for (i = 0, v = (int) n; i < h; i++)
    v = log2(v);
  
  return (int) ceil((double) 1.0*n/v);
}
