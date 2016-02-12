/**** Decompose.c ****/
/* Ken Shoemake, 1993 */
#include <math.h>
#include "Decompose.h"

/******* Matrix Preliminaries *******/

/** Fill out 3x3 matrix to 4x4 **/
#define mat_pad(A) (A[W][X]=A[X][W]=A[W][Y]=A[Y][W]=A[W][Z]=A[Z][W]=0,A[W][W]=1)

/** Copy nxn matrix A to C using "gets" for assignment **/
#define mat_copy(C,gets,A,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    C[i][j] gets (A[i][j]);}

/** Copy transpose of nxn matrix A to C using "gets" for assignment **/
#define mat_tpose(AT,gets,A,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    AT[i][j] gets (A[j][i]);}

/** Assign nxn matrix C the element-wise combination of A and B using "op" **/
#define mat_binop(C,gets,A,op,B,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    C[i][j] gets (A[i][j]) op (B[i][j]);}

/** Multiply the upper left 3x3 parts of A and B to get AB **/
void mat_mult(HMatrix A, HMatrix B, HMatrix AB)
{
    int i, j;
    for (i=0; i<3; i++) for (j=0; j<3; j++)
	AB[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j];
}

/** Return dot product of length 3 vectors va and vb **/
float vdot(float *va, float *vb)
{
    return (va[0]*vb[0] + va[1]*vb[1] + va[2]*vb[2]);
}

/** Set v to cross product of length 3 vectors va and vb **/
void vcross(float *va, float *vb, float *v)
{
    v[0] = va[1]*vb[2] - va[2]*vb[1];
    v[1] = va[2]*vb[0] - va[0]*vb[2];
    v[2] = va[0]*vb[1] - va[1]*vb[0];
}

/** Set MadjT to transpose of inverse of M times determinant of M **/
void adjoint_transpose(HMatrix M, HMatrix MadjT)
{
    vcross(M[1], M[2], MadjT[0]);
    vcross(M[2], M[0], MadjT[1]);
    vcross(M[0], M[1], MadjT[2]);
}

/******* Quaternion Preliminaries *******/

/* Construct a (possibly non-unit) quaternion from real components. */
Quat Qt_(float x, float y, float z, float w)
{
    Quat qq;
    qq.x = x; qq.y = y; qq.z = z; qq.w = w;
    return (qq);
}

/* Return conjugate of quaternion. */
Quat Qt_Conj(Quat q)
{
    Quat qq;
    qq.x = -q.x; qq.y = -q.y; qq.z = -q.z; qq.w = q.w;
    return (qq);
}

/* Return quaternion product qL * qR.  Note: order is important!
 * To combine rotations, use the product Mul(qSecond, qFirst),
 * which gives the effect of rotating by qFirst then qSecond. */
Quat Qt_Mul(Quat qL, Quat qR)
{
    Quat qq;
    qq.w = qL.w*qR.w - qL.x*qR.x - qL.y*qR.y - qL.z*qR.z;
    qq.x = qL.w*qR.x + qL.x*qR.w + qL.y*qR.z - qL.z*qR.y;
    qq.y = qL.w*qR.y + qL.y*qR.w + qL.z*qR.x - qL.x*qR.z;
    qq.z = qL.w*qR.z + qL.z*qR.w + qL.x*qR.y - qL.y*qR.x;
    return (qq);
}

/* Return product of quaternion q by scalar w. */
Quat Qt_Scale(Quat q, float w)
{
    Quat qq;
    qq.w = q.w*w; qq.x = q.x*w; qq.y = q.y*w; qq.z = q.z*w;
    return (qq);
}

/* Construct a unit quaternion from rotation matrix.  Assumes matrix is
 * used to multiply column vector on the left: vnew = mat vold.	 Works
 * correctly for right-handed coordinate system and right-handed rotations.
 * Translation and perspective components ignored. */
