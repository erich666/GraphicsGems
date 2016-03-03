#include <iostream>

#include "global.h"


std::ostream& operator<<(std::ostream& o, lightsource& l) {l.out(o); return o;}
std::ostream& operator<<(std::ostream& o, spotlight& l) {l.out(o); return o;}

list<lightsource*> lightsources;
