#include <math.h>
#include <stdlib.h>

//      NUMERIC CONSTANTS AND RELATED THINGS

const double Pi=3.1415926535;
inline double RAD(double r) {return Pi*r/180.;}     // DEG-TO-RAD CONVERSION
const double EPS=1e-4;                              // VERY SMALL NUMBER

//      LIST TEMPLATE

template <class T> struct listelem {
        T o;                                            // OBJECT
        listelem<T> *n;                                 // NEXT ELEMENT
        listelem(T o) {this->o=o; n=(listelem<T>*)0;}
};

template <class T> class list {
        listelem<T> *h, *t, *a;                         // HEAD, TAIL, ACTUAL
public:
        list() {h=t=(listelem<T>*)0;}
        ~list() {for(;h;h=a) {a=h->n; delete h;}}
        void operator+=(T o) {                          // INSERT INTO LIST
                listelem<T> *e=new listelem<T>(o);
                if(t) {t->n=e; t=e;} else {h=e; t=e;}
        }
        T first() {a=h; return a?a->o:(T)0;}
        T next() {a=a->n; return a?a->o:(T)0;}
        int operator[](T o) {                                   // o ON LIST?
                for(listelem<T>*e=h;e;e=e->n) if(e->o==o) return 1;
                return 0;
        }
};

//      DECLARATION TEMPLATE (SIMILAR TO LIST BUT MAY BE ENHANCED)

template <class T> struct declaration {
        char *i;                                                // IDENTIFIER
        T *p;                                                   // OBJECT
        declaration<T> *n;                                      // NEXT
        declaration(char* i, T* p) {this->i=i; this->p=p;}
        ~declaration() {delete i; delete p;}
};

template <class T> class declarations {
        declaration<T> *h;                                      // HEAD
public:
        declarations() {h=(declaration<T>*)0;}
        ~declarations() {
                for(declaration<T>* a; h; h=a) {a=h->n; delete h;}
        }
        void operator+=(declaration<T>* d) {d->n=h; h=d;}       // INSERT
        T* operator[](char* i) {                                // GET
                for(declaration<T>* d=h; d; d=d->n)
                        if(strcmp(d->i,i)==0)
                                return d->p;
                cout<<"identifier "<<i<<" not found\n";
                exit(1);
                return (T*)0;
        }
};

//      VECTORS AND MATRICES

class xform;

class vector {
        friend ostream& operator<<(ostream& o, vector& v);
        friend vector operator-(vector& v);
        friend class xform;
        double x, y, z;
public:
        vector() {x=0.; y=0.; z=0.;}
        vector(double x, double y, double z) {
                this->x=x; this->y=y; this->z=z;
        }
        vector operator+(vector& v) {return vector(x+v.x,y+v.y,z+v.z);}
        vector operator-(vector& v) {return vector(x-v.x,y-v.y,z-v.z);}
        vector operator*(vector& v) {return vector(x*v.x,y*v.y,z*v.z);}
        vector operator/(vector& v) {return vector(x/v.x,y/v.y,z/v.z);}
        vector operator+(double d) {return vector(x+d,y+d,z+d);}
        vector operator-(double d) {return vector(x-d,y-d,z-d);}
        vector operator*(double d) {return vector(x*d,y*d,z*d);}
        vector operator/(double d) {return vector(x/d,y/d,z/d);}
        double operator%(vector& v) {return x*v.x+y*v.y+z*v.z;}
        operator double() {return y;}
        double operator[](int i) {return i==0?this->x:i==1?this->y:this->z;}
        double get_x() { return this->x;}
        double get_y() { return this->y;}
        double get_z() { return this->z;}
};

extern vector norm(vector& v);

