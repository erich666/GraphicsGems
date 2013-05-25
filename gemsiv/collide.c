/*
 * C code from the article
 * "Fast Collision Detection of Moving Convex Polyhedra"
 * by Rich Rabbitz, rrabbitz%pgn138fs@serling.motown.ge.com
 * in "Graphics Gems IV", Academic Press, 1994
 */

#include <math.h>

typedef long	       Boolean;

#define FUZZ	       (0.00001)
#define TRUE	       (1)
#define FALSE	       (0)
#define MAX_VERTS      (1026)

#define ABS(x)	       ( (x) > 0 ? (x) : -(x) )
#define EQZ(x)	       ( ABS((x)) < FUZZ ? TRUE : FALSE )

#define DOT3(u,v)      ( u[0]*v[0] + u[1]*v[1] + u[2]*v[2])
#define VECADD3(r,u,v) { r[0]=u[0]+v[0]; r[1]=u[1]+v[1]; r[2]=u[2]+v[2]; }
#define VECADDS3(r,a,u,v){r[0]=a*u[0]+v[0]; r[1]=a*u[1]+v[1]; r[2]=a*u[2]+v[2];}
#define VECSMULT3(a,u) { u[0]= a * u[0]; u[1]= a * u[1]; u[2]= a * u[2]; }
#define VECSUB3(r,u,v) { r[0]=u[0]-v[0]; r[1]=u[1]-v[1]; r[2]=u[2]-v[2]; }
#define CPVECTOR3(u,v) { u[0]=v[0];	 u[1]=v[1];	 u[2]=v[2]; }
#define VECNEGATE3(u)  { u[0]=(-u[0]);	 u[1]=(-u[1]);	 u[2]=(-u[2]); }

#define GET(u,i,j,s) (*(u+i*s+j))
#define GET3(u,i,j,k,s) (*(u+i*(s*2)+(j*2)+k))

/**************************************************************************
 *
 * The structure polyhedron is used to store the geometry of the primitives
 * used in this collision detection example.  Since the collision detection
 * algorithm only needs to operate on the vertex set of a polyhedron, and
 * no rendering is done in this example, the faces and edges of a
 * polyhedron are not stored.  Adding faces and edges to the structure for
 * rendering purposes should be straight forward and will have no effect on
 * the collision detection computations.
 *
 **************************************************************************/

typedef struct polyhedron {
   double   verts[MAX_VERTS][3]; /* 3-D vertices of polyhedron. */
   int	    m;			 /* number of 3-D vertices.  */
   double   trn[3];		 /* translational position in world coords. */
   double   itrn[3];		 /* inverse of translational position. */
} *Polyhedron;

/**************************************************************************
 *
 * The structure couple is used to store the information required to
 * repeatedly test a pair of polyhedra for collision.  This information
 * includes : a reference to each polyhedron,  a flag indicating if there
 * is a cached prospective proper separating plane, two points for
 * constructing the proper separating plane, and possibly a cached set
 * of points from each polyhedron for speeding up the distance algorithm.
 *
 **************************************************************************/

typedef struct couple {
   Polyhedron  polyhdrn1;	/* First polyhedron of collision test. */
   Polyhedron  polyhdrn2;	/* Second polyhedron of collision test. */
   Boolean     plane_exists;	/* prospective separating plane flag */
   double      pln_pnt1[3];	/* 1st point used to form separating plane. */
   double      pln_pnt2[3];	/* 2nd point used to form separating plane. */
   int	       vert_indx[4][2]; /* cached points for distance algorithm. */
   int	       n;		/* number of cached points, if any. */
} *Couple;


/*** Arrays for vertex sets of three primitives ***/

double box[24];
double cyl[108];
double sphere[1026];

/*** RJR 08/20/93 ***********************************************************
 *
 *   Function to create vertex set of a polyhedron used to represent a
 *   box primitive.
 *
 *   On Entry:
 *	box_verts - an empty array of type double of size 24 or more.
 *
 *   On Exit:
 *	box_verts  - vertices of a polyhedron representing a box with
 *		     dimensions length = 5.0, width = 5.0, and height = 5.0.
 *
 *   Function Return : none.
 *
 ****************************************************************************/

void mak_box(box_verts)
double		  box_verts[];
{
   int		  i;
   static  double verts[24] =
	  {-5.0,  5.0, 5.0,  -5.0,  5.0, -5.0,	5.0,  5.0, -5.0,
	  5.0,	5.0, 5.0,  -5.0, -5.0, 5.0,  -5.0, -5.0, -5.0,
	  5.0, -5.0, -5.0,  5.0, -5.0, 5.0};

   for (i = 0; i < 24; i++)
      box_verts[i] = verts[i];
}