Quat Qt_FromMatrix(HMatrix mat)
{
    /* This algorithm avoids near-zero divides by looking for a large component
     * - first w, then x, y, or z.  When the trace is greater than zero,
     * |w| is greater than 1/2, which is as small as a largest component can be.
     * Otherwise, the largest diagonal entry corresponds to the largest of |x|,
     * |y|, or |z|, one of which must be larger than |w|, and at least 1/2. */
    Quat qu;
    float tr, s;

    tr = mat[X][X] + mat[Y][Y]+ mat[Z][Z];
    if (tr >= 0.f) {
	    s = sqrtf(tr + mat[W][W]);
	    qu.w = s*0.5f;
	    s = 0.5f / s;
	    qu.x = (mat[Z][Y] - mat[Y][Z]) * s;
	    qu.y = (mat[X][Z] - mat[Z][X]) * s;
	    qu.z = (mat[Y][X] - mat[X][Y]) * s;
	} else {
	    int h = X;
	    if (mat[Y][Y] > mat[X][X]) h = Y;
	    if (mat[Z][Z] > mat[h][h]) h = Z;
	    switch (h) {
#define caseMacro(i,j,k,I,J,K) \
	    case I:\
		s = sqrtf( (mat[I][I] - (mat[J][J]+mat[K][K])) + mat[W][W] );\
		qu.i = s*0.5f;\
		s = 0.5f / s;\
		qu.j = (mat[I][J] + mat[J][I]) * s;\
		qu.k = (mat[K][I] + mat[I][K]) * s;\
		qu.w = (mat[K][J] - mat[J][K]) * s;\
		break
	    caseMacro(x,y,z,X,Y,Z);
	    caseMacro(y,z,x,Y,Z,X);
	    caseMacro(z,x,y,Z,X,Y);
	    }
	}
    if (mat[W][W] != 1.f) qu = Qt_Scale(qu, 1.f/sqrtf(mat[W][W]));
    return (qu);
}
/******* Decomp Auxiliaries *******/

static HMatrix mat_id = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

/** Compute either the 1 or infinity norm of M, depending on tpose **/
float mat_norm(HMatrix M, int tpose)
{
    int i;
    float sum, max;
    max = 0.f;
    for (i=0; i<3; i++) {
	if (tpose) sum = fabsf(M[0][i])+fabsf(M[1][i])+fabsf(M[2][i]);
	else	   sum = fabsf(M[i][0])+fabsf(M[i][1])+fabsf(M[i][2]);
	if (max<sum) max = sum;
    }
    return max;
}

float norm_inf(HMatrix M) {return mat_norm(M, 0);}
float norm_one(HMatrix M) {return mat_norm(M, 1);}

/** Return index of column of M containing maximum abs entry, or -1 if M=0 **/
int find_max_col(HMatrix M)
{
    float abs, max;
    int i, j, col;
    max = 0.f; col = -1;
    for (i=0; i<3; i++) for (j=0; j<3; j++) {
	abs = M[i][j]; if (abs<0.f) abs = -abs;
	if (abs>max) {max = abs; col = j;}
    }
    return col;
}

/** Setup u for Household reflection to zero all v components but first **/
void make_reflector(float *v, float *u)
{
    float s = sqrtf(vdot(v, v));
    u[0] = v[0]; u[1] = v[1];
    u[2] = v[2] + ((v[2]<0.f) ? -s : s);
    s = sqrtf(2.f/vdot(u, u));
    u[0] = u[0]*s; u[1] = u[1]*s; u[2] = u[2]*s;
}

/** Apply Householder reflection represented by u to column vectors of M **/
void reflect_cols(HMatrix M, float *u)
{
    int i, j;
    for (i=0; i<3; i++) {
	float s = u[0]*M[0][i] + u[1]*M[1][i] + u[2]*M[2][i];
	for (j=0; j<3; j++) M[j][i] -= u[j]*s;
    }
}
/** Apply Householder reflection represented by u to row vectors of M **/
void reflect_rows(HMatrix M, float *u)
{
    int i, j;
    for (i=0; i<3; i++) {
	float s = vdot(u, M[i]);
	for (j=0; j<3; j++) M[i][j] -= u[j]*s;
    }
}

