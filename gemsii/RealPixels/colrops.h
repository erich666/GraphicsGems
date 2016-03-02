#pragma once

#include "color.h"

void shiftcolrs(COLR* scan, int len, int adjust);
void colrs_gambs(COLR* scan, int len);
void setcolrgam(double g);