/*** RJR 08/20/93 ***********************************************************
 *
 *   Function to create vertex set of a polyhedron used to approximate
 *   a cylinder primitive.
 *
 *   On Entry:
 *	cyl_verts - an empty array of type double of size 108 or more.
 *
 *   On Exit:
 *	cyl_verts  - vertices of a polyhedron approximating a cylinder with
 *		     a base radius = 5.0, and height = 5.0.
 *   Function Return : none.
 *
 ****************************************************************************/

void mak_cyl(cyl_verts)
double	      cyl_verts[];
{
   int	      i;
   double     *pD_1, *pD_2, rads, stp, radius;

   pD_1 = cyl_verts;	  pD_2 = cyl_verts + 54;
   stp = 0.34906585;	  rads = 0.0;		    radius = 5.0;

   for (i = 0; i < 18; i++) {
      pD_1[0] = pD_2[0] = radius * cos(rads);	   /* X for top and bot. */
      pD_1[1] = 5.0;				   /* Y for top */
      pD_2[1] = 0.0;				   /* Y for bot. */
      pD_1[2] = pD_2[2] = -(radius * sin(rads));   /* Z for top and bot. */
      rads += stp;   pD_1 += 3;	  pD_2 += 3;
   }
}


/*** RJR 08/20/93 ***********************************************************
 *
 *   Function to create vertex set of a polyhedron used to approximate
 *   a sphere primitive.
 *
 *   On Entry:
 *	sph_verts - an empty array of type double of size 1026 or more.
 *
 *   On Exit:
 *	sph_verts  - vertices of a polyhedron approximating a sphere with
 *		     a radius = 5.25.
 *   Function Return : none.
 *
 ****************************************************************************/

void mak_sph(sph_verts)
double	    sph_verts[];
{
   int	    i, j;
   double   rads_1, rads_2, stp_1, stp_2, *pD_1, *pD_2, radius;

   rads_1 = 1.570796327;   stp_1  = 0.174532935;
   rads_2 = 0.0;	   stp_2  = 0.34906585;
   pD_1	  = sph_verts;	   radius = 5.25;

   for (i = 0; i < 19; i++) {
      pD_1[0] = radius * cos(rads_1);	  pD_1[1] = radius * sin(rads_1);
      pD_1[2] = 0.0;
      if (EQZ(pD_1[0]))
	 pD_1[0] = 0.01;
      rads_2 = 0.0;	stp_2 = 0.34906585;

      for (j = 0; j < 18; j++) {
	 pD_2 = pD_1 + j * 3;
	 pD_2[0] =   pD_1[0] * cos(rads_2) - pD_1[2] * sin(rads_2);  /* X */
	 pD_2[1] =   pD_1[1];					     /* Y */
	 pD_2[2] = -(pD_1[0] * sin(rads_2) + pD_1[2] * cos(rads_2)); /* Z */
	 rads_2 += stp_2;
      }
      pD_1 += 54;   rads_1 -= stp_1;
   }
}


/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to evaluate the support and contact functions at A for a given
 *   polytope. See equations (6) & (7).
 *
 *   On Entry:
 *	P   - table of 3-element points containing polytope vertices.
 *	r   - number of points in table.
 *	A   - vector at which support and contact functions will be evaluated.
 *	Cp  - empty 3-element array.
 *	P_i - pointer to an int.
 *
 *   On Exit:
 *	Cp  - contact point of P w.r.t. A.
 *	P_i - index into P of contact point.
 *
 *   Function Return :
 *	the result of the evaluation of eq. (6) for P and A.
 *
 ****************************************************************************/

double Hp(P, r, A, Cp, P_i)
double		 P[][3], A[], Cp[];
int		 r, *P_i;
{
   int		 i;
   double	 max_val, val;

   max_val = DOT3(P[0], A);	 *P_i = 0;

   for (i = 1; i < r; i++) {
      val = DOT3(P[i], A);
      if (val > max_val) {
	 *P_i = i;
	 max_val = val;
      }
   }
   CPVECTOR3(Cp, P[*P_i]);

   return max_val;
}