/** Find orthogonal factor Q of rank 1 (or less) M **/
void do_rank1(HMatrix M, HMatrix Q)
{
    float v1[3], v2[3], s;
    int col;
    mat_copy(Q,=,mat_id,4);
    /* If rank(M) is 1, we should find a non-zero column in M */
    col = find_max_col(M);
    if (col<0) return; /* Rank is 0 */
    v1[0] = M[0][col]; v1[1] = M[1][col]; v1[2] = M[2][col];
    make_reflector(v1, v1); reflect_cols(M, v1);
    v2[0] = M[2][0]; v2[1] = M[2][1]; v2[2] = M[2][2];
    make_reflector(v2, v2); reflect_rows(M, v2);
    s = M[2][2];
    if (s<0.f) Q[2][2] = -1.f;
    reflect_cols(Q, v1); reflect_rows(Q, v2);
}

/** Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose **/
void do_rank2(HMatrix M, HMatrix MadjT, HMatrix Q)
{
    float v1[3], v2[3];
    float w, x, y, z, c, s, d;
    int col;
    /* If rank(M) is 2, we should find a non-zero column in MadjT */
    col = find_max_col(MadjT);
    if (col<0) {do_rank1(M, Q); return;} /* Rank<2 */
    v1[0] = MadjT[0][col]; v1[1] = MadjT[1][col]; v1[2] = MadjT[2][col];
    make_reflector(v1, v1); reflect_cols(M, v1);
    vcross(M[0], M[1], v2);
    make_reflector(v2, v2); reflect_rows(M, v2);
    w = M[0][0]; x = M[0][1]; y = M[1][0]; z = M[1][1];
    if (w*z>x*y) {
	c = z+w; s = y-x; d = sqrtf(c*c+s*s); c = c/d; s = s/d;
	Q[0][0] = Q[1][1] = c; Q[0][1] = -(Q[1][0] = s);
    } else {
	c = z-w; s = y+x; d = sqrtf(c*c+s*s); c = c/d; s = s/d;
	Q[0][0] = -(Q[1][1] = c); Q[0][1] = Q[1][0] = s;
    }
    Q[0][2] = Q[2][0] = Q[1][2] = Q[2][1] = 0.f; Q[2][2] = 1.f;
    reflect_cols(Q, v1); reflect_rows(Q, v2);
}


/******* Polar Decomposition *******/

/* Polar Decomposition of 3x3 matrix in 4x4,
 * M = QS.  See Nicholas Higham and Robert S. Schreiber,
 * Fast Polar Decomposition of An Arbitrary Matrix,
 * Technical Report 88-942, October 1988,
 * Department of Computer Science, Cornell University.
 */
float polar_decomp(HMatrix M, HMatrix Q, HMatrix S)
{
#define TOL 1.0e-6
    HMatrix Mk, MadjTk, Ek;
    float det, M_one, M_inf, MadjT_one, MadjT_inf, E_one, gamma, g1, g2;
    int i, j;
    mat_tpose(Mk,=,M,3);
    M_one = norm_one(Mk);  M_inf = norm_inf(Mk);
    do {
	adjoint_transpose(Mk, MadjTk);
	det = vdot(Mk[0], MadjTk[0]);
	if (det==0.f) {do_rank2(Mk, MadjTk, Mk); break;}
	MadjT_one = norm_one(MadjTk); MadjT_inf = norm_inf(MadjTk);
	gamma = sqrtf(sqrtf((MadjT_one*MadjT_inf)/(M_one*M_inf))/fabsf(det));
	g1 = gamma*0.5f;
	g2 = 0.5f/(gamma*det);
	mat_copy(Ek,=,Mk,3);
	mat_binop(Mk,=,g1*Mk,+,g2*MadjTk,3);
	mat_copy(Ek,-=,Mk,3);
	E_one = norm_one(Ek);
	M_one = norm_one(Mk);  M_inf = norm_inf(Mk);
    } while (E_one>(M_one*TOL));
    mat_tpose(Q,=,Mk,3); mat_pad(Q);
    mat_mult(Mk, M, S);	 mat_pad(S);
    for (i=0; i<3; i++) for (j=i; j<3; j++)
	S[i][j] = S[j][i] = 0.5f*(S[i][j]+S[j][i]);
    return (det);
}

















