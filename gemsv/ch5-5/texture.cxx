#include <iostream.h>

#include "global.h"

ostream& operator<<(ostream& o, texture& t) {t.out(o); return o;}