/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to evaluate the support and contact functions at A for the
 *   set difference of two polytopes. See equations (8) & (9).
 *
 *   On Entry:
 *	P1   - table of 3-element points containing first polytope's vertices.
 *	m1   - number of points in P1.
 *	P2   - table of 3-element points containing second polytope's vertices.
 *	m2   - number of points in P2.
 *	A    - vector at which to evaluate support and contact functions.
 *	Cs   - an empty array of size 3.
 *	P1_i - a pointer to an int.
 *	P2_i - a pointer to an int.
 *
 *   On Exit:
 *	Cs   - solution to equation 9.
 *	P1_i - index into P1 for solution to equation 9.
 *	P2_i - index into P2 for solution to equation 9.
 *
 *   Function Return :
 *	the result of the evaluation of eq. (8) for P1 and P2 at A.
 *
 ****************************************************************************/

double Hs(P1, m1, P2, m2, A, Cs, P1_i, P2_i)
double	     P1[][3], P2[][3], A[], Cs[];
int	     m1, m2, *P1_i, *P2_i;
{
   double    Cp_1[3], Cp_2[3], neg_A[3], Hp_1, Hp_2;

   Hp_1 = Hp(P1, m1, A, Cp_1, P1_i);

   CPVECTOR3(neg_A, A);
   VECNEGATE3(neg_A);
   Hp_2 = Hp(P2, m2, neg_A, Cp_2, P2_i);

   VECSUB3(Cs ,Cp_1, Cp_2);

   return (Hp_1 + Hp_2);
}

/*** RJR 05/26/93 ***********************************************************
 *
 *   Alternate function to compute the point in a polytope closest to the
 *   origin in 3-space. The polytope size m is restricted to 1 < m <= 4.
 *   This function is called only when comp_sub_dist fails.
 *
 *   On Entry:
 *	stop_index - number of sets to test.
 *	D_P	   - array of determinants for each set.
 *	Di_P	   - cofactors for each set.
 *	Is	   - indices for each set.
 *	c2	   - row size offset.
 *
 *   On Exit:
 *
 *   Function Return :
 *	the index of the set that is numerically closest to eq. (14).
 *
 ****************************************************************************/

int sub_dist_back(P, stop_index, D_P, Di_P, Is, c2)
double		  P[][3], Di_P[][4], *D_P;
int		  stop_index, *Is, c2;
{
   Boolean	  first, pass;
   int		  i, k, s, is, best_s;
   float	  sum, v_aff, best_v_aff;

   first = TRUE;  best_s = -1;
   for (s = 0; s < stop_index; s++) {
      pass = TRUE;
      if (D_P[s] > 0.0) {
	 for (i = 1; i <= GET(Is,s,0,c2); i++) {
	    is = GET(Is,s,i,c2);
	    if (Di_P[s][is] <= 0.0)
	       pass = FALSE;
	 }
      }
      else
	 pass = FALSE;

      if (pass) {

	 /*** Compute equation (33) in Gilbert ***/

	 k = GET(Is,s,1,c2);
	 sum = 0;
	 for (i = 1; i <= GET(Is, s, 0, c2); i++) {
	    is = GET(Is,s,i,c2);
	    sum += Di_P[s][is] * DOT3(P[is],P[k]);
	 }
	 v_aff = sqrt(sum / D_P[s]);
	 if (first) {
	    best_s = s;
	    best_v_aff = v_aff;
	    first = FALSE;
	 }
	 else {
	    if (v_aff < best_v_aff) {
	       best_s = s;
	       best_v_aff = v_aff;
	    }
	 }
      }
   }
   if (best_s == -1) {
      printf("backup failed\n");
      exit(0);
   }
   return best_s;
}

/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to compute the point in a polytope closest to the origin in
 *   3-space. The polytope size m is restricted to 1 < m <= 4.
 *
 *   On Entry:
 *	   P  - table of 3-element points containing polytope's vertices.
 *	   m  - number of points in P.
 *	  jo  - table of indices for storing Dj_P cofactors in Di_P.
 *	  Is  - indices into P for all sets of subsets of P.
 *	  IsC - indices into P for complement sets of Is.
 *   near_pnt - an empty array of size 3.
 *  near_indx - an empty array of size 4.
 *     lambda - an empty array of size 4.
 *
 *   On Exit:
 *   near_pnt - the point in P closest to the origin.
 *  near_indx - indices for a subset of P which is affinely independent.
 *		See eq. (14).
 *     lambda - the lambda as in eq. (14).
 *
 *   Function Return :
 *	the number of entries in near_indx and lambda.
 *
 ****************************************************************************/