/******* Spectral Decomposition *******/

/* Compute the spectral decomposition of symmetric positive semi-definite S.
 * Returns rotation in U and scale factors in result, so that if K is a diagonal
 * matrix of the scale factors, then S = U K (U transpose). Uses Jacobi method.
 * See Gene H. Golub and Charles F. Van Loan. Matrix Computations. Hopkins 1983.
 */
HVect spect_decomp(HMatrix S, HMatrix U)
{
    HVect kv;
    float Diag[3],OffD[3]; /* OffD is off-diag (by omitted index) */
    float g,h,fabsh,fabsOffDi,t,theta,c,s,tau,ta,OffDq,a,b;
    static char nxt[] = {Y,Z,X};
    int sweep, i, j;
    mat_copy(U,=,mat_id,4);
    Diag[X] = S[X][X]; Diag[Y] = S[Y][Y]; Diag[Z] = S[Z][Z];
    OffD[X] = S[Y][Z]; OffD[Y] = S[Z][X]; OffD[Z] = S[X][Y];
    for (sweep=20; sweep>0; sweep--) {
	float sm = fabsf(OffD[X])+fabsf(OffD[Y])+fabsf(OffD[Z]);
	if (sm==0.f) break;
	for (i=Z; i>=X; i--) {
	    int p = nxt[i]; int q = nxt[p];
	    fabsOffDi = fabsf(OffD[i]);
	    g = 100.f*fabsOffDi;
	    if (fabsOffDi>0.f) {
		h = Diag[q] - Diag[p];
		fabsh = fabsf(h);
		if (fabsh+g==fabsh) {
		    t = OffD[i]/h;
		} else {
		    theta = 0.5f*h/OffD[i];
		    t = 1.f/(fabsf(theta)+sqrtf(theta*theta+1.f));
		    if (theta<0.f) t = -t;
		}
		c = 1.f/sqrtf(t*t+1.f); s = t*c;
		tau = s/(c+1.f);
		ta = t*OffD[i]; OffD[i] = 0.f;
		Diag[p] -= ta; Diag[q] += ta;
		OffDq = OffD[q];
		OffD[q] -= s*(OffD[p] + tau*OffD[q]);
		OffD[p] += s*(OffDq   - tau*OffD[p]);
		for (j=Z; j>=X; j--) {
		    a = U[j][p]; b = U[j][q];
		    U[j][p] -= s*(b + tau*a);
		    U[j][q] += s*(a - tau*b);
		}
	    }
	}
    }
    kv.x = Diag[X]; kv.y = Diag[Y]; kv.z = Diag[Z]; kv.w = 1.f;
    return (kv);
}

/******* Spectral Axis Adjustment *******/

/* Given a unit quaternion, q, and a scale vector, k, find a unit quaternion, p,
 * which permutes the axes and turns freely in the plane of duplicate scale
 * factors, such that q p has the largest possible w component, i.e. the
 * smallest possible angle. Permutes k's components to go with q p instead of q.
 * See Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
 * Proceedings of Graphics Interface 1992. Details on p. 262-263.
 */