struct matrix {
        double m11, m12, m13;                                   // 1ST ROW
        double m21, m22, m23;                                   // 2ND ROW
        double m31, m32, m33;                                   // 3RD ROW
        matrix(double m11=1., double m12=0., double m13=0.,
               double m21=0., double m22=1., double m23=0.,
               double m31=0., double m32=0., double m33=1.) {
                this->m11=m11; this->m12=m12; this->m13=m13;
                this->m21=m21; this->m22=m22; this->m23=m23;
                this->m31=m31; this->m32=m32; this->m33=m33;
        }
        matrix operator*(matrix& m) {
                return matrix(
                        m11*m.m11+m12*m.m21+m13*m.m31,
                        m11*m.m12+m12*m.m22+m13*m.m32,
                        m11*m.m13+m12*m.m23+m13*m.m33,

                        m21*m.m11+m22*m.m21+m23*m.m31,
                        m21*m.m12+m22*m.m22+m23*m.m32,
                        m21*m.m13+m22*m.m23+m23*m.m33,

                        m31*m.m11+m32*m.m21+m33*m.m31,
                        m31*m.m12+m32*m.m22+m33*m.m32,
                        m31*m.m13+m32*m.m23+m33*m.m33
                );
        }
        vector operator*(vector& v) {
                return vector(
                        vector(m11,m12,m13)%v,
                        vector(m21,m22,m23)%v,
                        vector(m31,m32,m33)%v
                );
        }
        vector operator/(vector& v) {                     // INVERSE
                return vector(
                        v%vector(m11,m21,m31),
                        v%vector(m12,m22,m32),
                        v%vector(m13,m23,m33)
                );
        }
};

//      RAYS AND INTERSECTIONS

struct ray {
        vector o;                                               // ORIGIN
        vector d;                                               // DIRECTION
        int l;                                                  // LEVEL
        int c;                                                  // CODE
        ray(vector& o, vector& d, int l, int c)
                {this->o=o; this->d=d; this->l=l; this->c=c;}
};

class object;

struct intersect {
        double t;                                       // RAY PARAMETER
        vector p;                                       // SURFACE POINT
        vector n;                                       // SURFACE NORMAL
        object *o;                                      // OBJECT SURFACE
        intersect() {o=(object*)0;}
        intersect(double t, vector& p, vector& n, object *o) {
                this->t=t; this->p=p; this->n=n; this->o=o;
        }
};

//      HALF-SPACES

struct halfspace {
        vector n;                                       // NORMAL
        double d;                                       // DISTANCE FROM ORIGIN
        halfspace()                                     // DEFAULT CONSTRUCTOR
                {this->n=vector(0.,0.,1.); this->d=0.;}
        halfspace(vector& n, double d)                  // USUAL DEFINITION
                {this->n=norm(n); this->d=d;}
        halfspace(vector& u, vector& v)     // BISECTOR PLANE
                {n=norm(v-u); d=((u+v)/2.)%n;}
        int operator&(vector& p) {return n%p<=d;} // IF CONTAINS POINT p
};

//      GEOMETRIC TRANSFORMATIONS (RIGID MOTIONS)

class xform {
        friend ostream& operator<<(ostream& o, xform& T);
        matrix A;                               // ORTHONORMAL ORIENTATION
        vector s;                               // DIAGONAL SCALE
        vector t;                               // TRANSLATION VECTOR
public:
        xform() {s=vector(1.,1.,1.); t=vector(0.,0.,0.);}
        void operator+=(vector& t) {this->t=this->t+t;}   // TRANSLATE
        void operator*=(vector& s) {                      // SCALE
                this->s=this->s*s; this->t=s*this->t;
        }
        void operator<<=(vector& r) {                     // ROTATE
                double sa=sin(RAD(r.x)), ca=cos(RAD(r.x));
                double sb=sin(RAD(r.y)), cb=cos(RAD(r.y));
                double sc=sin(RAD(r.z)), cc=cos(RAD(r.z));
                matrix Ap=matrix(cc,-sc,0., sc,cc,0., 0.,0.,1.)*
                         (matrix(cb,0.,sb, 0.,1.,0., -sb,0.,cb)*
                          matrix(1.,0.,0., 0.,ca,-sa, 0.,sa,ca));
                matrix B(
                        s.x*Ap.m11, s.y*Ap.m12, s.z*Ap.m13,
                        s.x*Ap.m21, s.y*Ap.m22, s.z*Ap.m23,
                        s.x*Ap.m31, s.y*Ap.m32, s.z*Ap.m33
                );
                s=vector(
                        sqrt(B.m11*B.m11+B.m12*B.m12+B.m13*B.m13),
                        sqrt(B.m21*B.m21+B.m22*B.m22+B.m23*B.m23),
                        sqrt(B.m31*B.m31+B.m32*B.m32+B.m33*B.m33)
                );
                A=matrix(
                        B.m11/s.x, B.m12/s.x, B.m13/s.x,
                        B.m21/s.y, B.m22/s.y, B.m23/s.y,
                        B.m31/s.z, B.m32/s.z, B.m33/s.z
                );
                t=Ap*t;
        }
        vector operator*(vector& v) {return s*(A*v)+t;}         // TRANSFORM
        vector operator/(vector& v) {return (A/(v-t))/s;}       // INVERSE
        vector operator<<(vector& v) {return s*(A*v);}          // ROTATE
        vector operator>>(vector& v) {return (A/v)/s;}          // INVERSE
        ray operator*(ray& r)
                {return ray((*this)*r.o,(*this)<<r.d,r.l,r.c);}
        ray operator/(ray& r)
                {return ray((*this)/r.o,(*this)>>r.d,r.l,r.c);}
        intersect operator*(intersect& i)
                {return intersect(i.t,(*this)*i.p,norm((*this)<<i.n),i.o);}
        list<intersect*> *operator*(list<intersect*> *l) {
                for(intersect* i=l->first(); i; i=l->next())
                        *i=(*this)*(*i);
                return l;
        }
        halfspace operator*(halfspace& h) {
                vector p=(*this)*(h.n*h.d); vector n=(*this)<<h.n;
                return halfspace(n,p%n);
        }
        halfspace operator/(halfspace& h) {
                vector p=(*this)/(h.n*h.d); vector n=(*this)>>h.n;
                return halfspace(n,p%n);
        }
};

