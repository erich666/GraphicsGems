/*
   doquartic.c

   a test of a quartic solving routine

   Don Herbison-Evans   24 June 1994
*/

#include <stdio.h>

double nought;
double doub1,doub2;
double doub3,doub4;
double doub6,doub12;
double doub24;
double doubmax;       /* approx square root of max double number */
double doubmin;       /* smallest double number */
double doubtol;       /* tolerance of double numbers */
double rt3;
double inv2,inv3,inv4;

void setcns();
int descartes();
double exp(),log(),sqrt(),cos(),acos();
double cubic();
int qudrtc();
int quartic();
int ferrari();
int neumark();
void errors();

main()
{
   double a,b,c,d;
   double rts[4];
   double rterr[4];
   int nr;

   setcns();
   a = -(double)10;
   b = (double)35;
   c = -(double)50;
   d = (double)24;
   nr = descartes(a,b,c,d,rts);
   errors(a,b,c,d,rts,rterr,nr);
   nr = ferrari(a,b,c,d,rts);
   errors(a,b,c,d,rts,rterr,nr);
   nr = neumark(a,b,c,d,rts);
   errors(a,b,c,d,rts,rterr,nr);
   exit(0);
}
/****************************************************/

void setcns()
/* 
     set up constants 
*/
{
      int j;

      nought = (double)0;
      doub1 = (double)1;
      doub2 = (double)2;
      doub3 = (double)3;
      doub4 = (double)4;
      doub6 = (double)6;
      doub12 = (double)12;
      doub24 = (double)24;
      inv2 = doub1/(double)2;
      inv3 = doub1/(double)3;
      inv4 = doub1/(double)4;
      rt3 = sqrt(doub3) ;

      doubtol = doub1;
      for (  j = 1 ; doub1+doubtol > doub1 ; ++ j )
      {
          doubtol *= inv2;
      }
      doubtol = sqrt(doubtol);

      doubmin = inv2 ;
      for (  j = 1 ; j <= 100 ; ++ j )
      {
          doubmin=doubmin*doubmin ;
          if ((doubmin*doubmin) <= (doubmin*doubmin*inv2))
              break;
      }
      doubmax=0.7/sqrt(doubmin) ;

} /* setcns */
/***********************************/

int quartic(a,b,c,d,rts,rterr)
double a,b,c,d,rts[4],rterr[4];
/*
   Solve quartic equation using either
   quadratic, Ferrari's or Neumark's algorithm.

   called by 
   calls  qudrtc, ferrari, neumark.

     21 Jan 1989  Don Herbison-Evans
*/
{
   int qudrtc(),ferrari(),neumark();
   int j,k,nq,nr;
   double odd, even;
   double roots[4];

   if (a < nought) odd = -a; else odd = a;
   if (c < nought) odd -= c; else odd += c;
   if (b < nought) even = -b; else even = b;
   if (d < nought) even -= d; else even += d;
   if (odd < even*doubtol)
   {
      nq = qudrtc(b,d,roots,b*b-doub4*d);
      j = 0;
      for (k = 0; k < nq; ++k)
      {
         if (roots[k] > nought)
         {
            rts[j] = sqrt(roots[k]);
            rts[j+1] = -rts[j];
            ++j; ++j;
         }
      }
      nr = j;
   }
   else
   {
      if (a < nought) k = 1; else k = 0;
      if (b < nought) k += k+1; else k +=k; 
      if (c < nought) k += k+1; else k +=k; 
      if (d < nought) k += k+1; else k +=k; 
      switch (k)
      {
              case 0 : nr = ferrari(a,b,c,d,rts) ; break;
              case 1 : nr = neumark(a,b,c,d,rts) ; break;
              case 2 : nr = neumark(a,b,c,d,rts) ; break;
              case 3 : nr = ferrari(a,b,c,d,rts) ; break;
              case 4 : nr = ferrari(a,b,c,d,rts) ; break;
              case 5 : nr = neumark(a,b,c,d,rts) ; break;
              case 6 : nr = ferrari(a,b,c,d,rts) ; break;
              case 7 : nr = ferrari(a,b,c,d,rts) ; break;
              case 8 : nr = neumark(a,b,c,d,rts) ; break;
              case 9 : nr = ferrari(a,b,c,d,rts) ; break;
              case 10 : nr = ferrari(a,b,c,d,rts) ; break;
              case 11 : nr = neumark(a,b,c,d,rts) ; break;
              case 12 : nr = ferrari(a,b,c,d,rts) ; break;
              case 13 : nr = ferrari(a,b,c,d,rts) ; break;
              case 14 : nr = ferrari(a,b,c,d,rts) ; break;
              case 15 : nr = ferrari(a,b,c,d,rts) ; break;
      }
   }
   errors(a,b,c,d,rts,rterr,nr);
   return(nr);
} /* quartic */
/*****************************************/

