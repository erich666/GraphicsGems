# rle.h and rle_config.h from github.com/sarnold/urt

add_definitions(-DUSE_STRING_H)
add_library(AALines AALines.c AALines.h AAMain.c AATables.c FastMatMul.c LongConst.h utah.c utah.h)

