#include <iostream.h>

#include "global.h"

list<object*> objects;

ostream& operator<<(ostream& o, vector& v) {
        o<<"<"<<v.x<<","<<v.y<<","<<v.z<<">"; 
        return o;
}
vector operator-(vector& v) {return vector(-v.x,-v.y,-v.z);}
ostream& operator<<(ostream& o, xform& T) {
        o<<"xform {\n";
        o<<"\t"<<"orientation {\n";
        o<<"\t"<<"\t"<<T.A.m11<<"\t"<<T.A.m12<<"\t"<<T.A.m13<<"\n";
        o<<"\t"<<"\t"<<T.A.m21<<"\t"<<T.A.m22<<"\t"<<T.A.m23<<"\n";
        o<<"\t"<<"\t"<<T.A.m31<<"\t"<<T.A.m32<<"\t"<<T.A.m33<<"\n";
        o<<"\t"<<"}\n";
        o<<"\t"<<"scale "<<T.s<<"\n";
        o<<"\t"<<"translate "<<T.t<<"\n";
        o<<"}\n";
        return o;
}
ostream& operator<<(ostream& o, color& c) {
        o<<"<red "<<c.r<<" green "<<c.g<<" blue "<<c.b<<" filter "<<c.f<<">";
        return o;
}
ostream& operator<<(ostream& o, intensity& i) {
        o<<"<red "<<i.r<<" green "<<i.g<<" blue "<<i.b<<">";
        return o;
}
ostream& operator<<(ostream& o, object& p) {p.out(o); return o;}

vector norm(vector& v) {return v*(1./sqrt(v%v));}
