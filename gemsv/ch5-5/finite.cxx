#include <iostream.h>
#include <math.h>

#include "global.h"

ostream& operator<<(ostream& o, sphere& s) {s.out(o); return o;}

list<intersect*> *sphere::shape(ray& r) {
        list<intersect*> *l=new list<intersect*>;
        double b=r.d%(r.o-this->c);
        double C=(r.o-this->c)%(r.o-this->c)-this->r*this->r;
        double d=b*b-C;
        if(d<0.) return l;
        d=sqrt(d); double t1=(-b-d); double t2=(-b+d);
        if(t1>EPS) {
                intersect* i=new intersect; i->t=t1; i->p=r.o+r.d*t1;
                i->n=norm(i->p-this->c); i->o=this; 
                *l+=i;
        }
        if(t2>EPS) {
                intersect* i=new intersect; i->t=t2; i->p=r.o+r.d*t2;
                i->n=norm(i->p-this->c); i->o=this; 
                *l+=i;
        }
        return l;
}
