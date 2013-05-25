//      PERMUTATION TEMPLATE (USED IN GAUSSIAN ELIMINATION)

template <int D> class PERMUTATION {
        int n[D];                                               // ELEMENTS
public:
        PERMUTATION() {for(register int i=0; i<D; i++) n[i]=i;}
        int operator[](int i) {return n[i];}
        void operator()(int i, int j) {                         // SWAP
                if(i==j) return;
                register int t=n[i]; n[i]=n[j]; n[j]=t;
        }
};

//      D-DIMENSIONAL VECTOR TEMPLATE

template <int D> class VECTOR {
        friend ostream& operator<<(ostream& o, VECTOR<D>& v);
        double x[D];                                            // COORDINATES
public:
        VECTOR() {for(register int i=0; i<D; i++) x[i]=0.;}
        VECTOR(double x[D]) {for(register int i=0; i<D; i++) this->x[i]=x[i];}
        VECTOR(double x[D], PERMUTATION<D>& p) {
                for(register int i=0; i<D; i++) this->x[i]=x[p[i]];
        }
        double operator[](int i) {return x[i];}
        void operator-=(VECTOR<D>& v) {
                for(register int i=0; i<D; i++) x[i]-=v.x[i];
        }
        VECTOR<D> operator*(double d) {
                VECTOR<D> w; for(register int i=0; i<D; i++) w.x[i]=x[i]*d;
                return w;
        }
        VECTOR<D> operator/(double d) {
                VECTOR<D> w; for(register int i=0; i<D; i++) w.x[i]=x[i]/d;
                return w;
        }
        double operator*(VECTOR<D>& v) {
                double d=0.; for(register int i=0; i<D; i++) d+=x[i]*v.x[i];
                return d;
        }
        VECTOR<D> operator+(VECTOR<D>& v) {
                VECTOR<D> w; for(register int i=0; i<D;i++) w.x[i]=x[i]+v.x[i];
                return w;
        }
        VECTOR<D> operator-(VECTOR<D>& v) {
                VECTOR<D> w; for(register int i=0; i<D;i++) w.x[i]=x[i]-v.x[i];
                return w;
        }
};

//      D-DIMENSIONAL SQUARE MATRIX TEMPLATE

template <int D> class MATRIX {
        friend ostream& operator<<(ostream& o, MATRIX<D>& A);
        VECTOR<D> a[D];                                         // ROWS
public:
        MATRIX(VECTOR<D> a[D]) {for(register int i=0;i<D;i++) this->a[i]=a[i];}
        MATRIX(double a[D][D]) {
                for(register int i=0; i<D; i++) this->a[i]=VECTOR<D>(a[i]);
        }
        VECTOR<D> operator*(VECTOR<D>& x) {
                double y[D];
                for(register int i=0; i<D; i++) y[i]=a[i]*x;
                return VECTOR<D>(y);
        }
        int operator()(VECTOR<D>& x, VECTOR<D>& b) {    // SOLVE (*this)x=b
//              GAUSSIAN ELIMINATION METHOD
                const double EPS=1e-10;
                VECTOR<D> B[D]; double c[D]; PERMUTATION<D> p; 
                register int i, j, k;
                for(i=0; i<D; i++) {B[i]=a[i]; c[i]=b[i];}      // COPY
                for(i=0; i<D; i++) {                            // THROUGH ROWS
                        double a, amax=0., e, emain;
                        for(j=i; j<D; j++)                      // MAIN ELEMENT
                                if((a=fabs(e=B[p[j]][i]))>amax)
                                        {emain=e; amax=a; k=j;}
                        if(amax<EPS) return 0;                  // SINGULAR
                        p(i,k);                                 // SWAP
                        for(j=i+1; j<D; j++) {                  // NULL BELOW
                                double s=B[p[j]][i]/emain;
                                B[p[j]]-=B[p[i]]*s;
                                c[p[j]]-=c[p[i]]*s;
                        }
                }
                for(i=D-1; i>=0; i--) {                 // BUILD SOLUTION
                        for(j=D-1; j>i; j--) c[p[i]]-=B[p[i]][j]*c[p[j]];
                        c[p[i]]/=B[p[i]][i];
                }
                x=VECTOR<D>(c,p); return 1;
        }
};

//      VORONOI-VERTEX TEMPLATE

