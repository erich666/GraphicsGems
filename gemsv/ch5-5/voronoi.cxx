#include <iostream.h>
#include <math.h>

#include "global.h"
#include "voronoi.h"                            // D-DIM. VORONOI TEMPLATE

struct cell : public VECTOR<3> {                // 3-DIMENSIONAL VORONOI-CELL
        vector p;                               // POSITION OF PARTICLE
        list<cell*> ln;                         // CONTIGUOUS CELLS
        list<object*> lh;                       // HOST OBJECT(S)
        list<object*> lo;                       // LIST OF INTERSECTING OBJECTS
        long t;                                 // LOCAL TRAVERSE CODE (WORK)
        cell *b;                                // BACK (voronoi::disperse)
        cell() {t=0L;}
        cell(vector& v, object *o=(object*)0) {
                double x[3]; x[0]=v[0]; x[1]=v[1]; x[2]=v[2];
                *((VECTOR<3>*)this)=VECTOR<3>(x); p=v; {if(o)lh+=o;} t=0L;
        }
        int operator&(vector& p) {              // CELL CONTAINS POINT p
                for(cell*n=ln.first();n;n=ln.next())
                        if(!(halfspace(this->p,n->p)&p)) return 0;
                return 1;
        }
        int operator&(object& o) {              // CELL INTERSECTS OBJECT o
                for(cell*n=ln.first();n;n=ln.next())
                        if(!(o&halfspace(this->p,n->p))) return 0;
                return 1;
        }
};

//      GLOBAL METHODS (NOT REALLY AN OBJECT ORIENTED APPROACH!)

static void initcells(VERTEX<3>*v) {        // VORONOI-CELLS FROM VORONOI<3>
        for(register int i=0; i<3; i++) {
                cell *ci=(cell*)(v->p[i]);
                for(register int j=i+1; j<4; j++) {
                        cell *cj=(cell*)(v->p[j]);
                        if(!ci->ln[cj]) ci->ln+=cj;
                        if(!cj->ln[ci]) cj->ln+=ci;
                }
        }
}

static int lexcmp(const void *v1, const void *v2) {
        cell *c1=*(cell**)v1; cell *c2=*(cell**)v2;
        const double EPS=1e-6;
        for(register int i=0; i<3; i++) {
                double d=(*c1)[i]-(*c2)[i];
                if(d<-EPS) return -1;
                if(d>EPS) return 1;
        }
        return 0;
}

//      METHODS OF class voronoi

void voronoi::disperse(object *o, cell *C) {
        traverse++;
        if(traverse==-1L) {                             // OVERFLOW
                traverse=-3L; disperse((object*)0,C);   // REINITIALIZE
                traverse=0L;
        } 
        cell *c=C; c->b=(cell*)0;                       // PARTICULAR CASE
        cell *n=c->ln.first();                          // ACTUAL NEIGHBOR
        register int back=0;                            // DON'T STEP BACK YET
        for(;;) {                                       // ITERATIVE TRAVERSE
                c->t=traverse;                          // MARK AS TRAVERSED
                register int go=0;                      // DON'T GO YET
                do {                                    // ACTION ON ACTUAL c
                        if(back) {                      // STEP BACKWARDS
                                cell *b=c->b;           // FROM WHERE WE CAME
                                if(o) c->lo+=o;         // PERFORM ACTION
                                c=b; back=0; continue;  // TAKE BACK CELL
                        }
                        if(n==c->b) continue;           // DON'T STEP BACK YET
                        if(n->t==traverse) continue;    // ALREADY TRAVERSED
                        if(o && !(*n&(*o))) continue;   // PARTIAL TRAVERSE
                        go=1; break;                    // GO ON
                } while(n=c->ln.next());                // UNTIL NOT ALL DONE
                if(go) {                                // STEP FORWARDS
                        n->b=c;                         // ESTABLISH BACK LINK
                        c=n; n=c->ln.first(); continue; // LET'S GO AHEAD
                }
                if(c==C) break;                         // RETURNED
                back=1;                                 // GO BACK IF NO BETTER
        }
        if(o) C->lo+=o; C->t=traverse;                  // C IS THE LAST ONE
}

