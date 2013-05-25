/*******************************
 * Scanline-rejection routines *
 * for spheres and polygons.   *
 * By Tomas Moller             *
 *******************************/
#include "vector.h"

#define EPSILON 1e-6            /* a small number */
#define INFINITY 1e8            /* a large number */
enum StatusFlag {OnScanline,OffScanline,Interval,NeverAgain};

struct FirstHitStatus
{
        StatusFlag Flag;                /* se enum above */
        short IntervalMin,IntervalMax;  /* the interval */
};

/******************************************************************
* SphereComputeFirstHitStatus - computes the first hit status     *
*   for a sphere and the Interval (if any) for the scanline-plane *
*   given by the first two parameters.                            *
*                                                                 *
* Entry:                                                          *
*   Ns - normal of the scanline-plane                             *
*   ds - "d-value" for the scanline-plane                         *
*   LeftMost - the leftmost point on the scanline. Referred to    *
*       as L in the text.                                         *
*   ScanLineDir - the direction of the scanline in space.         *
*       Observe that it is constructed by subtracting two         *
*       adjacent points on the scanline from each other.          *
*   width - number of pixels per scanline                         *
*   Ui,Vi - the indices to the uv-plane. 0==x,1==y,2==z           *
*   Eyepos - the positition of the eye (or camera)                *
*   Sphcen - the centre of the sphere                             *
*   Sr -  the radius of the sphere                                *
*                                                                 *
* The function returns a struct FirstHitStatus with all           *
* necessary information.                                          *
******************************************************************/
struct FirstHitStatus SphereComputeFirstHitStatus(Vector &Ns,float ds,
        Vector LeftMost,Vector ScanLineDir,int width,int Ui,int Vi,
        Vector Eyepos,Vector Sphcen,float Sr)
{
    struct FirstHitStatus FHstatus;
    float signed_dist=Ns*Sphcen+ds;
    if(signed_dist>Sr) FHstatus.Flag=NeverAgain;    /* sphere is above plane */
    else if(signed_dist<-Sr) FHstatus.Flag=OffScanline; /* below plane */
    else
    {
        Vector D,H,Cc,Nd;                          /* Cc=Circle Origo */
        float sintheta,costheta,centerdist;
        float cr2,d2,t1,t2,t3,V1v,V1u,V2v,V2u;     /* squared circle radius */
        Cc=Sphcen-Ns*signed_dist;                               
        cr2=Sr*Sr-signed_dist*signed_dist;         /* the * is dot-product */
        D=Cc-Eyepos;
        d2=D*D;                                    /* D dot D=squared length of D */
        if(d2<=cr2)
        {                                          /* we are inside the sphere */
            FHstatus.Flag=OnScanline;
            return FHstatus;
        }
        H=D%Ns;                                    /* % = cross product */
        t1=cr2/d2;
        sintheta=sqrt(t1);
        costheta=sqrt(1-t1);
        t1=H[Ui]*sintheta;
        t2=D[Ui]*costheta;
        V1u=t1+t2;                                 /* compute V1 and V2 */
        V2u=-t1+t2;
        t1=H[Vi]*sintheta;
        t2=D[Vi]*costheta;
        V1v=t1+t2;
        V2v=-t1+t2;
        t1=LeftMost[Ui]-Eyepos[Ui];                /* some constants */
        t2=Eyepos[Vi]-LeftMost[Vi];
        t3=V1u*ScanLineDir[Vi]-V1v*ScanLineDir[Ui];
        if(t3!=0.0)                                /* V1 parallel to ScanLineDir ? */
        {
            FHstatus.IntervalMax=(int)floor((V1v*t1+V1u*t2)/t3);
            t3=V2u*ScanLineDir[Vi]-V2v*ScanLineDir[Ui];
            if(t3==0.0) FHstatus.IntervalMin=0;
            else FHstatus.IntervalMin=(int)ceil((V2v*t1+V2u*t2)/t3);
        }
        else                                      /* V1 parallel to ScanLineDir */
        {
            t3=V2u*ScanLineDir[Vi]-V2v*ScanLineDir[Ui];
            FHstatus.IntervalMin=(int)ceil((V2v*t1+V2u*t2)/t3);
            FHstatus.IntervalMax=width-1;
        }
        /* check if interval is valid and set status */
        if(FHstatus.IntervalMin>=width || FHstatus.IntervalMax<0)
             FHstatus.Flag=OffScanline;
        else
        {
            if(FHstatus.IntervalMax>=width) FHstatus.IntervalMax=width-1;
            if(FHstatus.IntervalMin<0) FHstatus.IntervalMin=0;
            if(FHstatus.IntervalMin==0 && FHstatus.IntervalMax==width-1)
                FHstatus.Flag=OnScanline;
            else FHstatus.Flag=Interval;
        }
    }
    if(FHstatus.IntervalMin>FHstatus.IntervalMax) FHstatus.Flag=OffScanline;
    return FHstatus;
}

/* Macro used by PolyComputeFirstHitStatus. It projects
*  the point (x,y,z) onto the scanline and saves the
*  result in [realIntervalMin,realIntervalMax].
*/