int comp_sub_dist(P, m, jo, Is, IsC, near_pnt, near_indx, lambda)
double		    P[][3], near_pnt[], lambda[];
int		    m, *jo, *Is, *IsC, near_indx[];
{
   Boolean	    pass, fail;
   int		    i, j, k, isp, is, s, row, col, stop_index, c1, c2;
   double	    D_P[15], x[3], Dj_P, Di_P[15][4];
   static int	    combinations[5] = {0,0,3,7,15};

   stop_index = combinations[m];    /** how many subsets in P **/
   c1 = m;  c2 = m + 1;		    /** row offsets for IsC and Is **/

   /** Initialize Di_P for singletons **/

   Di_P[0][0] = Di_P[1][1] = Di_P[2][2] = Di_P[3][3] = 1.0;
   s = 0;   pass = FALSE;

   while ((s < stop_index) && (!pass)) {	/* loop through each subset */
      D_P[s] = 0.0;  fail = FALSE;
      for (i = 1; i <= GET(Is,s,0,c2); i++) {	/** loop through all Is **/
	 is = GET(Is,s,i,c2);
	 if (Di_P[s][is] > 0.0)			/** Condition 2 Theorem 2 **/
	    D_P[s] += Di_P[s][is];		/** sum from eq. (16)	  **/
	 else
	    fail = TRUE;
      }

      for (j = 1; j <= GET(IsC,s,0,c1); j++) {	/** loop through all IsC **/
	 Dj_P = 0;  k = GET(Is,s,1,c2);
	 isp = GET(IsC,s,j,c1);

	 for (i = 1; i <= GET(Is,s,0,c2); i++) {
	    is = GET(Is,s,i,c2);
	    VECSUB3(x, P[k], P[isp]);		  /** Wk - Wj  eq. (18) **/
	    Dj_P += Di_P[s][is] * DOT3(P[is], x); /** sum from eq. (18) **/
	 }
	 row = GET3(jo,s,isp,0,c1);
	 col = GET3(jo,s,isp,1,c1);
	 Di_P[row][col] = Dj_P;			 /** add new cofactors	**/

	 if (Dj_P > 0.00001)			 /** Condition 3 Theorem 2 **/
	    fail = TRUE;
      }
      if ((!fail) && (D_P[s] > 0.0))  /** Conditions 2 && 3 && 1 Theorem 2  **/
	 pass = TRUE;
      else
	 s++;
   }
   if (!pass) {
      printf("*** using backup procedure in sub_dist\n");
      s = sub_dist_back(P, stop_index, D_P, Di_P, Is, c2);
   }

   near_pnt[0] = near_pnt[1] = near_pnt[2] = 0.0;
   j = 0;
   for (i = 1; i <= GET(Is,s,0,c2); i++) {	 /** loop through all Is **/
      is = GET(Is,s,i,c2);
      near_indx[j] = is;
      lambda[j] = Di_P[s][is] / D_P[s];		       /** eq. (17)  **/
      VECADDS3(near_pnt, lambda[j], P[is], near_pnt);  /** eq. (17)  **/
      j++;
   }

   return (i-1);
}

/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to compute the point in a polytope closest to the origin in
 *   3-space.  The polytope size m is restricted to 1 < m <= 4.
 *
 *   On Entry:
 *	   P  - table of 3-element points containing polytope's vertices.
 *	   m  - number of points in P.
 *   near_pnt - an empty array of size 3.
 *  near_indx - an empty array of size 4.
 *     lambda - an empty array of size 4.
 *
 *   On Exit:
 *   near_pnt - the point in P closest to the origin.
 *  near_indx - indices for a subset of P which is affinely independent.
 *		See eq. (14).
 *     lambda - the lambda as in eq. (14).
 *
 *   Function Return :
 *	the number of entries in near_indx and lambda.
 *
 ****************************************************************************/

