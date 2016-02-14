#include <iostream>

#include "global.h"

std::ostream& operator<<(std::ostream& o, camera& c) {
        o<<"camera {\n";
        o<<"location "<<c.l<<"\n";
        o<<"sky "<<c.s<<"\n";
        o<<"direction "<<c.d<<"\n";
        o<<"up "<<c.u<<"\n";
        o<<"right "<<c.r<<"\n";
        o<<"look at "<<c.a<<"\n";
        o<<c.T;
        o<<"}\n";
        return o;
}

camera actcamera;