//      CAMERA MODEL

class camera {
        friend ostream& operator<<(ostream& o, camera& c);
        vector l;                                       // LOCATION
        vector s;                                       // SKY
        vector d;                                       // DIRECTION
        vector u;                                       // UP
        vector r;                                       // RIGHT
        vector a;                                       // LOOK_AT
        xform T;                                        // TRANSFORMATION
public:
        camera() {
                l=vector(0.,0.,0.); s=vector(0.,1.,0.); d=vector(0.,0.,1.);
                u=vector(0.,1.,0.); r=vector(1.33,0.,0.); a=vector(0.,0.,1.);
        }
        void setl(vector& v) {l=v;}
        void sets(vector& v) {s=v;}
        void setd(vector& v) {d=v;}
        void setu(vector& v) {u=v;}
        void setr(vector& v) {r=v;}
        void seta(vector& v) {a=v;}
        void traT(vector& v) {T+=v;}
        void rotT(vector& v) {T<<=v;}
        void scaT(vector& v) {T*=v;}
        ray getray(double x, double y) {
                return this->T*ray(l,norm((r*x)+(u*y)+d),0,0);
        }
};

//      TEXTURES - I. PIGMENT

class icolor {
        unsigned char r, g, b;                          // RED, GREEN, BLUE
public:
        icolor(unsigned char r=0, unsigned char g=0, unsigned char b=0) {
                this->r=r; this->g=g; this->b=b;
        }
};

class color;

class intensity {
        friend ostream& operator<<(ostream& o, intensity& i);
        friend class color;
        double r, g, b;                                 // RED, GREEN, BLUE
public:
        intensity(double r=0., double g=0., double b=0.) {
                this->r=r; this->g=g; this->b=b;
        }
        intensity operator*(double d) {return intensity(r*d,g*d,b*d);}
        intensity operator+(intensity& i)
                {return intensity(r+i.r,g+i.g,b+i.b);}
        void operator+=(intensity& i) {r+=i.r; g+=i.g; b+=i.b;}
        operator icolor() {
                unsigned char ir=r<1.?(unsigned char)(r*255.):255;
                unsigned char ig=g<1.?(unsigned char)(g*255.):255;
                unsigned char ib=b<1.?(unsigned char)(b*255.):255;
                return icolor(ir, ig, ib);
        }
};

class color {
        friend ostream& operator<<(ostream& o, color& c);
        double r, g, b;                                 // RED, GREEN, BLUE
        double f;                                       // FILTER
public:
        color(double r=0., double g=0., double b=0., double f=0.) {
                this->r=r; this->g=g; this->b=b; this->f=f;
        }
        void setr(double d) {r=d;}
        void setg(double d) {g=d;}
        void setb(double d) {b=d;}
        void setf(double d) {f=d;}
        double filter() {return f;}
        color operator*(color& c){return color(r*c.r,g*c.g,b*c.b,f*c.f);}
        operator intensity() {return intensity(r,g,b);}
        intensity operator*(intensity& i)
                {return intensity(r*i.r,g*i.g,b*i.b);}
};