int sub_dist(P, m, near_pnt, near_indx, lambda)
double	      P[][3], near_pnt[], lambda[];
int	      near_indx[], m;
{
   int	      size;

/*
 *
 *  Tables to index the Di_P cofactor table in comp_sub_dist.  The s,i
 *  entry indicates where to store the cofactors computed with Is_C.
 *
 */

   static int	   jo_2[2][2][2]  = { {{0,0}, {2,1}},
				      {{2,0}, {0,0}}};

   static int	   jo_3[6][3][2]  = { {{0,0}, {3,1}, {4,2}},
				      {{3,0}, {0,0}, {5,2}},
				      {{4,0}, {5,1}, {0,0}},
				      {{0,0}, {0,0}, {6,2}},
				      {{0,0}, {6,1}, {0,0}},
				      {{6,0}, {0,0}, {0,0}}};

   static int	  jo_4[14][4][2] = { { {0,0}, {4,1}, {5,2}, {6,3}},
				     { {4,0}, {0,0}, {7,2}, {8,3}},
				     { {5,0}, {7,1}, {0,0}, {9,3}},
				     { {6,0}, {8,1}, {9,2}, {0,0}},
				     { {0,0}, {0,0},{10,2},{11,3}},
				     { {0,0},{10,1}, {0,0},{12,3}},
				     { {0,0},{11,1},{12,2}, {0,0}},
				     {{10,0}, {0,0}, {0,0},{13,3}},
				     {{11,0}, {0,0},{13,2}, {0,0}},
				     {{12,0},{13,1}, {0,0}, {0,0}},
				     { {0,0}, {0,0}, {0,0},{14,3}},
				     { {0,0}, {0,0},{14,2}, {0,0}},
				     { {0,0},{14,1}, {0,0}, {0,0}},
				     {{14,0}, {0,0}, {0,0}, {0,0}}};


/*
 *  These tables represent each Is.  The first column of each row indicates
 *  the size of the set.
 *
 */
   static int	  Is_2[3][3] = { {1,0,0}, {1,1,0}, {2,0,1}};

   static int	  Is_3[7][4] = { {1,0,0,0}, {1,1,0,0}, {1,2,0,0}, {2,0,1,0},
				 {2,0,2,0}, {2,1,2,0}, {3,0,1,2}};

   static int	 Is_4[15][5] = { {1,0,0,0,0}, {1,1,0,0,0}, {1,2,0,0,0},
				 {1,3,0,0,0}, {2,0,1,0,0}, {2,0,2,0,0},
				 {2,0,3,0,0}, {2,1,2,0,0}, {2,1,3,0,0},
				 {2,2,3,0,0}, {3,0,1,2,0}, {3,0,1,3,0},
				 {3,0,2,3,0}, {3,1,2,3,0}, {4,0,1,2,3}};

/*
 *  These tables represent each Is complement. The first column of each row
 *  indicates the size of the set.
 *
 */
   static int	 IsC_2[3][2] = { {1,1}, {1,0}, {0,0}};

   static int	 IsC_3[7][3] = { {2,1,2}, {2,0,2}, {2,0,1}, {1,2,0}, {1,1,0},
				 {1,0,0}, {0,0,0}};

   static int	IsC_4[15][4] = { {3,1,2,3}, {3,0,2,3}, {3,0,1,3}, {3,0,1,2},
				 {2,2,3,0}, {2,1,3,0}, {2,1,2,0}, {2,0,3,0},
				 {2,0,2,0}, {2,0,1,0}, {1,3,0,0}, {1,2,0,0},
				 {1,1,0,0}, {1,0,0,0}, {0,0,0,0}};


   /** Call comp_sub_dist with appropriate tables according to size of P **/

   switch (m) {
      case 2:
	 size = comp_sub_dist(P, m, jo_2[0][0], Is_2[0], IsC_2[0], near_pnt,
							   near_indx, lambda);
      break;
      case 3:
	 size = comp_sub_dist(P, m, jo_3[0][0], Is_3[0], IsC_3[0], near_pnt,
							   near_indx, lambda);
      break;
      case 4:
	 size = comp_sub_dist(P, m, jo_4[0][0], Is_4[0], IsC_4[0], near_pnt,
							    near_indx, lambda);
      break;
   }

   return size;
}


/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to compute the minimum distance between two convex polytopes in
 *   3-space.
 *
 *   On Entry:
 *	   P1 - table of 3-element points containing first polytope's vertices.
 *	   m1 - number of points in P1.
 *	   P2 - table of 3-element points containing second polytope's vertices.
 *	   m2 - number of points in P2.
 *	   VP - an empty array of size 3.
 *  near_indx - a 4x2 matrix possibly containing indices of initialization
 *		points. The first column are indices into P1, and the second
 *		column are indices into P2.
 *     lambda - an empty array of size 4.
 *	   m3 - a pointer to an int, which indicates how many initial points
 *		to extract from near_indx. If 0, near_indx is ignored.
 *
 *   On Exit:
 *	 Vp   - vector difference of the two near points in P1 and P2.
 *		The length of this vector is the minimum distance between P1
 *		and P2.
 *  near_indx - updated indices into P1 and P2 which indicate the affinely
 *		independent point sets from each polytope which can be used
 *		to compute along with lambda the near points in P1 and P2
 *		as in eq. (12). These indices can be used to re-initialize
 *		dist3d in the next iteration.
 *     lambda - the lambda as in eqs. (11) & (12).
 *	   m3 - the updated number of indices for P1 and P2 in near_indx.
 *
 *   Function Return : none.
 *
 ****************************************************************************/