template <int D> struct VERTEX {
        VECTOR<D>* p[D+1];                              // FORMING POINTS
        VERTEX<D>* v[D+1];                              // CONTIGUOUS VERTICES
        VECTOR<D> c;                                    // POSITION
        double rr;                                      // RADIUS SQUARE
        int b;                                          // BACK INDEX (WORK)
        int i;                                          // ACTUAL INDEX (WORK)
        long t;                                         // TRAVERSE CODE (WORK)
private:
        void initialize(VECTOR<D>* f[D+1]) {
                register int i;
                for(i=0; i<D+1; i++) {p[i]=f[i]; v[i]=(VERTEX<D>*)0;}
                this->b=-1; this->i=0; this->t=0L;
                VECTOR<D> A[D]; double b[D];
                for(i=0; i<D; i++) {
                        A[i]=(*f[i+1])-(*f[i]);
                        b[i]=(((*f[i+1])+(*f[i]))*0.5)*A[i];
                }
                if(!MATRIX<D>(A)(c,VECTOR<D>(b))) {     // EQUATION A*c=b
                        rr=-1.;                         // DEGENERATE
                        return;
                }
                rr=(*p[0]-c)*(*p[0]-c);
        }
public:
        VERTEX(VECTOR<D>* f[D+1]) {initialize(f);}      // FORMING POINTS
        VERTEX(VECTOR<D> *q, VERTEX<D> *v, int i) {     // POINT q AND RING i
                VECTOR<D> *f[D+1]; f[0]=q;
                for(register int j=1; j<D+1; j++) f[j]=v->p[(j+i)%(D+1)];
                initialize(f);
        }
};

//      VORONOI-DIAGRAM TEMPLATE

template <int D> class VORONOI {
        friend ostream& operator<<(ostream& o, VORONOI<D>& v);
        VECTOR<D> C;                                    // CENTROID
        VECTOR<D> *b[D+1], B[D+1];                      // BOUNDING SIMPLEX
        VERTEX<D> *c;                                   // CLOSEST TO CENTROID
        double ll(VECTOR<D>& v) {return v*v;}           // LENGTH SQUARE
        double dd(VECTOR<D>& v, VECTOR<D>& w) {         // DISTANCE SQUARE
                VECTOR<D> d=v-w; return d*d;
        }
        void normals(int d, double n[D+1][D]) { // NORMAL VECTORS FOR bound()
                register int i, j;
                if(d==2) {
                        n[0][0]=1.0; n[0][1]=1.0;
                        n[1][0]=-1.0; n[1][1]=1.0;
                        n[2][0]=0.0; n[2][1]=-1.0;
                        return;
                }
                normals(d-1, n);
                for(i=0; i<d; i++) n[i][d-1]=1.0;
                for(i=0; i<d-1; i++) n[d][i]=0.0;
                n[d][d-1]=-1.0;
        }
        void bound(list<VECTOR<D>*>* l);        // BUILD BOUNDING SIMPLEX
        VECTOR<D>* q;                           // ACTUAL POINT
        list<VERTEX<D>*> *ld;                   // VERTICES TO DELETE
        void find();                            // FIND A VERTEX TO DELETE
        void search();                          // FIND ALL VERTICES TO DELETE
        list<VERTEX<D>*> *ln;                   // NEW VERTICES
        void create();                          // CREATE NEW VERTICES
        int samering(VERTEX<D>*v, int iv, VERTEX<D>*w, int iw) {
                for(register int i=(iv+1)%(D+1);i!=iv;i=(i+1)%(D+1)) {
                        VECTOR<D> *p=v->p[i];
                        for(register int j=(iw+1)%(D+1);j!=iw;j=(j+1)%(D+1))
                                if(w->p[j]==p) {j=-1; break;}
                        if(j>=0) return 0;
                }
                return 1;
        }
        void link();                            // LINK NEW VERTICES TO EACH O.
        void build(list<VECTOR<D>*>* l) {       // DISJOINT PARTICLES
                traverse=0L;
                bound(l);
                for(VECTOR<D>* p=l->first(); p; p=l->next())
                        {q=p; find(); search(); create(); link();}
        }
        long traverse;                          // TRAVERSE CODE
        static void free(VERTEX<D>*v){delete v;}// FOR DESTRUCTOR
        static void donothing(VERTEX<D>*v){}    // FOR REINITIALIZE traverse
public:
        VORONOI(list<VECTOR<D>*>* l) {
                for(register int i=0; i<D+1; i++) b[i]=&B[i];
                build(l);
        }
        VORONOI(list<VECTOR<D>*>* l, VECTOR<D> *b[D+1]) {
                for(register int i=0; i<D+1; i++) this->b[i]=b[i];
                build(l);
        }
        void operator()(void (*f)(VERTEX<D>* v));       // TRAVERSE VERTICES
        ~VORONOI() {(*this)(free);}                     // TRAVERSE AND DELETE
};

