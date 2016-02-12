#if(MSVC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
#endif
add_library(graph_layout defines.h fileio.C layout.C graph.C vector.C)