void dist3d(P1, m1, P2, m2, VP, near_indx, lambda, m3)
double		    P1[][3], P2[][3], VP[], lambda[];
int		    m1, m2, near_indx[][2], *m3;
{
   Boolean	    pass;
   int		    set_size, I[4], i, j, i_tab[4], j_tab[4], P1_i, P2_i, k;
   double	    Hs(), Pk[4][3], Pk_subset[4][3], Vk[3], neg_Vk[3], Cp[3],
		    Gp;

   if ((*m3) == 0) {	     /** if *m3 == 0 use single point initialization **/
      set_size = 1;
      VECSUB3(Pk[0], P1[0], P2[0]);	 /** first elementary polytope **/
      i_tab[0] = j_tab[0] = 0;
   }
   else {				 /** else use indices from near_indx **/
      for (k = 0; k < (*m3); k++) {
	 i = i_tab[k] = near_indx[k][0];
	 j = j_tab[k] = near_indx[k][1];
	 VECSUB3(Pk[k], P1[i], P2[j]);	 /** first elementary polytope **/
      }
      set_size = *m3;
   }

   pass = FALSE;
   while (!pass) {

      /** compute Vk **/

      if (set_size == 1) {
	 CPVECTOR3(Vk, Pk[0]);
	 I[0] = 0;
      }
      else
	 set_size = sub_dist(Pk, set_size, Vk, I, lambda);

      /** eq. (13) **/

      CPVECTOR3(neg_Vk, Vk);	  VECNEGATE3(neg_Vk);
      Gp = DOT3(Vk, Vk) + Hs(P1, m1, P2, m2, neg_Vk, Cp, &P1_i, &P2_i);

      /** keep track of indices for P1 and P2 **/

      for (i = 0; i < set_size; i++) {
	 j = I[i];
	 i_tab[i] = i_tab[j];	  /** j is value from member of some Is **/
	 j_tab[i] = j_tab[j];	  /** j is value from member of some Is **/
      }

      if (EQZ(Gp))		  /** Do we have a solution **/
	 pass = TRUE;
      else {
	 for (i = 0; i < set_size; i++) {
	    j = I[i];
	    CPVECTOR3(Pk_subset[i], Pk[j]);  /** extract affine subset of Pk **/
	 }
	 for (i = 0; i < set_size; i++)
	    CPVECTOR3(Pk[i], Pk_subset[i]);  /** load into Pk+1 **/

	 CPVECTOR3(Pk[i], Cp);		     /** Union of Pk+1 with Cp **/
	 i_tab[i] = P1_i;  j_tab[i] = P2_i;
	 set_size++;
      }
   }

   CPVECTOR3(VP, Vk);			  /** load VP **/
   *m3 = set_size;
   for(i = 0; i < set_size; i++) {
      near_indx[i][0] = i_tab[i];	  /** set indices of near pnt. in P1 **/
      near_indx[i][1] = j_tab[i];	  /** set indices of near pnt. in P2 **/
   }
}

/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to compute a proper separating plane between a pair of
 *   polytopes.	 The plane will be a support plane for polytope 1.
 *
 *   On Entry:
 *	couple - couple structure for a pair of polytopes.
 *
 *   On Exit:
 *	couple - containing new proper separating plane, if one was
 *		 found.
 *
 *   Function Return :
 *	result of whether a separating plane exists, or not.
 *
 ****************************************************************************/

