add_library(oopov global.h voronoi.h camera.cxx csg.cxx finish.cxx finite.cxx infinite.cxx lightsrc.cxx main.cxx misc.cxx normal.cxx pigment.cxx texture.cxx voronoi.cxx )
add_executable(oopov_show show.c)
target_link_libraries(oopov_show oopov)