void voronoi::preprocess(list<object*> *lo) {           // PREPROCESSING
        list<cell*> *lc=new list<cell*>;
        register int n=0;
        for(object* o=lo->first(); o; o=lo->next()) {           // OBJECTS
                list<vector*> *lp=o->particles();
                for(vector* p=lp->first(); p; p=lp->next()) 
                        {(*lc)+=new cell(*p,o); n++; delete p;}
                delete lp;
        }
        for(o=lightsources.first(); o; o=lightsources.next()) { // LIGHTSRC.S
                list<vector*> *lp=o->particles();
                for(vector* p=lp->first(); p; p=lp->next()) 
                        {(*lc)+=new cell(*p); n++; delete p;}
                delete lp;
        }
        (*lc)+=new cell(actcamera.getray(0.,0.).o); n++;        // EYE
        cell **C=new cell*[n];
        n=0;
        for(cell* c=lc->first(); c; c=lc->next()) C[n++]=c;
        delete lc;
        qsort((void*)C, (size_t)n, sizeof(cell*), lexcmp);  // LEXICOGRAPHIC
        list<VECTOR<3>*> *lv=new list<VECTOR<3>*>;
        (*lv)+=C[0]; cell* p=C[0];
        vector cm=C[0]->p;                              // CENTER OF MASS
        for(register int i=1; i<n; i++) {
                cm=cm+C[i]->p;
                if(lexcmp((const void**)&C[i],          // MULTIPLE PARTICLE
                          (const void**)&p)==0) {
                        for(o=C[i]->lh.first(); o; o=C[i]->lh.next())
                                p->lh+=o;
                        delete C[i];
                } else {(*lv)+=C[i]; p=C[i];}           // SINGLE (YET)
        }
        cm=cm/(double)n;                                // CENTER OF MASS
        delete C;
        VECTOR<3>*b[4]; for(i=0;i<4;i++) b[i]=new cell; // BOUNDARY CELLS
        VORONOI<3> V(lv,b);                             // VERTEX STRUCTURE
        for(i=0;i<4;i++) {                              // BOUNDARY CELLS
                cell* c=(cell*)b[i];
                c->p=vector((*c)[0],(*c)[1],(*c)[2]);   // UPDATE
        }
        V(initcells);                                   // 3D VORONOI-CELLS
        traverse=0L;                                    // INITIALIZE disperse
        double ddmin; register int first=1;             // INITIALIZE SEARCH
        for(c=(cell*)lv->first();c;c=(cell*)lv->next()) {
                for(o=c->lh.first();o;o=c->lh.next())   // BUILD OBJECT LISTS
                        disperse(o,c);                  // PARTIAL TRAVERSE
                vector u=c->p-cm; double dd=u%u;        // DISTANCE^2 FROM cm
                if(first || dd<ddmin)                   // FIND CLOSEST TO cm
                        {ddmin=dd;this->C=c;}
                first=0;
        }
        n=1<<(lmax+1);
        s=new cell*[n]; for(i=0;i<n;i++) s[i]=this->C;  // START FOR firstlist
        delete lv; // ???
}

void voronoi::step() {                                  // ONE STEP ALONG RAY r
        double tmin=0.; cell* nmin=(cell*)0;            // INITIALIZE SEARCH
        for(cell*n=a->ln.first(); n; n=a->ln.next()) {  // NEIGHBORING CELLS
                vector u=n->p-a->p;                     // NORMAL OF BISECTOR
                double d=r->d%u;                        // DENOMINATOR
                if(fabs(d)<EPS) continue;               // PARALLEL FACE
                vector v=(n->p+a->p)*.5-r->o;
                double t=(v%u)/d;                       // t AT INTERSECTION
                if(t<=this->t) continue;                // WOULD BE A BACK STEP
                if(t<EPS)                               // r->o ON BISECTOR PL.
                        if(u%r->d>0.) {tmin=EPS; nmin=n; continue;}
                        else continue;
                if(!tmin || t<tmin) {tmin=t; nmin=n;}   // CLOSER INTERSECTION
        }
        this->t=tmin; a=nmin;
}

list<object*>* voronoi::firstlist(ray& r) {
        register int i=r.c>=0?r.c:(1<<(lmax+1))-1;      // INDEX INTO s
        a=s[i];                                         // INITIALIZE step()
        if(!((*a)&r.o)) {                               // COHERENCE FAILED
                ray R(a->p,norm(r.o-a->p),0,0);         // PATH OF WALK
                this->r=&R; t=0.;                       // INITIALIZE step()
                for(step(); a; step())                  // WALK
                        if((*a)&r.o) {s[i]=a; break;}   // SUCCESS
        }
        this->r=&r; t=0.;                               // INITIALIZE step()
        return a?&a->lo:(list<object*>*)0;
}

list<object*>* voronoi::nextlist() {
        step();                                         // ONE STEP
        return a?&a->lo:(list<object*>*)0;              // SUCCESS OR FAIL
}