#define PROJECTPOINT(x,y,z)                                              \
    switch(Ui)                                                           \
    {                                                                    \
        case Xi:u=x; break;                                              \
        case Yi:u=y; break;                                              \
        case Zi:u=z; break;                                              \
    }                                                                    \
    switch(Vi)                                                           \
    {                                                                    \
        case Xi:v=x; break;                                              \
        case Yi:v=y; break;                                              \
        case Zi:v=z; break;                                              \
    }                                                                    \
    denom=ScanLineDir[Ui]*(Eyepos[Vi]-v)-ScanLineDir[Vi]*(Eyepos[Ui]-u); \
    if(denom==0.0)                                                       \
    {                                                                    \
        if(ScanLineDir[Ui]!=0.0) alpha=(u-Eyepos[Ui])/ScanLineDir[Ui];   \
        else alpha=(v-Eyepos[Vi])/ScanLineDir[Vi];                       \
        if(alpha>0.0) realIntervalMax=width-1;                           \
        else realIntervalMin=0;                                          \
    }                                                                    \
    else                                                                 \
    {                                                                    \
        alpha=(Eyepos[Ui]-LeftMost[Ui])*(Eyepos[Vi]-v);                  \
        alpha-=(Eyepos[Ui]-u)*(Eyepos[Vi]-LeftMost[Vi]);                 \
        alpha/=denom;                                                    \
        if(alpha>realIntervalMax) realIntervalMax=alpha;                 \
        if(alpha<realIntervalMin) realIntervalMin=alpha;                 \
    }                                                                    \

/******************************************************************
* PolyComputeFirstHitStatus - computes the first hit status for a *
*   polygon and the Interval (if any) for the scanline-plane      *
*   given by the first two parameters.                            *
*                                                                 *
* Entry:                                                          *
*   Ns - normal of the scanline-plane                             *
*   ds - "d-value" for the scanline-plane                         *
*   LeftMost - the leftmost point on the scanline. Referred to    *
*       as L in the text.                                         *
*   ScanLineDir - the direction of the scanline in space.         *
*       Observe that it is constructed by subtracting two         *
*       adjacent points on the scanline from each other.          *
*   width - number of pixels per scanline                         *
*   Ui,Vi - the indices to the uv-plane. 0==x,1==y,2==z           *
*   Eyepos - the positition of the eye (or camera)                *
*   x,y,z - the points of the polygon                             *
*   NrVert - Number of vertices                                   *
*                                                                 *
* The function returns a struct FirstHitStatus with all           *
* necessary information.                                          *
******************************************************************/
struct FirstHitStatus PolyComputeFirstHitStatus(Vector Ns,float ds,
        Vector LeftMost,Vector ScanLineDir,int width,int Ui,int Vi,
        Vector Eyepos,float *x,float *y,float *z,short NrVert)
{
    struct FirstHitStatus FHstatus;
    Vector isect,dir;
    float dist,prevdist=0,denom,alpha,u,v;
    float realIntervalMax=-INFINITY,realIntervalMin=INFINITY;
    prevdist=Ns.X()*x[NrVert-1]+Ns.Y()*y[NrVert-1]+Ns.Z()*z[NrVert-1]+ds;
    /* start with last point */
    for(int i=0;i<NrVert;i++)
    {
        dist=Ns.X()*x[i]+Ns.Y()*y[i]+Ns.Z()*z[i]+ds;
        if(dist==0.0)
        {       /* point i is on the plane, project it on the scanline */
            PROJECTPOINT(x[i],y[i],z[i]);
        }
        else if((prevdist<0.0 && dist>0.0) || (prevdist>0.0 && dist<0.0))
        /* intersection */
        {
            isect.Set(x[i],y[i],z[i]);
            if(i==0)
            {
                dir.SetX(x[NrVert-1]-x[0]);
                dir.SetY(y[NrVert-1]-y[0]);
                dir.SetZ(z[NrVert-1]-z[0]);
            }
            else dir.Set(x[i-1]-x[i],y[i-1]-y[i],z[i-1]-z[i]);
            alpha=(-ds-Ns*isect)/(Ns*dir);
            isect+=dir*alpha;           /* intersection point calculated */
            PROJECTPOINT(isect.X(),isect.Y(),isect.Z());
        }
        prevdist=dist;
    }
   if(realIntervalMax==-INFINITY)          // no intersection
   {
       if(dist>0) FHstatus.Flag=NeverAgain;
       else FHstatus.Flag=OffScanline;
   }
   else if(realIntervalMax<0 || realIntervalMin>=width) 
       FHstatus.Flag=OffScanline;
   else
   {
       FHstatus.IntervalMax=(int)floor(realIntervalMax);
       FHstatus.IntervalMin=(int)ceil(realIntervalMin);        
       if(FHstatus.IntervalMax>=width) FHstatus.IntervalMax=width-1;
       if(FHstatus.IntervalMin<0) FHstatus.IntervalMin=0;
       if(FHstatus.IntervalMin==0 && FHstatus.IntervalMax==width-1)
           FHstatus.Flag=OnScanline;
       else FHstatus.Flag=Interval;
    }
    if(FHstatus.IntervalMin>FHstatus.IntervalMax) FHstatus.Flag=OffScanline;
    return FHstatus;
}