int ferrari(a,b,c,d,rts)
   double a,b,c,d,rts[4];
/* 
     solve the quartic equation - 

   x**4 + a*x**3 + b*x**2 + c*x + d = 0 

   called by quartic
   calls     cubic, qudrtc.

     input - 
   a,b,c,e - coeffs of equation. 

     output - 
   nquar - number of real roots. 
   rts - array of root values. 

     method :  Ferrari - Lagrange
     Theory of Equations, H.W. Turnbull p. 140 (1947)

     calls  cubic, qudrtc 
*/
{
   double cubic();
   int qudrtc();

   int nquar,n1,n2 ;
   double asq,ainv2;
   double v1[4],v2[4] ;
   double p,q,r ;
   double y;
   double e,f,esq,fsq,ef ;
   double g,gg,h,hh;

   fprintf(stderr,"\nFerrari %g %g %g %g\n",a,b,c,d);
   asq = a*a;

   p = b ;
   q = a*c-doub4*d ;
   r = (asq - doub4*b)*d + c*c ;
   y = cubic(p,q,r) ;

   esq = inv4*asq - b - y;
   if (esq < nought) return(0);
   else
   {
      fsq = inv4*y*y - d;
      if (fsq < nought) return(0);
      else
      {
         ef = -(inv4*a*y + inv2*c);
         if ( ((a > nought)&&(y > nought)&&(c > nought))
           || ((a > nought)&&(y < nought)&&(c < nought))
           || ((a < nought)&&(y > nought)&&(c < nought))
           || ((a < nought)&&(y < nought)&&(c > nought))
           ||  (a == nought)||(y == nought)||(c == nought)
         )
/* use ef - */
         {
            if ((b < nought)&&(y < nought)&&(esq > nought))
            {
               e = sqrt(esq);
               f = ef/e;
            }
            else if ((d < nought) && (fsq > nought))
            {
               f = sqrt(fsq);
               e = ef/f;
            }
            else
            {
               e = sqrt(esq);
               f = sqrt(fsq);
               if (ef < nought) f = -f;
            }
         }
         else
         {
            e = sqrt(esq);
            f = sqrt(fsq);
            if (ef < nought) f = -f;
         }
/* note that e >= nought */
         ainv2 = a*inv2;
         g = ainv2 - e;
         gg = ainv2 + e;
         if ( ((b > nought)&&(y > nought))
           || ((b < nought)&&(y < nought)) )
         {
            if (( a > nought) && (e != nought)) g = (b + y)/gg;
               else if (e != nought) gg = (b + y)/g;
         }
         if ((y == nought)&&(f == nought))
         {
            h = nought;
            hh = nought;
         }
         else if ( ((f > nought)&&(y < nought))
                || ((f < nought)&&(y > nought)) )
         {
            hh = -inv2*y + f;
            h = d/hh;
         }
         else
         {
            h = -inv2*y - f;
            hh = d/h;
         }
         n1 = qudrtc(gg,hh,v1, gg*gg - doub4*hh) ;
         n2 = qudrtc(g,h,v2, g*g - doub4*h) ;
         nquar = n1+n2 ;
         rts[0] = v1[0] ;
         rts[1] = v1[1] ;
         rts[n1+0] = v2[0] ;
         rts[n1+1] = v2[1] ;
         return(nquar);
      }
   }
} /* ferrari */
/*****************************************/

