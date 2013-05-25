#include <iostream.h>

#include "global.h"


ostream& operator<<(ostream& o, lightsource& l) {l.out(o); return o;}
ostream& operator<<(ostream& o, spotlight& l) {l.out(o); return o;}

list<lightsource*> lightsources;