Quat snuggle(Quat q, HVect *k)
{
#define SQRTHALF (0.7071067811865475244f)
#define sgn(n,v)    ((n)?-(v):(v))
#define swap(a,i,j) {a[3]=a[i]; a[i]=a[j]; a[j]=a[3];}
#define cycle(a,p)  if (p) {a[3]=a[0]; a[0]=a[1]; a[1]=a[2]; a[2]=a[3];}\
		    else   {a[3]=a[2]; a[2]=a[1]; a[1]=a[0]; a[0]=a[3];}
    Quat p;
    float ka[4];
    int i, turn = -1;
    ka[X] = k->x; ka[Y] = k->y; ka[Z] = k->z;
    if (ka[X]==ka[Y]) {if (ka[X]==ka[Z]) turn = W; else turn = Z;}
    else {if (ka[X]==ka[Z]) turn = Y; else if (ka[Y]==ka[Z]) turn = X;}
    if (turn>=0) {
	Quat qtoz, qp;
	unsigned neg[3], win;
	float mag[3], t;
	static Quat qxtoz = {0,SQRTHALF,0,SQRTHALF};
	static Quat qytoz = {SQRTHALF,0,0,SQRTHALF};
	static Quat qppmm = { 0.5f, 0.5f,-0.5f,-0.5f};
	static Quat qpppp = { 0.5f, 0.5f, 0.5f, 0.5f};
	static Quat qmpmm = {-0.5f, 0.5f,-0.5f,-0.5f};
	static Quat qpppm = { 0.5f, 0.5f, 0.5f,-0.5f};
	static Quat q0001 = { 0.f, 0.f, 0.f, 1.f};
	static Quat q1000 = { 1.f, 0.f, 0.f, 0.f};
	switch (turn) {
	default: return (Qt_Conj(q));
	case X: q = Qt_Mul(q, qtoz = qxtoz); swap(ka,X,Z) break;
	case Y: q = Qt_Mul(q, qtoz = qytoz); swap(ka,Y,Z) break;
	case Z: qtoz = q0001; break;
	}
	q = Qt_Conj(q);
	mag[0] = (float)q.z*q.z+(float)q.w*q.w-0.5f;
	mag[1] = (float)q.x*q.z-(float)q.y*q.w;
	mag[2] = (float)q.y*q.z+(float)q.x*q.w;
	for (i=0; i<3; i++) if ((neg[i] = (mag[i]<0.f))) mag[i] = -mag[i];
	if (mag[0]>mag[1]) {if (mag[0]>mag[2]) win = 0; else win = 2;}
	else		   {if (mag[1]>mag[2]) win = 1; else win = 2;}
	switch (win) {
	case 0: if (neg[0]) p = q1000; else p = q0001; break;
	case 1: if (neg[1]) p = qppmm; else p = qpppp; cycle(ka,0) break;
	case 2: if (neg[2]) p = qmpmm; else p = qpppm; cycle(ka,1) break;
	}
	qp = Qt_Mul(q, p);
	t = sqrtf(mag[win]+0.5f);
	p = Qt_Mul(p, Qt_(0.f,0.f,-qp.z/t,qp.w/t));
	p = Qt_Mul(qtoz, Qt_Conj(p));
    } else {
	float qa[4], pa[4];
	unsigned lo, hi, neg[4], par = 0;
	float all, big, two;
	qa[0] = q.x; qa[1] = q.y; qa[2] = q.z; qa[3] = q.w;
	for (i=0; i<4; i++) {
	    pa[i] = 0.f;
	    if ((neg[i] = (qa[i]<0.f))) qa[i] = -qa[i];
	    par ^= neg[i];
	}
	/* Find two largest components, indices in hi and lo */
	if (qa[0]>qa[1]) lo = 0; else lo = 1;
	if (qa[2]>qa[3]) hi = 2; else hi = 3;
	if (qa[lo]>qa[hi]) {
	    if (qa[lo^1]>qa[hi]) {hi = lo; lo ^= 1;}
	    else {hi ^= lo; lo ^= hi; hi ^= lo;}
	} else {if (qa[hi^1]>qa[lo]) lo = hi^1;}
	all = (qa[0]+qa[1]+qa[2]+qa[3])*0.5f;
	two = (qa[hi]+qa[lo])*SQRTHALF;
	big = qa[hi];
	if (all>two) {
	    if (all>big) {/*all*/
		{int i; for (i=0; i<4; i++) pa[i] = sgn(neg[i], 0.5f);}
		cycle(ka,par)
	    } else {/*big*/ pa[hi] = sgn(neg[hi],1.f);}
	} else {
	    if (two>big) {/*two*/
		pa[hi] = sgn(neg[hi],SQRTHALF); pa[lo] = sgn(neg[lo], SQRTHALF);
		if (lo>hi) {hi ^= lo; lo ^= hi; hi ^= lo;}
		/* This wild code is simply defining a fixed array from a string.
		** The code in the braces is equivalent to:
		** hi = (lo+1)%3; lo = (lo+2)%3; */
		if (hi==W) {hi = "\001\002\000"[lo]; lo = 3-hi-lo;}
		swap(ka,hi,lo)
	    } else {/*big*/ pa[hi] = sgn(neg[hi],1.f);}
	}
	p.x = -pa[0]; p.y = -pa[1]; p.z = -pa[2]; p.w = pa[3];
    }
    k->x = ka[X]; k->y = ka[Y]; k->z = ka[Z];
    return (p);
}











