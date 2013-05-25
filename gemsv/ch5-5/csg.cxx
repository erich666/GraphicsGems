#include <iostream.h>

#include "global.h"

ostream& operator<<(ostream& o, csg& c) {c.out(o); return o;}
ostream& operator<<(ostream& o, csguni& c) {c.out(o); return o;}
ostream& operator<<(ostream& o, csgmer& c) {c.out(o); return o;}
ostream& operator<<(ostream& o, csgint& c) {c.out(o); return o;}
ostream& operator<<(ostream& o, csgdif& c) {c.out(o); return o;}

