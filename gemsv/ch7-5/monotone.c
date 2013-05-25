#include "basic.h"
#include <math.h>

#define CROSS_SINE(v0, v1) ((v0).x * (v1).y - (v1).x * (v0).y)
#define LENGTH(v0) (sqrt((v0).x * (v0).x + (v0).y * (v0).y))

static monchain_t mchain[TRSIZE]; /* Table to hold all the monotone */
				  /* polygons . Each monotone polygon */
				  /* is a circularly linked list */

static vertexchain_t vert[SEGSIZE]; /* chain init. information. This */
				    /* is used to decide which */
				    /* monotone polygon to split if */
				    /* there are several other */
				    /* polygons touching at the same */
				    /* vertex  */

static int mon[SEGSIZE];	/* contains position of any vertex in */
				/* the monotone chain for the polygon */
static int visited[TRSIZE];
static int chain_idx, op_idx, mon_idx;

/* Function returns TRUE if the trapezoid lies inside the polygon */
static int inside_polygon(t)
     trap_t *t;
{
  int rseg = t->rseg;

  if (t->state == ST_INVALID)
    return 0;

  if ((t->lseg <= 0) || (t->rseg <= 0))
    return 0;
  
  if (((t->u0 <= 0) && (t->u1 <= 0)) || 
      ((t->d0 <= 0) && (t->d1 <= 0))) /* triangle */
    return (_greater_than(&seg[rseg].v1, &seg[rseg].v0));
  
  return 0;
}


/* return a new mon structure from the table */
static int newmon()
{
  return ++mon_idx;
}


/* return a new chain element from the table */
static int new_chain_element()
{
  return ++chain_idx;
}


static double get_angle(vp0, vpnext, vp1)
     point_t *vp0;
     point_t *vpnext;
     point_t *vp1;
{
  point_t v0, v1;
  
  v0.x = vpnext->x - vp0->x;
  v0.y = vpnext->y - vp0->y;

  v1.x = vp1->x - vp0->x;
  v1.y = vp1->y - vp0->y;

  if (CROSS_SINE(v0, v1) >= 0)	/* sine is positive */
    return DOT(v0, v1)/LENGTH(v0)/LENGTH(v1);
  else
    return (-1.0 * DOT(v0, v1)/LENGTH(v0)/LENGTH(v1) - 2);
}


/* (v0, v1) is the new diagonal to be added to the polygon. Find which */
/* chain to use and return the positions of v0 and v1 in p and q */ 
static int get_vertex_positions(v0, v1, ip, iq)
     int v0;
     int v1;
     int *ip;
     int *iq;
{
  vertexchain_t *vp0, *vp1;
  register int i;
  double angle, temp;
  int tp, tq;

  vp0 = &vert[v0];
  vp1 = &vert[v1];
  
  /* p is identified as follows. Scan from (v0, v1) rightwards till */
  /* you hit the first segment starting from v0. That chain is the */
  /* chain of our interest */
  
  angle = -4.0;
  for (i = 0; i < 4; i++)
    {
      if (vp0->vnext[i] <= 0)
	continue;
      if ((temp = get_angle(&vp0->pt, &(vert[vp0->vnext[i]].pt), 
			    &vp1->pt)) > angle)
	{
	  angle = temp;
	  tp = i;
	}
    }

  *ip = tp;

  /* Do similar actions for q */

  angle = -4.0;
  for (i = 0; i < 4; i++)
    {
      if (vp1->vnext[i] <= 0)
	continue;      
      if ((temp = get_angle(&vp1->pt, &(vert[vp1->vnext[i]].pt), 
			    &vp0->pt)) > angle)
	{
	  angle = temp;
	  tq = i;
	}
    }

  *iq = tq;

  return 0;
}

  
/* v0 and v1 are specified in anti-clockwise order with respect to 
 * the current monotone polygon mcur. Split the current polygon into 
 * two polygons using the diagonal (v0, v1) 
 */
