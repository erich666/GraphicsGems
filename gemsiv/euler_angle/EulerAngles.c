/**** EulerAngles.c - Convert Euler angles to/from matrix or quat ****/
/* Ken Shoemake, 1993 */
#include <math.h>
#include <float.h>
#include "EulerAngles.h"

EulerAngles Eul_(float ai, float aj, float ah, int order)
{
    EulerAngles ea;
    ea.x = ai; ea.y = aj; ea.z = ah;
    ea.w = (float)order;
    return (ea);
}
/* Construct quaternion from Euler angles (in radians). */
Quat Eul_ToQuat(EulerAngles ea)
{
    Quat qu;
    float a[3], ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int i,j,k,h,n,s,f;
    EulGetOrd(ea.w,i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    if (n==EulParOdd) ea.y = -ea.y;
    ti = ea.x*0.5f; tj = ea.y*0.5f; th = ea.z*0.5f;
    ci = cosf(ti);  cj = cosf(tj);  ch = cosf(th);
    si = sinf(ti);  sj = sinf(tj);  sh = sinf(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
    if (s==EulRepYes) {
	a[i] = cj*(cs + sc);	/* Could speed up with */
	a[j] = sj*(cc + ss);	/* trig identities. */
	a[k] = sj*(cs - sc);
	qu.w = cj*(cc - ss);
    } else {
	a[i] = cj*sc - sj*cs;
	a[j] = cj*ss + sj*cc;
	a[k] = cj*cs - sj*sc;
	qu.w = cj*cc + sj*ss;
    }
    if (n==EulParOdd) a[j] = -a[j];
    qu.x = a[X]; qu.y = a[Y]; qu.z = a[Z];
    return (qu);
}

/* Construct matrix from Euler angles (in radians). */
void Eul_ToHMatrix(EulerAngles ea, HMatrix M)
{
    float ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int i,j,k,h,n,s,f;
    EulGetOrd(ea.w,i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = -ea.y; ea.z = -ea.z;}
    ti = ea.x;	  tj = ea.y;	th = ea.z;
    ci = cosf(ti); cj = cosf(tj); ch = cosf(th);
    si = sinf(ti); sj = sinf(tj); sh = sinf(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
    if (s==EulRepYes) {
	M[i][i] = cj;	  M[i][j] =  sj*si;    M[i][k] =  sj*ci;
	M[j][i] = sj*sh;  M[j][j] = -cj*ss+cc; M[j][k] = -cj*cs-sc;
	M[k][i] = -sj*ch; M[k][j] =  cj*sc+cs; M[k][k] =  cj*cc-ss;
    } else {
	M[i][i] = cj*ch; M[i][j] = sj*sc-cs; M[i][k] = sj*cc+ss;
	M[j][i] = cj*sh; M[j][j] = sj*ss+cc; M[j][k] = sj*cs-sc;
	M[k][i] = -sj;	 M[k][j] = cj*si;    M[k][k] = cj*ci;
    }
    M[W][X]=M[W][Y]=M[W][Z]=M[X][W]=M[Y][W]=M[Z][W]=0.f; M[W][W]=1.f;
}

/* Convert matrix to Euler angles (in radians). */
EulerAngles Eul_FromHMatrix(HMatrix M, int order)
{
    EulerAngles ea;
    int i,j,k,h,n,s,f;
    EulGetOrd(order,i,j,k,h,n,s,f);
    if (s==EulRepYes) {
	float sy = sqrtf(M[i][j]*M[i][j] + M[i][k]*M[i][k]);
	if (sy > 16*FLT_EPSILON) {
	    ea.x = atan2f(M[i][j], M[i][k]);
	    ea.y = atan2f(sy, M[i][i]);
	    ea.z = atan2f(M[j][i], -M[k][i]);
	} else {
	    ea.x = atan2f(-M[j][k], M[j][j]);
	    ea.y = atan2f(sy, M[i][i]);
	    ea.z = 0;
	}
    } else {
	float cy = sqrtf(M[i][i]*M[i][i] + M[j][i]*M[j][i]);
	if (cy > 16*FLT_EPSILON) {
	    ea.x = atan2f(M[k][j], M[k][k]);
	    ea.y = atan2f(-M[k][i], cy);
	    ea.z = atan2f(M[j][i], M[i][i]);
	} else {
	    ea.x = atan2f(-M[j][k], M[j][j]);
	    ea.y = atan2f(-M[k][i], cy);
	    ea.z = 0;
	}
    }
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    ea.w = (float)order;
    return (ea);
}

/* Convert quaternion to Euler angles (in radians). */
EulerAngles Eul_FromQuat(Quat q, int order)
{
    HMatrix M;
    float Nq = q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
    float s = (Nq > 0.f) ? (2.f / Nq) : 0.f;
    float xs = q.x*s,	  ys = q.y*s,	 zs = q.z*s;
    float wx = q.w*xs,	  wy = q.w*ys,	 wz = q.w*zs;
    float xx = q.x*xs,	  xy = q.x*ys,	 xz = q.x*zs;
    float yy = q.y*ys,	  yz = q.y*zs,	 zz = q.z*zs;
    M[X][X] = 1.f - (yy + zz); M[X][Y] = xy - wz; M[X][Z] = xz + wy;
    M[Y][X] = xy + wz; M[Y][Y] = 1.f - (xx + zz); M[Y][Z] = yz - wx;
    M[Z][X] = xz - wy; M[Z][Y] = yz + wx; M[Z][Z] = 1.f - (xx + yy);
    M[W][X]=M[W][Y]=M[W][Z]=M[X][W]=M[Y][W]=M[Z][W]=0.f; M[W][W]=1.f;
    return (Eul_FromHMatrix(M, order));
}