class finish;

class fog {
};

class pigment {
        friend ostream& operator<<(ostream& o, pigment& p);
protected:
        color q;                                        // QUICK COLOR
        vector t;                                       // TURBULENCE
        int o;                                          // OCTAVES
        double m;                                       // OMEGA
        double l;                                       // LAMBDA
        double f, p;                                    // FREQUENCY, PHASE
        xform T;                                        // TRANSFORMATION
public:
        virtual void out(ostream& o) {}
        pigment() {t=vector(0.,0.,0.); o=6; m=.5; l=2.; f=1.; p=0.;}
        virtual ~pigment() {}
        void operator|=(pigment& p) {
                t=p.t; o=p.o; m=p.m; l=p.l; f=p.f; this->p=p.p; T=p.T;
        }
        void setq(color& q) {this->q=q;}
        void sett(vector& t) {this->t=t;}
        void seto(int o) {this->o=o;}
        void setm(double m) {this->m=m;}
        void setl(double l) {this->l=l;}
        void setf(double f) {this->f=f;}
        void setp(double p) {this->p=p;}
        void traT(vector& v) {T+=v;}
        void rotT(vector& v) {T<<=v;}
        void scaT(vector& v) {T*=v;}
        virtual pigment* copy() {pigment *p=new pigment; *p=*this; return p;}
        virtual color paint(vector& p) {return q;}
        color surfcolor(vector& p) {return paint(T/p);}
};

class solid : public pigment {
        color c;
public:
        solid() {c=color(1.,1.,1.); setq(c);}
        solid(color& c) {this->c=c; setq(c);}
        void out(ostream& o) {o<<"color "<<c<<"\n";}
        pigment* copy() {solid *p=new solid; *p=*this; return p;}
        color paint(vector& p) {return c;}
};

class checker : public pigment {
        color c1, c2;
public:
        checker() {c1=color(0.,0.,1.,0.); c2=color(0.,1.,0.,0.);}
        checker(color& c1) {this->c1=c1; c2=color(0.,1.,0.,0.);}
        checker(color& c1, color& c2) {this->c1=c1; this->c2=c2;}
        void out(ostream& o) {o<<"checker color "<<c1<<" color "<<c2<<"\n";}
        pigment* copy() {checker *p=new checker; *p=*this; return p;}
        color paint(vector& p)
                {return (((int)p[0]+(int)p[1]+(int)p[2])%2)?c2:c1;}
};

class hexagon : public pigment {
        color c1, c2, c3;
public:
        hexagon() {
                c1=color(0.,0.,1.,0.); c2=color(0.,1.,0.,0.);
                c3=color(1.,0.,0.,0.);
        }
        hexagon(color& c1)
                {this->c1=c1; c2=color(0.,1.,0.,0.); c3=color(1.,0.,0.,0.);}
        hexagon(color& c1, color& c2)
                {this->c1=c1; this->c2=c2; c3=color(1.,0.,0.,0.);}
        hexagon(color& c1, color& c2, color& c3)
                {this->c1=c1; this->c2=c2; this->c3=c3;}
        pigment* copy() {hexagon *p=new hexagon; *p=*this; return p;}
        color paint(vector& p) {return c1;}
};

struct colormapitem {
        double b, e;                                    // BEGIN, END VALUES
        color cb, ce;                                   // BEGIN, END COLORS
        colormapitem() {b=0.; e=1.; cb=color(0.,0.,0.); ce=color(1.,1.,1.);}
        colormapitem(double b, double e, color& cb, color& ce)
                {this->b=b; this->e=e; this->cb=cb; this->ce=ce;}
};

class colormap {
        friend ostream& operator<<(ostream& o, colormap& m);
        friend class colormapped;
        colormapitem *I;
        int nI;
public:
        colormap() {I=new colormapitem; nI=1;}
        virtual ~colormap() {delete I;}
        colormap& operator=(colormap& m) {
                delete I; I=new colormapitem[m.nI];
                for(register int i=0; i<m.nI; i++) I[i]=m.I[i]; nI=m.nI;
                return *this;
        }
        void operator+=(colormapitem& J) {
                colormapitem *K=new colormapitem[nI+1];
                for(register int i=0; i<nI; i++) K[i]=I[i];
                K[nI]=J; delete I; I=K; nI=nI+1; 
        }
        colormap* copy() {colormap*p=new colormap; *p=*this; return p;}
};