static int make_new_monotone_poly(mcur, v0, v1)
     int mcur;
     int v0;
     int v1;
{
  int p, q, ip, iq;
  int mnew = newmon();
  int i, j, nf0, nf1;
  vertexchain_t *vp0, *vp1;
  
  vp0 = &vert[v0];
  vp1 = &vert[v1];

  get_vertex_positions(v0, v1, &ip, &iq);

  p = vp0->vpos[ip];
  q = vp1->vpos[iq];

  /* At this stage, we have got the positions of v0 and v1 in the */
  /* desired chain. Now modify the linked lists */

  i = new_chain_element();	/* for the new list */
  j = new_chain_element();

  mchain[i].vnum = v0;
  mchain[j].vnum = v1;

  mchain[i].next = mchain[p].next;
  mchain[mchain[p].next].prev = i;
  mchain[i].prev = j;
  mchain[j].next = i;
  mchain[j].prev = mchain[q].prev;
  mchain[mchain[q].prev].next = j;

  mchain[p].next = q;
  mchain[q].prev = p;

  nf0 = vp0->nextfree;
  nf1 = vp1->nextfree;

  vp0->vnext[ip] = v1;

  vp0->vpos[nf0] = i;
  vp0->vnext[nf0] = mchain[mchain[i].next].vnum;
  vp1->vpos[nf1] = j;
  vp1->vnext[nf1] = v0;

  vp0->nextfree++;
  vp1->nextfree++;

#ifdef DEBUG
  fprintf(stderr, "make_poly: mcur = %d, (v0, v1) = (%d, %d)\n", 
	  mcur, v0, v1);
  fprintf(stderr, "next posns = (p, q) = (%d, %d)\n", p, q);
#endif

  mon[mcur] = p;
  mon[mnew] = i;
  return mnew;
}

/* Main routine to get monotone polygons from the trapezoidation of 
 * the polygon.
 */

int monotonate_trapezoids(n)
     int n;
{
  register int i;
  int tr_start;

  memset((void *)vert, 0, sizeof(vert));
  memset((void *)visited, 0, sizeof(visited));
  memset((void *)mchain, 0, sizeof(mchain));
  memset((void *)mon, 0, sizeof(mon));
  
  /* First locate a trapezoid which lies inside the polygon */
  /* and which is triangular */
  for (i = 0; i < TRSIZE; i++)
    if (inside_polygon(&tr[i]))
      break;
  tr_start = i;
  
  /* Initialise the mon data-structure and start spanning all the */
  /* trapezoids within the polygon */

  for (i = 1; i <= n; i++)
    {
      mchain[i].prev = i - 1;
      mchain[i].next = i + 1;
      mchain[i].vnum = i;
      vert[i].pt = seg[i].v0;
      vert[i].vnext[0] = i + 1;	/* next vertex */
      vert[i].vpos[0] = i;	/* locn. of next vertex */
      vert[i].nextfree = 1;
    }
  mchain[1].prev = n;
  mchain[n].next = 1;
  vert[n].vnext[0] = 1;
  vert[n].vpos[0] = n;
  chain_idx = n;
  mon_idx = 0;
  mon[0] = 1;			/* position of any vertex in the first */
				/* chain  */
  
  /* traverse the polygon */
  if (tr[tr_start].u0 > 0)
    traverse_polygon(0, tr_start, tr[tr_start].u0, TR_FROM_UP);
  else if (tr[tr_start].d0 > 0)
    traverse_polygon(0, tr_start, tr[tr_start].d0, TR_FROM_DN);
  
  /* return the number of polygons created */
  return newmon();
}


