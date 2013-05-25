#include <iostream.h>
#include <math.h>

#include "global.h"

ostream& operator<<(ostream& o, pigment& p) {
        o<<"pigment {\n";
        p.out(o);
        o<<"turbulence "<<p.t<<"\n"<<"octaves "<<p.o<<"\n";
        o<<"omega "<<p.m<<"\n"<<"lambda "<<p.l<<"\n";
        o<<"frequency "<<p.f<<"\n"<<"phase "<<p.p<<"\n";
        o<<"quick_color "<<p.q<<"\n"<<p.T;
        o<<"}\n";
        return o;
}

ostream& operator<<(ostream& o, colormap& m) {
        o<<"color_map{\n";
        for(register int i=0; i<m.nI; i++) {
                o<<"    ["<<m.I[i].b<<", "<<m.I[i].e;
                o<<"  "<<m.I[i].cb<<"  "<<m.I[i].ce<<"]\n";
        }
        o<<"}\n";
        return o;
}

ostream& operator<<(ostream& o, colormapped& m) {
        o<<"undefined colormapped {\n"<<m.cm<<"}\n"; return o;
}