int neumark(a,b,c,d,rts)
   double a,b,c,d,rts[4];
/* 
     solve the quartic equation - 

   x**4 + a*x**3 + b*x**2 + c*x + d = 0 

   called by quartic
   calls     cubic, qudrtc.

     input parameters - 
   a,b,c,e - coeffs of equation. 

     output parameters - 
   nquar - number of real roots. 
   rts - array of root values. 

     method -  S. Neumark 

     Solution of Cubic and Quartic Equations - Pergamon 1965 
        translated to C with help of Shawn Neely

*/
{
   int nquar,n1,n2 ;
   double y,g,gg,h,hh,gdis,gdisrt,hdis,hdisrt,g1,g2,h1,h2 ;
   double bmy,gerr,herr,y4,d4,bmysq ;
   double v1[4],v2[4] ;
   double asq ;
   double p,q,r ;
   double hmax,gmax ;
   double cubic();
   int qudrtc();

   fprintf(stderr,"\nNeumark %g %g %g %g\n",a,b,c,d);
   asq = a*a ;

   p =  -b*doub2 ;
   q = b*b + a*c - doub4*d ;
   r = (c - a*b)*c + asq*d ;
   y = cubic(p,q,r) ;

   bmy = b - y ;
   y4 = y*doub4 ;
   d4 = d*doub4 ;
   bmysq = bmy*bmy ;
   gdis = asq - y4 ;
   hdis = bmysq - d4 ;
   if ((gdis < nought) || (hdis < nought)) return(0);
   else
   {
      g1 = a*inv2 ;
      h1 = bmy*inv2 ;
      gerr = asq + y4 ;
      herr = hdis ;
      if (d > nought) herr = bmysq + d4 ;
      if ((y < nought) || (herr*gdis > gerr*hdis))
      {
         gdisrt = sqrt(gdis) ;
         g2 = gdisrt*inv2 ;
         if (gdisrt != nought) h2 = (a*h1 - c)/gdisrt ;
            else h2 = nought;
      }
      else
      {
         hdisrt = sqrt(hdis) ;
         h2 = hdisrt*inv2 ;
         if (hdisrt != nought) g2 = (a*h1 - c)/hdisrt ;
            else g2 = nought;
      }
/* 
     note that in the following, the tests ensure non-zero 
     denominators -  
*/
      h = h1 - h2 ;
      hh = h1 + h2 ;
      hmax = hh ;
      if (hmax < nought) hmax =  -hmax ;
      if (hmax < h) hmax = h ;
      if (hmax <  -h) hmax =  -h ;
      if ((h1 > nought)&&(h2 > nought)) h = d/hh ;
      if ((h1 < nought)&&(h2 < nought)) h = d/hh ;
      if ((h1 > nought)&&(h2 < nought)) hh = d/h ;
      if ((h1 < nought)&&(h2 > nought)) hh = d/h ;
      if (h > hmax) h = hmax ;
      if (h <  -hmax) h =  -hmax ;
      if (hh > hmax) hh = hmax ;
      if (hh <  -hmax) hh =  -hmax ;

      g = g1 - g2 ;
      gg = g1 + g2 ;
      gmax = gg ;
      if (gmax < nought) gmax =  -gmax ;
      if (gmax < g) gmax = g ;
      if (gmax <  -g) gmax =  -g ;
      if ((g1 > nought)&&(g2 > nought)) g = y/gg ;
      if ((g1 < nought)&&(g2 < nought)) g = y/gg ;
      if ((g1 > nought)&&(g2 < nought)) gg = y/g ;
      if ((g1 < nought)&&(g2 > nought)) gg = y/g ;
      if (g > gmax) g = gmax ;
      if (g <  -gmax) g =  -gmax ;
      if (gg > gmax) gg = gmax ;
      if (gg <  -gmax) gg =  -gmax ;
 
      n1 = qudrtc(gg,hh,v1, gg*gg - doub4*hh) ;
      n2 = qudrtc(g,h,v2, g*g - doub4*h) ;
      nquar = n1+n2 ;
      rts[0] = v1[0] ;
      rts[1] = v1[1] ;
      rts[n1+0] = v2[0] ;
      rts[n1+1] = v2[1] ;
 
      return(nquar);
   }
} /* neumark */
/****************************************************/