class colormapped : public pigment {
        friend ostream& operator<<(ostream& o, colormapped& m);
public:
        colormap cm;
        colormapped() {}
        virtual ~colormapped() {}
        pigment* copy() {colormapped*p=new colormapped; *p=*this; return p;}
};

class gradient : public colormapped {
};

class marble : public colormapped {
};

class wood : public colormapped {
};

class onion : public colormapped {
};

class leopard : public colormapped {
};

class granite : public colormapped {
};

class bozo : public colormapped {
};

class spotted : public colormapped {
};

class agate : public colormapped {
};

class mandel : public colormapped {
};

class radial : public colormapped {
};

//      TEXTURES - II. NORMAL

class normal {
        friend ostream& operator<<(ostream& o, normal& n);
        vector t;                                       // TURBULENCE
        double f, p;                                    // FREQUENCY, PHASE
        xform T;                                        // TRANSFORMATION
public:
        virtual void out(ostream& o) {}
        normal() {t=vector(0.,0.,0.); f=1.; p=0.;}
        virtual ~normal() {}
        void operator|=(normal& n) {
                t=n.t; f=n.f; p=n.p; T=n.T;
        }
        void sett(vector& t) {this->t=t;}
        void setf(double f) {this->f=f;}
        void setp(double p) {this->p=p;}
        void traT(vector& v) {T+=v;}
        void rotT(vector& v) {T<<=v;}
        void scaT(vector& v) {T*=v;}
        virtual normal* copy() {normal *n=new normal; *n=*this; return n;}
        virtual vector perturbate(vector& n,vector& p) {return n;}
        vector surfnormal(vector& n,vector& p) {
                return norm(T<<perturbate(norm(T>>n),T*p));
        }
};

class bumps : public normal {
        double p;                                       // PERTURBATION
public:
        void out(ostream& o) {o<<"bumps "<<p<<"\n";}
        bumps() {p=.4;}
        bumps(double d) {p=d;}
        normal* copy() {bumps *n=new bumps; *n=*this; return n;}
        vector perturbate(vector& n,vector& p) {
                double theta,phi,nx,ny,nz;
                theta=acos(n.get_z())+RAD((double)rand()*this->p);
                phi=atan(n.get_y()/n.get_x())+RAD((double)rand()*this->p);
                /*double k=RAD(p%p);
                theta=acos(n.get_z())+RAD(sin(k)*this->p);
                phi=atan(n.get_y()/n.get_x())+RAD(cos(k)*this->p);*/
                nz=cos(theta);
                nx=sqrt((1-nz*nz)/(1+tan(phi)*tan(phi)));
                ny=nx*tan(phi);
                n=vector(nx,ny,nz);
                return n;
                }
};

class dents : public normal {
        double d;                                               // DENTING
public:
        void out(ostream& o) {o<<"dents "<<d<<"\n";}
        dents() {d=.4;}
        dents(double d) {this->d=d;}
        normal* copy() {dents *n=new dents; *n=*this; return n;}
        vector perturbate(vector& n,vector& p) {return n;}
};

class ripples : public normal {
};

class waves : public normal {
};

class wrinkles : public normal {
};

class bumpmap : public normal {
};

//      TEXTURES - III. FINISH

class finish {
        friend ostream& operator<<(ostream& o, finish& f);
        double kd, kb, kc;                      // DIFFUSE, BRILLIANCE, CRAND
        double ka;                              // AMBIENT
        double kr;                              // REFLECTION
        double ks, kp, kh, kg;                  // PHONG, SPECULAR HIGHLIGHTS
        int km;                                 // METALLIC
        double kt, ki;                          // REFRACTION, INDEX OF REFR.
public:
        finish() {
                kd=.6; kb=0.; kc=0.; ka=.1; kr=0.; ks=0.; kp=40.; 
                kh=0.; kg=.05; km=0; kt=0.; ki=1.;
        }
        void setkd(double d) {kd=d;}
        void setkb(double d) {kb=d;}
        void setkc(double d) {kc=d;}
        void setka(double d) {ka=d;}
        void setkr(double d) {kr=d;}
        void setks(double d) {ks=d;}
        void setkp(double d) {kp=d;}
        void setkh(double d) {kh=d;}
        void setkg(double d) {kg=d;}
        void setkm(int i) {km=i;}
        void setkt(double d) {kt=d;}
        void setki(double d) {ki=d;}
        finish* copy() {finish *f=new finish; *f=*this; return f;}
        intensity surfoptics(color& c, vector& n, ray& r, intersect& i);
};

