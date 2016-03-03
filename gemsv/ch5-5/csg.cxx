#include <iostream>

#include "global.h"

std::ostream& operator<<(std::ostream& o, csg& c) {c.out(o); return o;}
std::ostream& operator<<(std::ostream& o, csguni& c) {c.out(o); return o;}
std::ostream& operator<<(std::ostream& o, csgmer& c) {c.out(o); return o;}
std::ostream& operator<<(std::ostream& o, csgint& c) {c.out(o); return o;}
std::ostream& operator<<(std::ostream& o, csgdif& c) {c.out(o); return o;}

