#include <stdio.h>
#include <stdlib.h>

#define SQR(a) ((a)*(a))

typedef double MATX[10][10];
typedef double VECT[10];
typedef struct {double x; double y;} Point2;

Point2 pt[1023];                 /* From coordinates */
double zeta[1023], eta[1023];    /* To   coordinates */

int Gauss(MATX ain, VECT bin, int n, VECT v)
/*  Gaussian elimination by converting to upper triangular system.
    Row interchanges are done via re-indexing sub[]. See Vandergraft:
    Intro. Numerical Methods, 2ed, Academic Press, 1983, Chapter 6. */
{   MATX a;  VECT b;
    int  i, j, k, last, index;
    double big, absv;
    int  sub[21];

    for(k=0; k < n; k++) {       /* make local copies */
        for(j=0; j < n; j++) a[k][j] = ain[k][j];
        b[k] = bin[k];
        }

    last= n-1;
    for (k= 0; k <= last; k++) sub[k]= k;
    for (k= 0; k <= last-1; k++) {
        big= 0.0;
        for (i= k; i <= last; i++) {
            absv= abs(a[sub[i]][k]);
            if (absv > big)
               { big= absv; index= i; }
            }
        if (big == 0.0) return 0;
        j= sub[k];
        sub[k]= sub[index];
        sub[index]= j;
        big= 1.0/a[sub[k]][k];
        for (i= k+1; i <= last; i++) {
            a[sub[i]][k]= -a[sub[i]][k]*big;
            for (j= k+1; j <= last; j++)
                 a[sub[i]][j] += a[sub[i]][k] * a[sub[k]][j];
            b[sub[i]]    += a[sub[i]][k] * b[sub[k]];
            }
        }
        v[last]= b[sub[last]] / a[sub[last]][last];
        for (k= last-1; k >= 0; k--) {
            v[k]= b[sub[k]];
            for (i= k+1; i <= last; i++)
                 v[k] = v[k] -a[sub[k]][i] * v[i];
            v[k] = v[k] /a[sub[k]][k];
            }
        return 1;
}

void PrintMatrix(MATX a, VECT v, int size)
{   int r, c;
    for(r= 0; r < size; r++) {
        for(c= 0; c < size; c++) printf("%14.6lf  ",a[r][c]);
        printf(" %14.6lf\n", v[r]);
        }
     printf("\n");
}

void PrintSolution(VECT v, int vectorsize, char which)
/*  Print the solution vector */
{   int k;
    printf("Solution vector %c\n", which);
    for(k = 0; k < vectorsize; k++)
        if (abs(v[k]) < 1.0E6) printf("%14.6f  ",v[k]);
        else printf("%14.6e  ", v[k]);
    printf("\n");
}

void FirstOrderExact(VECT xv, VECT yv)
{   int k, ok;  VECT b;  MATX c;
    for(k= 0; k<=2; k++) b[k] = zeta[k];
    for(k= 0; k<=2; k++) {
        c[k][0] = pt[k].x;
        c[k][1] = pt[k].y;
        c[k][2] = 1.0;
        };

     printf("Augmented matrix:\n");
     PrintMatrix(c, b, 3);
     ok =Gauss(c, b, 3, xv);
     PrintSolution(xv, 3, 'X');

     for(k= 0; k<=2; k++) b[k] = eta[k];
     for(k= 0; k<=2; k++) {
         c[k][0] = pt[k].x;
         c[k][1] = pt[k].y;
         c[k][2] = 1.0;
         };

     PrintMatrix(c, b, 3);
     ok =Gauss(c, b, 3, yv);
     PrintSolution(yv, 3, 'Y');
}

