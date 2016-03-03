#include <iostream>

#include "global.h"

std::ostream& operator<<(std::ostream& o, texture& t) {t.out(o); return o;}
