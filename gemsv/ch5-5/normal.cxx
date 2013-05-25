#include <iostream.h>

#include "global.h"

ostream& operator<<(ostream& o, normal& n) {
        o<<"normal {\n";
        n.out(o);
        o<<"turbulence "<<n.t<<"\n";
        o<<"frequency "<<n.f<<"\n"<<"phase "<<n.p<<"\n"<<n.T;
        o<<"}\n";
        return o;
}