void errors(a,b,c,d,rts,rterr,nrts)
double a,b,c,d,rts[4],rterr[4];
int nrts;
/*
   find the errors

   called by quartic.
*/
{
   int k;
   double deriv,test;
   double fabs(),sqrt(),curoot();

   if (nrts > 0)
   {
      for (  k = 0 ; k < nrts ; ++ k )
      {
         test = (((rts[k]+a)*rts[k]+b)*rts[k]+c)*rts[k]+d ;
         if (test == nought) rterr[k] = nought;
         else
         {
            deriv =
               ((doub4*rts[k]+doub3*a)*rts[k]+doub2*b)*rts[k]+c ;
            if (deriv != nought)
               rterr[k] = fabs(test/deriv);
            else
            {
               deriv = (doub12*rts[k]+doub6*a)*rts[k]+doub2*b ;
               if (deriv != nought)
                   rterr[k] = sqrt(fabs(test/deriv)) ;
               else
               {
                  deriv = doub24*rts[k]+doub6*a ;
                  if (deriv != nought)
                     rterr[k] = curoot(fabs(test/deriv));
                  else
                     rterr[k] = sqrt(sqrt(fabs(test)/doub24));
               }
            }
         }
         fprintf(stderr,"errorsa  %d %9g %9g\n",
               k,rts[k],rterr[k]);
      }
   }
   else fprintf(stderr,"errors ans: none\n");
} /* errors */
/**********************************************/

int qudrtc(b,c,rts,dis)
   double b,c,rts[4],dis ;
/* 
     solve the quadratic equation - 

         x**2+b*x+c = 0 

     called by  quartic, ferrari, neumark, ellcut 
*/
{
   int nquad;
   double rtdis ;

   if (dis >= nought)
   {
      nquad = 2 ;
      rtdis = sqrt(dis) ;
      if (b > nought) rts[0] = ( -b - rtdis)*inv2 ;
         else rts[0] = ( -b + rtdis)*inv2 ;
      if (rts[0] == nought) rts[1] =  -b ;
      else rts[1] = c/rts[0] ;
   }
   else
   {
      nquad = 0;
      rts[0] = nought ;
      rts[1] = nought ;
   }
   return(nquad);
} /* qudrtc */
/**************************************************/