/* recursively visit all the trapezoids */
int traverse_polygon(mcur, trnum, from, dir)
     int mcur;
     int trnum;
     int from;
     int dir;
{
  trap_t *t = &tr[trnum];
  int mnew;
  int v0, v1;
  int retval;
  int do_switch = FALSE;

  if ((trnum <= 0) || visited[trnum])
    return 0;

  visited[trnum] = TRUE;
  
  /* We have much more information available here. */
  /* rseg: goes upwards   */
  /* lseg: goes downwards */

  /* Initially assume that dir = TR_FROM_DN (from the left) */
  /* Switch v0 and v1 if necessary afterwards */


  /* special cases for triangles with cusps at the opposite ends. */
  /* take care of this first */
  if ((t->u0 <= 0) && (t->u1 <= 0))
    {
      if ((t->d0 > 0) && (t->d1 > 0)) /* downward opening triangle */
	{
	  v0 = tr[t->d1].lseg;
	  v1 = t->lseg;
	  if (from == t->d1)
	    {
	      do_switch = TRUE;
	      mnew = make_new_monotone_poly(mcur, v1, v0);
	      traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
	      traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);	    
	    }
	  else
	    {
	      mnew = make_new_monotone_poly(mcur, v0, v1);
	      traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
	      traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
	    }
	}
      else
	{
	  retval = SP_NOSPLIT;	/* Just traverse all neighbours */
	  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
	  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
	  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
	  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
      }
    }
  
  else if ((t->d0 <= 0) && (t->d1 <= 0))
    {
      if ((t->u0 > 0) && (t->u1 > 0)) /* upward opening triangle */
	{
	  v0 = t->rseg;
	  v1 = tr[t->u0].rseg;
	  if (from == t->u1)
	    {
	      do_switch = TRUE;
	      mnew = make_new_monotone_poly(mcur, v1, v0);
	      traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
	      traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);	    
	    }
	  else
	    {
	      mnew = make_new_monotone_poly(mcur, v0, v1);
	      traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
	      traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
	    }
	}
      else
	{
	  retval = SP_NOSPLIT;	/* Just traverse all neighbours */
	  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
	  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
	  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
	  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
	}
    }
  
  else if ((t->u0 > 0) && (t->u1 > 0)) 
    {
      if ((t->d0 > 0) && (t->d1 > 0)) /* downward + upward cusps */
	{
	  v0 = tr[t->d1].lseg;
	  v1 = tr[t->u0].rseg;
	  retval = SP_2UP_2DN;
	  if (((dir == TR_FROM_DN) && (t->d1 == from)) ||
	      ((dir == TR_FROM_UP) && (t->u1 == from)))
	    {
	      do_switch = TRUE;
	      mnew = make_new_monotone_poly(mcur, v1, v0);
	      traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
	      traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
	      traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
	      traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
	    }
	  else
	    {
	      mnew = make_new_monotone_poly(mcur, v0, v1);
	      traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
	      traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
	      traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
	      traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);	      
	    }
	}
      else			/* only downward cusp */
	{
	  if (_equal_to(&t->lo, &seg[t->lseg].v1))
	    {
	      v0 = tr[t->u0].rseg;
	      v1 = MODULO_NEXT(t->lseg + 1, global.nseg);
	      retval = SP_2UP_LEFT;
	      if ((dir == TR_FROM_UP) && (t->u0 == from))
		{
		  do_switch = TRUE;
		  mnew = make_new_monotone_poly(mcur, v1, v0);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
		}
	    }
	  else
	    {
	      v0 = t->rseg;
	      v1 = tr[t->u0].rseg;	
	      retval = SP_2UP_RIGHT;
	      if ((dir == TR_FROM_UP) && (t->u1 == from))
		{
		  do_switch = TRUE;
		  mnew = make_new_monotone_poly(mcur, v1, v0);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
		}
	    }
	}
    }
  else if ((t->u0 > 0) || (t->u1 > 0)) /* no downward cusp */
    {
      if ((t->d0 > 0) && (t->d1 > 0)) /* only upward cusp */
	{
	  if (_equal_to(&t->hi, &seg[t->lseg].v0))
	    {
	      v0 = tr[t->d1].lseg;
	      v1 = t->lseg;
	      retval = SP_2DN_LEFT;
	      if (!((dir == TR_FROM_DN) && (t->d0 == from)))
		{
		  do_switch = TRUE;
		  mnew = make_new_monotone_poly(mcur, v1, v0);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1);
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);	      
		}
	    }
	  else
	    {
	      v0 = tr[t->d1].lseg;
	      v1 = MODULO_NEXT(t->rseg + 1, global.nseg);
	      retval = SP_2DN_RIGHT;	    
	      if ((dir == TR_FROM_DN) && (t->d1 == from))
		{
		  do_switch = TRUE;
		  mnew = make_new_monotone_poly(mcur, v1, v0);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
		}
	    }
	}
      else			/* no cusp */
	{
	  if (_equal_to(&t->hi, &seg[t->lseg].v0) &&
	      _equal_to(&t->lo, &seg[t->rseg].v0))
	    {
	      v0 = t->rseg;
	      v1 = t->lseg;
	      retval = SP_SIMPLE_LRDN;
	      if (dir == TR_FROM_UP)
		{
		  do_switch = TRUE;
		  mnew = make_new_monotone_poly(mcur, v1, v0);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
		}
	    }
	  else if (_equal_to(&t->hi, &seg[t->rseg].v1) &&
		   _equal_to(&t->lo, &seg[t->lseg].v1))
	    {
	      v0 = MODULO_NEXT(t->rseg + 1, global.nseg);
	      v1 = MODULO_NEXT(t->lseg + 1, global.nseg);
	      retval = SP_SIMPLE_LRUP;
	      if (dir == TR_FROM_UP)
		{
		  do_switch = TRUE;
		  mnew = make_new_monotone_poly(mcur, v1, v0);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
		}
	    }
	  else			/* no split possible */
	    {
	      retval = SP_NOSPLIT;
	      traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
	      traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
	      traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
	      traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);	      	      
	    }
	}
    }

  return retval;
}