/******* Decompose Affine Matrix *******/

/* Decompose 4x4 affine matrix A as TFRUK(U transpose), where t contains the
 * translation components, q contains the rotation R, u contains U, k contains
 * scale factors, and f contains the sign of the determinant.
 * Assumes A transforms column vectors in right-handed coordinates.
 * See Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
 * Proceedings of Graphics Interface 1992.
 */
void decomp_affine(HMatrix A, AffineParts *parts)
{
    HMatrix Q, S, U;
    Quat p;
    float det;
    parts->t = Qt_(A[X][W], A[Y][W], A[Z][W], 0);
    det = polar_decomp(A, Q, S);
    if (det<0.f) {
	mat_copy(Q,=,-Q,3);
	parts->f = -1;
    } else parts->f = 1;
    parts->q = Qt_FromMatrix(Q);
    parts->k = spect_decomp(S, U);
    parts->u = Qt_FromMatrix(U);
    p = snuggle(parts->u, &parts->k);
    parts->u = Qt_Mul(parts->u, p);
}

/******* Invert Affine Decomposition *******/

/* Compute inverse of affine decomposition.
 */
void invert_affine(AffineParts *parts, AffineParts *inverse)
{
    Quat t, p;
    inverse->f = parts->f;
    inverse->q = Qt_Conj(parts->q);
    inverse->u = Qt_Mul(parts->q, parts->u);
    inverse->k.x = (parts->k.x==0.f) ? 0.f : 1.f/parts->k.x;
    inverse->k.y = (parts->k.y==0.f) ? 0.f : 1.f/parts->k.y;
    inverse->k.z = (parts->k.z==0.f) ? 0.f : 1.f/parts->k.z;
    inverse->k.w = parts->k.w;
    t = Qt_(-parts->t.x, -parts->t.y, -parts->t.z, 0);
    t = Qt_Mul(Qt_Conj(inverse->u), Qt_Mul(t, inverse->u));
    t = Qt_(inverse->k.x*t.x, inverse->k.y*t.y, inverse->k.z*t.z, 0);
    p = Qt_Mul(inverse->q, inverse->u);
    t = Qt_Mul(p, Qt_Mul(t, Qt_Conj(p)));
    inverse->t = (inverse->f>0.f) ? t : Qt_(-t.x, -t.y, -t.z, 0);
}