double cubic(p,q,r)
double p,q,r;
/* 
     find the lowest real root of the cubic - 
       x**3 + p*x**2 + q*x + r = 0 

   input parameters - 
     p,q,r - coeffs of cubic equation. 

   output- 
     cubic - a real root. 

   global constants -
     rt3 - sqrt(3) 
     inv3 - 1/3 
     doubmax - square root of largest number held by machine 

     method - 
     see D.E. Littlewood, "A University Algebra" pp.173 - 6 

     Charles Prineas   April 1981 

     called by  neumark.
     calls  acos3 
*/
{
   int nrts;
   double po3,po3sq,qo3;
   double uo3,u2o3,uo3sq4,uo3cu4 ;
   double v,vsq,wsq ;
   double m,mcube,n;
   double muo3,s,scube,t,cosk,sinsqk ;
   double root;
   double curoot();
   double acos3();
   double sqrt(),fabs();

   m = nought;
   nrts =0;
   if ((p > doubmax) || (p <  -doubmax)) root = -p;
   else
   if ((q > doubmax) || (q <  -doubmax))
   {
      if (q > nought) root = -r/q;
      else root = -sqrt(-q);
   }
   else
   if ((r > doubmax)|| (r <  -doubmax)) root =  -curoot(r) ;
   else
   {
      po3 = p*inv3 ;
      po3sq = po3*po3 ;
      if (po3sq > doubmax) root =  -p ;
      else
      {
         v = r + po3*(po3sq + po3sq - q) ;
         if ((v > doubmax) || (v < -doubmax)) root = -p ;
         else
         {
            vsq = v*v ;
            qo3 = q*inv3 ;
            uo3 = qo3 - po3sq ;
            u2o3 = uo3 + uo3 ;
            if ((u2o3 > doubmax) || (u2o3 < -doubmax))
            {
               if (p == nought)
               {
                  if (q > nought) root =  -r/q ;
                  else root =  -sqrt(-q) ;
               }
               else root =  -q/p ;
            }
            uo3sq4 = u2o3*u2o3 ;
            if (uo3sq4 > doubmax)
            {
               if (p == nought)
               {
                  if (q > nought) root = -r/q ;
                  else root = -sqrt(fabs(q)) ;
               }
               else root = -q/p ;
            }
            uo3cu4 = uo3sq4*uo3 ;
            wsq = uo3cu4 + vsq ;
            if (wsq >= nought)
            {
/* 
     cubic has one real root 
*/
               nrts = 1;
               if (v <= nought) mcube = ( -v + sqrt(wsq))*inv2 ;
               if (v  > nought) mcube = ( -v - sqrt(wsq))*inv2 ;
               m = curoot(mcube) ;
               if (m != nought) n = -uo3/m ;
                  else n = nought;
               root = m + n - po3 ;
            }
            else
            {
               nrts = 3;
/* 
     cubic has three real roots 
*/
               if (uo3 < nought)
               {
                  muo3 = -uo3;
                  s = sqrt(muo3) ;
                  scube = s*muo3;
                  t =  -v/(scube+scube) ;
                  cosk = acos3(t) ;
                  if (po3 < nought)
                     root = (s+s)*cosk - po3;
                  else
                  {
                     sinsqk = doub1 - cosk*cosk ;
                     if (sinsqk < nought) sinsqk = nought ;
                     root = s*( -cosk - rt3*sqrt(sinsqk)) - po3 ;
                  }
               }
               else
/* 
     cubic has multiple root -  
*/
               root = curoot(v) - po3 ;
            }
         }
      }
   }
   fprintf(stderr,"cubic %g %g %g %d %g\n",p,q,r,nrts,root);
   return(root);
} /* cubic */
/***************************************/

double acos3(x)
   double x ;
/* 
     find cos(acos(x)/3) 
    
     Don Herbison-Evans   16/7/81 

     called by cubic . 
*/
{
   double value;
   double acos(),cos();

   value = cos(acos(x)*inv3);
   return(value);
} /* acos3 */
/***************************************/

double curoot(x)
   double x ;
/* 
     find cube root of x.
 
     Don Herbison-Evans   30/1/89 

     called by cubic . 
*/
{
   double exp(),log();
   double value;
   double absx;
   int neg;

   neg = 0;
   absx = x;
   if (x < nought)
   {
      absx = -x;
      neg = 1;
   }
   value = exp( log(absx)*inv3 );
   if (neg == 1) value = -value;
   return(value);
} /* curoot */
/****************************************************/

int simple(a,b,c,d,rts)
   double a,b,c,d,rts[4];