//      TEXTURES - IV. TOP LEVEL

class texture {
        friend ostream& operator<<(ostream& o, texture& t);
        pigment *p;
        normal *n;
        finish *f;
protected:
        list<texture*> *l;
        xform T;
public:
        texture() {p=new solid;n=new normal;f=new finish;l=new list<texture*>;}
        virtual ~texture() {
                delete p; delete n; delete f;
                for(texture* t=l->first(); t; t=l->next()) delete t;
                delete l;
        }
        texture& operator=(texture& t) {
                delete p; p=t.p->copy();
                delete n; n=t.n->copy();
                delete f; f=t.f->copy();
                for(texture* x=l->first(); x; x=l->next()) delete x;
                delete l; l=new list<texture*>;
                for(texture* p=t.l->first(); p; p=t.l->next()) *l+=p->copy();
                return *this;
        }
        void operator+=(texture* t) {*l+=t;}
        virtual void out(ostream& o) {
                o<<"texture {\n";
                o<<*p; o<<*n; o<<*f;
                o<<T<<"}\n";
                for(texture* x=l->first(); x; x=l->next()) o<<*x;
        }
        void setp(pigment& p) {delete this->p; this->p=p.copy();}
        void setn(normal& n) {delete this->n; this->n=n.copy();}
        void setf(finish& f) {delete this->f; this->f=f.copy();}
        void traT(vector& v) {T+=v;}
        void rotT(vector& v) {T<<=v;}
        void scaT(vector& v) {T*=v;}
        pigment* copyp() {return p->copy();}
        normal* copyn() {return n->copy();}
        finish* copyf() {return f->copy();}
        virtual texture* copy() {texture* t=new texture; *t=*this; return t;}
        virtual intensity shade(ray& r, intersect& i) {
                i.p=T/i.p;
                color c=p->surfcolor(i.p);
                vector v=n->surfnormal(i.n, i.p);
                return f->surfoptics(c, v, r, i);
        }
};

class tiles : public texture {
public:
        void out(ostream& o) {
                o<<"texture {\n";
                o<<"tiles\n"<<*(texture*)this<<"tile2\n"<<*(l->first());
                o<<T<<"}\n";
        }
        texture* copy() {tiles* t=new tiles; *t=*this; return t;}
        intensity shade(ray& r, intersect& i)
                {return texture::shade(r,i);}
};

class materialmap : public texture {
};

//      OBJECTS IN GENERAL

class object {
protected:
        friend ostream& operator<<(ostream& o, object& p);
        texture t;                                      // TEXTURE
        xform T;                                        // TRANSFORMATION
        virtual list<intersect*> *shape(ray& r)   // RAY INTERSECTION
                {return new list<intersect*>;}
public:
        void outopt(ostream& o) {o<<t<<T;}
        virtual void out(ostream& o) {o<<"undefined {\n";outopt(o);o<<"}\n";}
        virtual ~object() {}
        void sett(texture& t) {this->t=t;}
        void setp(pigment& p) {t.setp(p);}
        void setn(normal& n) {t.setn(n);}
        void setf(finish& f) {t.setf(f);}
        void traT(vector& v) {T+=v;}
        void rotT(vector& v) {T<<=v;}
        void scaT(vector& v) {T*=v;}
        virtual object* copy() {object* o=new object; *o=*this; return o;}
        list<intersect*> *test(ray& r) {return T*shape(T/r);}
        intensity shade(ray& r, intersect& i) {return t.shade(r,i);}
        virtual list<vector*> *particles() {
                vector *v=new vector; *v=(this->T)*vector(0.,0.,0.);
                list<vector*> *l=new list<vector*>; *l+=v;
                return l;
        }
        virtual int operator&(halfspace& h) {return 1;}   // INTERSECTS h
};

//      INTERSECTION ACCELERATION TECHNIQUES