Boolean get_new_plane(couple)
Couple		  couple;
{
   Polyhedron	  polyhedron1, polyhedron2;
   Boolean	  plane_exists;
   double	  pnts1[MAX_VERTS][3], pnts2[MAX_VERTS][3], dist,
		  u[3], v[3], lambda[4], VP[3];
   int		  i, k, m1, m2;

   plane_exists = FALSE;

   polyhedron1 = couple->polyhdrn1;    polyhedron2 = couple->polyhdrn2;

   /** Apply M1 to vertices of polytope 1 **/

   m1 = polyhedron1->m;
   for (i = 0; i < m1; i++) {
      CPVECTOR3(pnts1[i], polyhedron1->verts[i]);
      VECADD3(pnts1[i], pnts1[i], polyhedron1->trn);
   }

   /** Apply M2 to vertices of polytope 1 **/

   m2 = polyhedron2->m;
   for (i = 0; i < m2; i++) {
      CPVECTOR3(pnts2[i], polyhedron2->verts[i]);
      VECADD3(pnts2[i], pnts2[i], polyhedron2->trn);
   }

   /** solve eq. (1) for two polytopes **/

   dist3d(pnts1, m1, pnts2, m2, VP, couple->vert_indx, lambda, &couple->n);

   dist = sqrt(DOT3(VP,VP));   /** distance between polytopes **/

   if (!EQZ(dist)) {	       /** Does a separating plane exist **/
      plane_exists = TRUE;
      u[0] = u[1] = u[2] = v[0] = v[1] = v[2] = 0.0;
      for (i = 0; i < couple->n; i++) {
	 k = couple->vert_indx[i][0];
	 VECADDS3(u, lambda[i], pnts1[k], u);  /** point in P1 **/
	 k = couple->vert_indx[i][1];
	 VECADDS3(v, lambda[i], pnts2[k], v);  /** point in P2 **/
      }

      /** Store separating plane in P1's local coordinates **/

      VECADD3(u, u, polyhedron1->itrn);
      VECADD3(v, v, polyhedron1->itrn);

      /** Place separating plane in couple data structure **/

      CPVECTOR3(couple->pln_pnt1, u);
      CPVECTOR3(couple->pln_pnt2, v);
   }
   return plane_exists;
}

/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to detect if two polyhedra are intersecting.
 *
 *   On Entry:
 *	couple - couple structure for a pair of polytopes.
 *
 *   On Exit:
 *
 *   Function Return :
 *	result of whether polyhedra are intersecting or not.
 *
 ****************************************************************************/

Boolean Collision(couple)
Couple		  couple;
{
   Polyhedron	  polyhedron1, polyhedron2;
   Boolean	  collide, loop;
   double	  u[3], v[3], norm[3], d;
   int		  i, m;

   polyhedron1 = couple->polyhdrn1;	polyhedron2 = couple->polyhdrn2;
   collide = FALSE;

   if (couple->plane_exists) {

      /** Transform proper separating plane to P2 local coordinates.   **/
      /** This avoids the computational cost of applying the	       **/
      /** transformation matrix to all the vertices of P2.	       **/

      CPVECTOR3(u, couple->pln_pnt1);	CPVECTOR3(v, couple->pln_pnt2);
      VECADD3(u, u, polyhedron1->trn);	VECADD3(v, v, polyhedron1->trn);
      VECADD3(u, u, polyhedron2->itrn); VECADD3(v, v, polyhedron2->itrn);
      VECSUB3(norm, v, u);

      m = polyhedron2->m;   i = 0; loop = TRUE;
      while ((i < m) && (loop)) {

	 /** Evaluate plane equation **/

	 VECSUB3(v, polyhedron2->verts[i], u);
	 d = DOT3(v, norm);

	 if (d <= 0.0) {		 /** is P2 in opposite half-space **/
	    loop = FALSE;
	    if (!get_new_plane(couple)) {
	       collide = TRUE;		 /** Collision **/
	       couple->plane_exists = FALSE;
	    }
	 }
	 i++;
      }
   }
   else
      if (get_new_plane(couple)) {
	 couple->plane_exists = TRUE;	 /** No Collision **/
      }
      else
	 collide = TRUE;		 /** Collision **/

   return collide;
}


/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to initialize a polyhedron.
 *
 *   On Entry:
 *   polyhedron - pointer to a polyhedron structure.
 *	  verts - verts to load.
 *	      m - number of verts.
 *	     tx - x translation.
 *	     ty - y translation.
 *	     tz - z translation.
 *
 *   On Exit:
 *	polyhedron - an initialized polyhedron.
 *
 *   Function Return : none.
 *
 ****************************************************************************/

void init_polyhedron(polyhedron, verts, m, tx, ty, tz)
Polyhedron    polyhedron;
double	      *verts, tx, ty, tz;
int	      m;
{
   int	      i;
   double     *p;

   polyhedron->trn[0]  =  tx;  polyhedron->trn[1]  =  ty;
   polyhedron->trn[2]  =  tz;

   polyhedron->itrn[0] = -tx;  polyhedron->itrn[1] = -ty;
   polyhedron->itrn[2] = -tz;

   polyhedron->m = m;

   p = verts;
   for (i = 0; i < m; i++) {
      CPVECTOR3(polyhedron->verts[i], p);
      p += 3;
   }
}