void SecondOrderExact(VECT xv, VECT yv)
{   int k, ok;  VECT b;  MATX c;
    for(k= 0; k<=5; k++) b[k] = zeta[k];
    for(k= 0; k<=5; k++) {
        c[k][0] = pt[k].x*pt[k].x;
        c[k][1] = pt[k].y*pt[k].y;
        c[k][2] = pt[k].x;
        c[k][3] = pt[k].y;
        c[k][4] = pt[k].x*pt[k].y;
        c[k][5] = 1;
        }

     printf("Augmented matrix:\n");
     PrintMatrix(c, b, 6);
     ok =Gauss(c, b, 6, xv);
     printf("x = a5*x^2 + a4*y^2 + a3*x + a2*y + a1*x*y + a0:\n");
     PrintSolution(xv, 6, 'X');

     for(k= 0; k<=5; k++) b[k] = eta[k];
     for(k= 0; k<=5; k++) {
         c[k][0] = SQR(pt[k].x);
         c[k][1] = SQR(pt[k].y);
         c[k][2] = pt[k].x;
         c[k][3] = pt[k].y;
         c[k][4] = pt[k].x*pt[k].y;
         c[k][5] = 1;
         }

     printf("Augmented matrix:\n");
     PrintMatrix(c, b, 6);
     ok =Gauss(c, b, 6, yv);
     printf("y = b5*x^2 + b4*y^2 + b3*x + b2*y + b1*x*y + b0:\n");
     PrintSolution(yv, 6, 'Y');
}

void FirstOrderLeastSquares(int npoints, VECT xv, VECT yv)
{   MATX c;  VECT b;
    double sumx= 0, sumxx= 0, sumy= 0, sumyy= 0, sumxy= 0,
           sumd= 0, sumdx= 0, sumdy = 0;
    int k, ok;
    double xt, yt;
    for(k=0; k < npoints; k++) {
        sumx  += pt[k].x;
        sumxx += SQR(pt[k].x);
        sumy  += pt[k].y;
        sumyy += SQR(pt[k].y);
        sumxy += pt[k].x*pt[k].y;
        sumd  += zeta[k];
        sumdx += pt[k].x*zeta[k];
        sumdy += pt[k].y*zeta[k];
        }

    c[0][0] = sumxx;   c[0][1] = sumxy;   c[0][2] = sumx;
    c[1][0] = sumxy;   c[1][1] = sumyy;   c[1][2] = sumy;
    c[2][0] = sumx;    c[2][1] = sumy;    c[2][2] = npoints;

    b[0] = sumdx;      b[1] = sumdy;      b[2] = sumd;
    ok = Gauss(c, b, 3, xv);

    sumd = sumdx = sumdy = 0;

    for(k=0; k < npoints; k++) {
        sumd += eta[k];
        sumdx+= pt[k].x*eta[k];
        sumxy+= pt[k].y*eta[k];
        };

     b[0] = sumdx;  b[1] = sumdy; b[2] = sumd;
     ok = Gauss(c, b, 3, yv);
     printf("residuals\n");
     for(k=0; k < npoints; k++) {
        xt = zeta[k] -(pt[k].x*xv[0] + pt[k].y*xv[1] + xv[2]);
        yt =  eta[k] -(pt[k].x*yv[0] + pt[k].y*yv[1] + yv[2]);
        printf("%4d   %12.6   %12.6\n", xt, yt);
        }
}