class method {
        list<object*> *lo;                              // SCENE OBJECTS
        virtual list<object*> *firstlist(ray& r)
                {return lo;}                            // BRUTE-FORCE METHOD
        virtual list<object*> *nextlist()
                {return (list<object*>*)0;}             // BRUTE-FORCE METHOD
public:
        method() {lo=new list<object*>;}
        virtual void preprocess(list<object*> *l)
                {delete lo; lo=l;}                      // NO PREPROCESSING
        intersect operator()(ray& r) {                  // GENERAL SCHEME
                intersect imin, *i;
                for(list<object*>*lo=firstlist(r); lo; lo=nextlist()) {
                        for(object *o=lo->first(); o; o=lo->next()) {
                                list<intersect*> *li=o->test(r);
                                if(i=li->first()) {
                                        if(!imin.o || i->t<imin.t )
                                                imin=*i;
                                }
                                for(i=li->first(); i; i=li->next())
                                        delete i;
                                delete li;
                        }
                }
                return imin;
        }
};

extern method* query;                                   // IN FILE main

//      OBJECTS SPECIFIED -  I. SOLID FINITE PRIMITIVES

class sphere : public object {
        vector c;                                       // CENTER
        double r;                                       // RADIUS
        list<intersect*> *shape(ray& r);
public:
        void out(ostream& o) {
                o<<"sphere {\n"<<c<<"\n"<<r<<"\n"; outopt(o); o<<"}\n";
        }
        friend ostream& operator<<(ostream& o, sphere& s);
        sphere() {c=vector(0.,0.,0.); r=1.;}
        sphere(vector& c, double r) {this->c=c;this->r=r;}
        object* copy() {sphere* o=new sphere; *o=*this; return o;}
        list<vector*> *particles() {
                vector *v=new vector; *v=T*c;
                list<vector*> *l=new list<vector*>; *l+=v;
                return l;
        }
        int operator&(halfspace& h)
                {halfspace H=T/h; return (c-H.n*(r+H.d))*H.n<=EPS;}
};

class box : public object {
};

class cone : public object {
};

class cylinder : public object {
};

class torus : public object {
};

class blob : public object {
};

//      OBJECTS SPECIFIED -  II. FINITE PATCH PRIMITIVES


//      OBJECTS SPECIFIED -  III. INFINITE SOLID PRIMITIVES

class plane : public object, public halfspace {
};

class quadric : public object {
};


//      OBJECTS SPECIFIED -  IV. CONSTRUCTIVE SOLID GEOMETRY (CSG)

class csg : public object {
        friend ostream& operator<<(ostream& o, csg& c);
protected:
        list<object*> *l;
public:
        csg(list<object*>*ol=(list<object*>*)0) {l=ol?ol:new list<object*>;}
        ~csg() {for(object* o=l->first();o;o=l->next()) delete o; delete l;}
        void outobj(ostream& o) {
                for(object *p=l->first();p;p=l->next()) o<<*p;
        }
        csg& operator=(csg& c) {
                object* o;
                for(o=l->first(); o; o=l->next()) delete o;
                delete l; l=new list<object*>;
                for(o=c.l->first(); o; o=c.l->next()) *l+=o->copy();
                return *this;
        }
        virtual object* copy() {csg* o=new csg; *o=*this; return o;}
        list<vector*> *particles() {
                list<vector*> *L=new list<vector*>;
                for(object*o=l->first();o;o=l->next()) {
                        list<vector*> *M=o->particles();
                        for(vector*v=M->first();v;v=M->next())
                                {*v=T*(*v); *L+=v;}
                        delete M;
                }
                return L;
        }
        int operator&(halfspace& h) {
                halfspace H=T/h;
                for(object*o=l->first();o;o=l->next()) if(*o&H) return 1;
                return 0;
        }
};

class csguni : public csg {
        friend ostream& operator<<(ostream& o, csguni& c);
public:
        csguni(list<object*>*ol=(list<object*>*)0) : csg(ol) {}
        ~csguni() {}
        void out(ostream& o) {
                o<<"union {\n"; outobj(o); outopt(o); o<<"}\n";
        }
        object* copy() {csguni* o=new csguni; *o=*this; return o;}
};

class csgmer : public csg {
        friend ostream& operator<<(ostream& o, csgmer& c);
public:
        csgmer(list<object*>*ol=(list<object*>*)0) : csg(ol) {}
        ~csgmer() {}
        void out(ostream& o) {
                o<<"merge {\n"; outobj(o); outopt(o); o<<"}\n";
        }
        object* copy() {csgmer* o=new csgmer; *o=*this; return o;}
};