/* 
     solve the quartic equation - 

   x**4 + a*x**3 + b*x**2 + c*x + d = 0 

   called by quartic
   calls     cubic, qudrtc.

     input - 
   a,b,c,e - coeffs of equation. 

     output - 
   nquar - number of real roots. 
   rts - array of root values. 

     method :  unstabilized Ferrari-Lagrange
     Abramowitz,M. & Stegun I.A.
     Handbook of Mathematical Functions
     Dover 1972 (ninth printing), pp. 17-18

     calls  cubic, qudrtc 
*/
{
   double cubic();
   int qudrtc();

   int nquar,n1,n2 ;
   double asq,y;
   double v1[4],v2[4] ;
   double p,q,r ;
   double e,f,esq,fsq ;
   double g,gg,h,hh;

   fprintf(stderr,"\nsimple %g %g %g %g\n",a,b,c,d);
   asq = a*a;

   p = -b ;
   q = a*c-doub4*d ;
   r = -asq*d - c*c + doub4*b*d ;
   y = cubic(p,q,r) ;

   esq = inv4*asq - b + y;
   fsq = inv4*y*y - d;
   if (esq < nought) return(0);
   else
   if (fsq < nought) return(0);
   else
   {
      e = sqrt(esq);
      f = sqrt(fsq);
      g = inv2*a - e;
      h = inv2*y - f;
      gg = inv2*a + e;
      hh = inv2*y + f;
      n1 = qudrtc(gg,hh,v1, gg*gg - doub4*hh) ;
      n2 = qudrtc(g,h,v2, g*g - doub4*h) ;
      nquar = n1+n2 ;
      rts[0] = v1[0] ;
      rts[1] = v1[1] ;
      rts[n1+0] = v2[0] ;
      rts[n1+1] = v2[1] ;
      return(nquar);
   }
} /* simple */
/*****************************************/
 
int descartes(a,b,c,d,rts)
double a,b,c,d,rts[4];
/*
   Solve quartic equation using
   Descartes-Euler-Cardano algorithm

   Strong, T. "Elemementary and Higher Algebra"
      Pratt and Oakley, p. 469 (1859)

     29 Jun 1994  Don Herbison-Evans
*/
{
   int qudrtc();
   double cubic();

   int nrts;
   int r1,r2;
   double v1[4],v2[4];
   double y;
   double p,q,r;
   double A,B,C;
   double m,n1,n2;
   double d3o8,d3o256;
   double inv8,inv16;
   double asq;
   double Binvm;

   d3o8 = (double)3/(double)8;
   inv8 = doub1/(double)8;
   inv16 = doub1/(double)16;
   d3o256 = (double)3/(double)256;

   fprintf(stderr,"\nDescartes %f %f %f %f\n",a,b,c,d);
   asq = a*a;

   A = b - asq*d3o8;
   B = c + a*(asq*inv8 - b*inv2);
   C = d + asq*(b*inv16 - asq*d3o256) - a*c*inv4;

   p = doub2*A;
   q = A*A - doub4*C;
   r = -B*B;

/***
   inv64 = doub1/(double)64;
   p = doub2*b - doub3*a*a*inv4 ;
   q = b*b - a*a*b - doub4*d + doub3*a*a*a*a*inv16 + a*c;
   r = -c*c - a*a*a*a*a*a*inv64 - a*a*b*b*inv4
       -a*a*a*c*inv4 + a*b*c + a*a*a*a*b*inv8;
***/

   y = cubic(p,q,r) ;
   if (y <= nought) 
      nrts = 0;
   else
   {
      m = sqrt(y);
      Binvm = B/m;
      n1 = (y + A + Binvm)*inv2;
      n2 = (y + A - Binvm)*inv2;
      r1 = qudrtc(-m, n1, v1, y-doub4*n1);
      r2 = qudrtc( m, n2, v2, y-doub4*n2);
      rts[0] = v1[0]-a*inv4;
      rts[1] = v1[1]-a*inv4;
      rts[r1] = v2[0]-a*inv4;
      rts[r1+1] = v2[1]-a*inv4;
      nrts = r1+r2;
   } 
   return(nrts);
} /* descartes */
/****************************************************/