/*** RJR 05/26/93 ***********************************************************
 *
 *   Function to move a polyhedron.
 *
 *   On Entry:
 *    polyhedron - pointer to a polyhedron.
 *	      tx - x translation.
 *	      ty - y translation.
 *	      tz - z translation.
 *
 *   On Exit:
 *	polyhedron - an updated polyhedron.
 *
 *   Function Return : none.
 *
 ****************************************************************************/

void move_polyhedron(polyhedron, tx, ty, tz)
Polyhedron   polyhedron;
double	     tx, ty, tz;
{
   polyhedron->trn[0]  += tx;  polyhedron->trn[1]  +=  ty;
   polyhedron->trn[2]  += tz;

   polyhedron->itrn[0] -= tx;  polyhedron->itrn[1] -=  ty;
   polyhedron->itrn[2] -= tz;
}

/*** RJR 05/26/93 ***********************************************************
 *
 *   This is the Main Program for the Collision Detection example. This test
 *   program creates the vertices of three polyhedra: a sphere, a box, and a
 *   cylinder. The three polyhedra oscillate back and forth along the x-axis.
 *   A collision test is done after each movement on each pair of polyhedra.
 *   This test program was run on an SGI Onyx/4 and an SGI 4D/80.  A total of
 *   30,000 collision detection tests were performed.  There were 3,160
 *   collisions detected. The dist3d function was called in 14% of the
 *   collision tests.  The average number of iterations in dist3d was 1.7.
 *   The above functions are designed to compute accurate solutions when
 *   the polyhedra are simple and convex.  The functions will work on
 *   concave polyhedra, but the solutions are computed using the convex hulls
 *   of the concave polyhedra.	In this case when the algorithm returns a
 *   disjoint result it is exact, but when it returns an intersection result
 *   it is approximate.
 *
 ****************************************************************************/
main()
{
   Polyhedron	  Polyhedron1, Polyhedron2, Polyhedron3;
   Couple	  Couple1, Couple2, Couple3;
   double	  xstp1, xstp2, xstp3;
   int		  i, steps;
   long		  hits = 0;

   /*** Initialize the 3 test polyhedra ***/

   mak_box(box);
   mak_cyl(cyl);
   mak_sph(sphere);

   Polyhedron1 = (Polyhedron)malloc(sizeof(struct polyhedron));
   init_polyhedron(Polyhedron1, sphere, 342,  0.0, 0.0, 0.0);

   Polyhedron2 = (Polyhedron)malloc(sizeof(struct polyhedron));
   init_polyhedron(Polyhedron2, box, 8, 50.0, 0.0, 0.0);

   Polyhedron3 = (Polyhedron)malloc(sizeof(struct polyhedron));
   init_polyhedron(Polyhedron3, cyl, 36, -50.0, 0.0, 0.0);

   Couple1 = (Couple)malloc(sizeof(struct couple));
   Couple1->polyhdrn1 = Polyhedron1;   Couple1->polyhdrn2 = Polyhedron2;
   Couple1->n = 0;
   Couple1->plane_exists = FALSE;

   Couple2 = (Couple)malloc(sizeof(struct couple));
   Couple2->polyhdrn1 = Polyhedron1;   Couple2->polyhdrn2 = Polyhedron3;
   Couple2->n = 0;
   Couple2->plane_exists = FALSE;

   Couple3 = (Couple)malloc(sizeof(struct couple));
   Couple3->polyhdrn1 = Polyhedron3;   Couple3->polyhdrn2 = Polyhedron2;
   Couple3->n = 0;
   Couple3->plane_exists = FALSE;

   /** Perform Collision Tests **/

   xstp1 = 1.0;	 xstp2 = 5.0; xstp3 = 10.0;  steps = 10000;

   for (i = 0; i < steps; i++) {
      move_polyhedron(Polyhedron1, xstp1, 0.0, 0.0);
      move_polyhedron(Polyhedron2, xstp2, 0.0, 0.0);
      move_polyhedron(Polyhedron3, xstp3, 0.0, 0.0);

      if (Collision(Couple1))
	 hits++;
      if (Collision(Couple2))
	 hits++;
      if (Collision(Couple3))
	 hits++;

      if (ABS(Polyhedron1->trn[0]) > 100.0)
	 xstp1 = -xstp1;
      if (ABS(Polyhedron2->trn[0]) > 100.0)
	 xstp2 = -xstp2;
      if (ABS(Polyhedron3->trn[0]) > 100.0)
	 xstp3 = -xstp3;
   }
   printf("number of tests = %d\n",(steps * 3));
   printf("number of hits = %ld\n", hits);
}
