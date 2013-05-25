#include <iostream.h>
#include <math.h>
#include "global.h"

ostream& operator<<(ostream& o, finish& f) {
        o<<"finish {\n";
        o<<"diffuse "<<f.kd<<"  "<<"brilliance "<<f.kb<<"\n";
        o<<"crand "<<f.kc<<"  "<<"ambient "<<f.ka<<"\n";
        o<<"reflection "<<f.kr<<"\n";
        o<<"phong "<<f.ks<<"  "<<"phong_size "<<f.kp<<"\n";
        o<<"specular "<<f.kh<<"  "<<"roughness "<<f.kg<<"\n";
        if(f.km) o<<"metallic\n";
        o<<"refraction "<<f.kt<<"  "<<"ior "<<f.ki<<"\n";
        o<<"}\n";
        return o;
}

extern vector norm(vector& v);

intensity finish::surfoptics(color& c, vector& n, ray& r, intersect& i){
        intensity I(this->ka, this->ka, this->ka);
        for(lightsource *ls=lightsources.first(); ls; ls=lightsources.next()) {
                list<light*>* L=ls->illum(i);
                for(light *l=L->first(); l; l=L->next()) {
                        double f=n%l->v;
                        vector h=norm(l->v+(-r.d));
                        double j=n%h;
                        double g=pow(j,this->kp);
                        if(f>0.)
                                I+=c*l->i*(this->kd*f+ this->ks*g);
                        delete l;
                }
                delete L;
        }
        if(this->kr) {
                vector d=norm(r.d-n*((n%r.d)*2));
                ray rr(i.p,d,r.l+1,2*r.c+1);
                I+=trace(rr)*this->kr ;
        }
        if(this->kt) {
                vector l=-r.d;
                double ci=l%n;
                double ct=1+(this->ki*this->ki)*(ci*ci-1);
                if(ct>=0.) {
                        vector d=norm(r.d*this->ki+n*(this->ki*ci-sqrt(ct)));
                        ray rt(i.p,d,r.l+1,2*r.c+2);
                        I+=trace(rt)*this->kt;
                }
        }
        return I;
}