template <int D> void VORONOI<D>::bound(list<VECTOR<D>*>* l) {
        register int i, j;

//      NORMAL VECTORS FOR FACES OF BOUNDING SIMPLEX

        double a[D+1][D]; normals(D, a); 
        VECTOR<D> n[D+1]; 
        for(i=0; i<D+1; i++) n[i]=VECTOR<D>(a[i])/sqrt(ll(VECTOR<D>(a[i])));

//      MAXIMAL DISTANCES IN DIRECTION OF NORMALS

        double d, dmax[D+1], dmin[D+1]; register int first=1;
        for(VECTOR<D>* p=l->first(); p; p=l->next()) {
                for(i=0; i<D+1; i++) {
                        d=n[i]*(*p);
                        if(first || d>dmax[i]) dmax[i]=d;
                        if(first || d<dmin[i]) dmin[i]=d;
                }
                first=0;
        }

//      VERTICES OF BOUNDING SIMPLEX (INTERSECT FACES CYCLICALLY)

        for(i=0; i<D+1; i++) dmax[i]+=(dmax[i]-dmin[i])*.5;     // INACCURACY
        VECTOR<D> A[D]; double t[D];
        for(i=0; i<D+1; i++) {
                for(j=0; j<D; j++) {
                        A[j]=n[(i+j)%(D+1)];
                        t[j]=dmax[(i+j)%(D+1)];
                }
                (void)MATRIX<D>(A)(*b[i],VECTOR<D>(t)); // EQUATION A*b[i]=t
        }

//      CENTRAL VERTEX AND CENTROID

        c=new VERTEX<D>(b);
        for(i=0; i<D+1; i++) C=C+(*b[i]);
        C=C*(1./(double)(D+1));
}

template <int D> void VORONOI<D>::find() {
        register int i, j;
        double P[D+1][D+1]; for(j=0; j<D+1; j++) P[D][j]=1.;
        double q[D+1]; for(j=0; j<D; j++) q[j]=(*this->q)[j]; q[D]=1.;
        VERTEX<D> *v=c;
        VECTOR<D+1> a;                          // BARICENTRIC COORDINATES OF q
        for(;;) {
                for(i=0; i<D; i++) for(j=0; j<D+1; j++) P[i][j]=(*v->p[j])[i];
                (void)MATRIX<D+1>(P)(a,VECTOR<D+1>(q));         // SOLVE P*a=q
                double aminus=0.;
                for(j=0; j<D+1; j++) if(a[j]<aminus) {aminus=a[j]; i=j;}
                if(aminus<0.) {v=v->v[i]; continue;}
                break;                                          // q INSIDE
        }
        ld=new list<VERTEX<D>*>; *ld+=v;
}

template <int D> void VORONOI<D>::search() {
        VERTEX<D> *vstart=ld->first(), *v=vstart; v->b=-1;
        register int back=0;
        for(;;) {
                register int go=0, i; VERTEX<D> *n;
                do {
                        if(back) {                      // STEP BACKWARDS
                                *ld+=v; 
                                i=v->b; v->b=-1; v=v->v[i]; back=0; continue;
                        }
                        if(v->i==v->b) continue;
                        if(v->v[v->i]==(VERTEX<D>*)0) continue;
                        n=v->v[v->i];                   // NEIGHBOR
                        if(n->b>=0) continue;           // ALREADY TRAVERSED
                        if((*ld)[n]) continue;          // ALREADY ON LIST
                        if(dd(*q,n->c)<n->rr) go=1;     // GO IF q IN SPHERE
                        if(go) break;
                } while((v->i=(v->i+1)%(D+1))!=0);
                if(go) {                                // STEP FORWARDS
                        for(i=0; i<D+1; i++)            // COMPUTE BACK INDEX
                                if(v==n->v[i]) {n->b=i; break;}
                        v=n; continue;
                }
                if(v==vstart) break;
                back=1;
        }    
}