void SecondOrderLeastSquares(MATX c, int npoints, VECT xv, VECT yv)
{   int j, k, ok;
    VECT b;
    double sumd=0, sumdx=0, sumdx2=0, sumdy=0,
                   sumdy2=0, sumdxy =0;
    double px2, py2, xt, yt;

    for(j=0; j<= 5; j++)
        for(k=0; k<= 5; k++) c[j][k] = 0;

    for(k =0; k < npoints; k++) {
        px2 = SQR(pt[k].x);
        py2 = SQR(pt[k].y);
        c[0][0] +=  px2 *px2;    /* coefficients for normal equations */
        c[0][1] +=  px2 *py2;
        c[0][2] +=  px2 *pt[k].x;
        c[0][3] +=  px2 *pt[k].y;
        c[0][4] +=  px2 *pt[k].x *pt[k].y;
        c[0][5] +=  px2;
        c[1][1] +=  py2 *py2;
        c[1][2] +=  pt[k].x *py2;
        c[1][3] +=  pt[k].y *py2;
        c[1][4] +=  pt[k].x *py2 *pt[k].y;
        c[1][5] +=  py2;
        c[2][2] +=  px2;
        c[2][3] +=  pt[k].x *pt[k].y;
        c[2][4] +=  px2 *pt[k].y;
        c[2][5] +=  pt[k].x;
        c[3][3] +=  py2;
        c[3][4] +=  pt[k].x *py2;
        c[3][5] +=  pt[k].y;
        c[4][4] +=  px2 *py2;
        c[4][5] +=  pt[k].x *pt[k].y;

        sumd    += zeta[k];
        sumdx   += pt[k].x *zeta[k];
        sumdx2  += px2 *zeta[k];
        sumdy   += pt[k].y *zeta[k];
        sumdy2  += py2 *zeta[k];
        sumdxy  += pt[k].x *pt[k].y *zeta[k];
        }

    c[1][0] =c[0][1];  /* Coefficient matrix is symmetric about diagonal */
    c[2][0] =c[0][2];  c[2][1] =c[1][2];
    c[3][0] =c[0][3];  c[3][1] =c[1][3];  c[3][2] =c[2][3];
    c[4][0] =c[0][4];  c[4][1] =c[1][4];  c[4][2] =c[2][4];
                                          c[4][3] =c[3][4];
    c[5][0] =c[0][5];  c[5][1] =c[1][5];  c[5][2] =c[2][5];
                                          c[5][3] =c[3][5];
    c[5][4] =c[4][5];  c[5][5] =npoints;

    b[0] =sumdx2;    b[1] =sumdy2;    b[2] =sumdx;  /* new vector */
    b[3] =sumdy;     b[4] =sumdxy;    b[5] =sumd;

    printf("Augmented matrix:\n");
    PrintMatrix(c, b, 6);
    ok =Gauss(c, b, 6, xv);
    printf("x = a5*x^2 + a4*y^2 + a3*x + a2*y + a1*x*y + a0:\n");
    PrintSolution(xv, 6, 'X');

    sumd = sumdx = sumdx2 = sumdy = sumdy2 = sumdxy =0;

    for(k =0; k < npoints; k++) {
        px2 = SQR(pt[k].x);
        py2 = SQR(pt[k].y);
        sumd   += eta[k];
        sumdx  += pt[k].x *eta[k];
        sumdx2 += px2 *eta[k];
        sumdy  += pt[k].y *eta[k];
        sumdy2 += py2 *eta[k];
        sumdxy += pt[k].x *pt[k].y *eta[k];
        }

 /* Coefficient matrix must remain unchanged. */

    b[0] =sumdx2;    b[1] =sumdy2;    b[2] =sumdx;  /* New vector */
    b[3] =sumdy;     b[4] =sumdxy;    b[5] =sumd;

    ok =Gauss(c, b, 6, yv);
    printf("y = b5*x^2 + b4*y^2 + b3*x + b2*y + b1*x*y + b0:\n");
    PrintSolution(yv, 6, 'Y');

    printf("Residuals:\n");
    for(k =0; k < npoints; k++) {
        xt = SQR(pt[k].x)*xv[0] + SQR(pt[k].y)*xv[1] +
                 pt[k].x *xv[2] + pt[k].y*xv[3] +
                 pt[k].x *pt[k].y*xv[4] +xv[5];
        xt = zeta[k] -xt;
        yt = SQR(pt[k].x)*yv[0] + SQR(pt[k].y)*yv[1] +
                 pt[k].x*yv[2] + pt[k].y*yv[3] +
                 pt[k].x *pt[k].y*yv[4] +yv[5];
        yt = eta[k] -yt;
        printf("%4d  %12.6f  %12.6f\n", (k+1), xt, yt);
        }
}