class csgint : public csg {
        friend ostream& operator<<(ostream& o, csgint& c);
public:
        csgint(list<object*>*ol=(list<object*>*)0) : csg(ol) {}
        ~csgint() {}
        void out(ostream& o) {
                o<<"intersection {\n";outobj(o);outopt(o);o<<"}\n";
        }
        object* copy() {csgint* o=new csgint; *o=*this; return o;}
};

class csgdif : public csg {
        friend ostream& operator<<(ostream& o, csgdif& c);
public:
        csgdif(list<object*>*ol=(list<object*>*)0) : csg(ol) {}
        ~csgdif() {}
        void out(ostream& o) {
                o<<"difference {\n";outobj(o);outopt(o);o<<"}\n";
        }
        object* copy() {csgdif* o=new csgdif; *o=*this; return o;}
};


//      OBJECTS SPECIFIED -  V. LIGHT SOURCES

struct light {
        intensity i;
        vector v;
        light(intensity& i, vector& v) {this->i=i; this->v=v;}
};

class lightsource : public object {
        friend ostream& operator<<(ostream& o, lightsource& l);
protected:
        vector p;                                       // POSITION
        color c;                                        // COLOR
public:
        virtual void out(ostream& o) {
                o<<"light_source {\n"<<p<<"\n"<<c<<"\n"; o<<T; o<<"}\n";
        }
        lightsource() {p=vector(0.,0.,0.); c=color(1.,1.,1.);}
        void setp(vector& p) {this->p=p;}
        void setc(color& c)  {this->c=c;}
        object* copy() {lightsource* o=new lightsource; *o=*this; return o;}
        list<intersect*> *shape(ray& r) {return new list<intersect*>;}
        virtual list<light*>* illum(intersect& i) {
                list<light*> *L=new list<light*>;
                if((*query)(ray(this->p,norm(i.p-this->p),0,-1)).o!=i.o)
                        return L;
                light *l=new light((intensity)c,norm(this->p-i.p));
                *L+=l;
                return L;
        }
        list<vector*> *particles() {
                vector *v=new vector; *v=T*p;
                list<vector*> *l=new list<vector*>; *l+=v;
                return l;
        }
};

class spotlight : public lightsource {
        friend ostream& operator<<(ostream& o, spotlight& l);
        vector a;                                               // POINT AT
        double r;                                               // RADIUS
        double f;                                               // FALLOFF
        double t;                                               // TIGHTNESS
public:
        virtual void out(ostream& o) {
                o<<"light_source {\n"<<p<<"\n"<<c<<"\n";
                o<<"spotlight\npoint_at "<<a<<"\nradius "<<r<<"\n";
                o<<"falloff "<<f<<"\ntightness "<<t<<"\n";
                o<<T;
                o<<"}\n";
        }
        spotlight() {a=vector(0.,0.,0.); r=0.; f=0., t=10;}
        spotlight(vector a, double r, double f, double t)
                {this->a=a; this->r=r; this->f=f; this->t=t;}
        void seta(vector& a) {this->a=a;}
        void setr(double r) {this->r=r;}
        void setf(double f) {this->f=f;}
        void sett(double t) {this->t=t;}
        object* copy() {spotlight* o=new spotlight; *o=*this; return o;}
};

//      ACCELERATION TECHNIQUES - I. VIA VORONOI-DIAGRAM

struct cell;                                    // DECLARED IN voronoi.cxx

class voronoi : public method {
        cell *C;                                // CELL CLOSEST TO CENTROID
        cell **s;                               // ARRAY OF STARTING CELLS
        long traverse;                          // TRAVERSE CODE (disperse)
        void disperse(object *o, cell*C);
        cell* a; ray* r; double t;              // USED BY step()
        void step();                            // USED BY first-,nextlist()
        list<object*> *firstlist(ray& r);
        list<object*> *nextlist();
public:
        void preprocess(list<object*> *lo);
};

//      GLOBAL OBJECTS

extern int lmax;                                // MAXIMAL LEVEL OF RECURSION
extern int verbose;                             // PRINTOUTS
extern list<object*> objects;                   // SCENE OBJECTS
extern list<lightsource*> lightsources;         // LIGHTSOURCES
extern camera actcamera;                        // CAMERA
extern intensity trace(ray& r);                 // RECURSIVE RAY TRACING
extern fog actfog;                              // FOG
