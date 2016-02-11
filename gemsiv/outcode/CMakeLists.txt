add_library(outcode outcode2.c outcode4.c xf1.c xf2.c xf3.c xf4.c)
add_executable(xcc2d xcc2d.c)
target_link_libraries(xcc2d outcode)
add_executable(xcc4d xcc4d.c)
target_link_libraries(xcc4d outcode)