template <int D> void VORONOI<D>::create() {
        VERTEX<D> *v;
        ln=new list<VERTEX<D>*>;
        for(v=ld->first(); v; v=ld->next()) {               // TAKE VERTICES
                for(register int i=0; i<D+1; i++) {         // TAKE NEIGHBORS
                        VERTEX<D> *n=v->v[i];
                        if((*ld)[n]) continue;              // ALSO DELETED
                        VERTEX<D> *m=new VERTEX<D>(q,v,i);  // POINT q + RING i
                        if(m->rr<0.) 
                                {delete m;*ld+=n;continue;} // DEGENERACY
                        *ln+=m;                             // STORE
                        register int j;
                        for(j=0; j<D+1; j++) 
                                if(m->p[j]==q) m->v[j]=n;   // OUTER LINK
                        if(n==(VERTEX<D>*)0) continue;      // NO REAL NEIGHBOR
                        for(j=0; j<D+1; j++) 
                                if(n->v[j]==v) n->v[j]=m;   // BACK LINK
                }
        }
        if((*ld)[c]) {                                      // NEW c NEEDED
                double d, ddmin; register int first=1;
                for(v=ln->first(); v; v=ln->next()) {
                        d=dd(v->c,C);
                        if(first || d<ddmin) {c=v; ddmin=d;}
                        first=0;
                }
        }
        for(v=ld->first(); v; v=ld->next()) {delete v;} // DELETE VERTICES
        delete ld;
}

template <int D> void VORONOI<D>::link() {
        register int i, j, n, iv, iw;
        VERTEX<D> *v, *w;
        n=0; for(v=ln->first(); v; v=ln->next()) n++;
        VERTEX<D> *N[n];
        n=0; for(v=ln->first(); v; v=ln->next()) N[n++]=v;
        for(i=0; i<n-1; i++) {
                v=N[i];
                for(j=i+1; j<n; j++) {
                        w=N[j];
                        for(iv=0; iv<D+1; iv++) {
                                for(iw=0; iw<D+1; iw++) {
                                        if(samering(v,iv,w,iw))
                                                {v->v[iv]=w; w->v[iw]=v;}
                                }
                        }
                }
        }
        delete ln;
}

template <int D> void VORONOI<D>::operator()(void (*f)(VERTEX<D>* v)) {
        traverse++;
        if(traverse==-1L) {                             // OVERFLOW
                traverse=-3L; (*this)(donothing);       // REINITIALIZE
                traverse=0L;
        } 
        VERTEX<D> *v=c; v->b=D+1;                       // PARTICULAR CASE
        register int back=0;
        for(;;) {                                       // ITERATIVE TRAVERSE
                v->t=traverse;                          // MARK AS TRAVERSED
                register int go=0;                      // DON'T GO YET
                VERTEX<D> *n;                           // ACTUAL NEIGHBOR
                do {                                    // ACTION ON ACTUAL v
                        if(back) {                      // STEP BACKWARDS
                                VERTEX<D>*n=v->v[v->b]; // FROM WHERE WE CAME
                                v->b=-1;                // FOR NEXT USAGE
                                f(v);                   // PERFORM ACTION
                                v=n; back=0; continue;  // TAKE BACK VERTEX
                        }
                        if(v->i==v->b) continue;        // DON'T STEP BACK YET
                        if(v->v[v->i]==(VERTEX<D>*)0) 
                                continue;               // NO REAL NEIGHBOR
                        n=v->v[v->i];                   // WHERE WE SHOULD GO
                        if(n->t==traverse) continue;    // ALREADY TRAVERSED
                        go=1; break;                    // GO ON
                } while((v->i=(v->i+1)%(D+1))!=0);      // UNTIL NOT ALL DONE
                if(go) {                                // STEP FORWARDS
                        for(register int i=0;i<D+1;i++) // FIND BACK LINK
                                if(v==n->v[i])
                                        {n->b=i;break;} // BOOK
                        v=n; continue;                  // LET'S GO
                }
                if(v==c) break;                         // RETURNED
                back=1;                                 // GO BACK IF NO BETTER
        }
        f(c); c->b=-1; c->t=traverse;                   // THE LAST ONE
}