int triangulate_monotone_polygons(nmonpoly, op)
     int nmonpoly;
     int op[][3];
{
  register int i;
  point_t ymax, ymin;
  int p, vfirst, posmax, posmin, v;
  int vcount;

#ifdef DEBUG
  for (i = 0; i < nmonpoly; i++)
    {
      fprintf(stderr, "\n\nPolygon %d: ", i);
      vfirst = mchain[mon[i]].vnum;
      p = mchain[mon[i]].next;
      fprintf (stderr, "%d ", mchain[mon[i]].vnum);
      while (mchain[p].vnum != vfirst)
	{
	  fprintf(stderr, "%d ", mchain[p].vnum);
	  p = mchain[p].next;
	}
    }
  fprintf(stderr, "\n");
#endif

  op_idx = 0;
  for (i = 0; i < nmonpoly; i++)
    {
      vcount = 1;
      vfirst = mchain[mon[i]].vnum;
      ymax = ymin = vert[vfirst].pt;
      posmax = posmin = mon[i];
      p = mchain[mon[i]].next;
      while ((v = mchain[p].vnum) != vfirst)
	{
	  if (_greater_than(&vert[v].pt, &ymax))
	    {
	      ymax = vert[v].pt;
	      posmax = p;
	    }
	  if (_less_than(&vert[v].pt, &ymin))
	    {
	      ymin = vert[v].pt;
	      posmin = p;
	    }
	  p = mchain[p].next;
	  vcount++;
	}
      
      if (vcount == 3)		/* already a triangle */
	{
	  op[op_idx][0] = mchain[p].vnum;
	  op[op_idx][1] = mchain[mchain[p].next].vnum;
	  op[op_idx][2] = mchain[mchain[p].prev].vnum;
	  op_idx++;
	}
      else			/* triangulate the polygon */
	{
	  v = mchain[mchain[posmax].next].vnum;
	  if (_equal_to(&vert[v].pt, &ymin))
	    {			/* LHS is a single line */
	      triangulate_single_polygon(posmax, TRI_LHS, op);
	    }
	  else
	    triangulate_single_polygon(posmax, TRI_RHS, op);
	}
    }
  
#ifdef DEBUG
  for (i = 0; i < (global.nseg - 2); i++)
    fprintf(stderr, "tri #%d: (%d, %d, %d)\n", i, op[i][0], op[i][1],
	   op[i][2]);
#endif
}


/* A greedy corner-cutting algorithm to triangulate a y-monotone 
 * polygon in O(n) time.
 * Joseph O-Rourke, Computational Geometry in C.
 */
int triangulate_single_polygon(posmax, side, op)
     int posmax;
     int side;
     int op[][3];
{
  register int v;
  int rc[SEGSIZE], ri = 0;	/* reflex chain */
  int endv, tmp, vpos;
  
  if (side == TRI_RHS)		/* RHS segment is a single segment */
    {
      rc[0] = mchain[posmax].vnum;
      tmp = mchain[posmax].next;
      rc[1] = mchain[tmp].vnum;
      ri = 1;
      
      vpos = mchain[tmp].next;
      v = mchain[vpos].vnum;
      
      if ((endv = mchain[mchain[posmax].prev].vnum) == 0)
	endv = global.nseg;
    }
  else				/* LHS is a single segment */
    {
      tmp = mchain[posmax].next;
      rc[0] = mchain[tmp].vnum;
      tmp = mchain[tmp].next;
      rc[1] = mchain[tmp].vnum;
      ri = 1;

      vpos = mchain[tmp].next;
      v = mchain[vpos].vnum;

      endv = mchain[posmax].vnum;
    }
  
  while ((v != endv) || (ri > 1))
    {
      if (ri > 0)		/* reflex chain is non-empty */
	{
	  if (CROSS(vert[v].pt, vert[rc[ri - 1]].pt, 
		    vert[rc[ri]].pt) > 0)
	    {			/* convex corner: cut if off */
	      op[op_idx][0] = rc[ri - 1];
	      op[op_idx][1] = rc[ri];
	      op[op_idx][2] = v;
	      op_idx++;	     
	      ri--;
	    }
	  else		/* non-convex */
	    {		/* add v to the chain */
	      ri++;
	      rc[ri] = v;
	      vpos = mchain[vpos].next;
	      v = mchain[vpos].vnum;
	    }
	}
      else			/* reflex-chain empty: add v to the */
	{			/* reflex chain and advance it  */
	  rc[++ri] = v;
	  vpos = mchain[vpos].next;
	  v = mchain[vpos].vnum;
	}
    } /* end-while */
  
  /* reached the bottom vertex. Add in the triangle formed */
  op[op_idx][0] = rc[ri - 1];
  op[op_idx][1] = rc[ri];
  op[op_idx][2] = v;
  op_idx++;	     
  ri--;
  
  return 0;
}


