add_definitions(-DSTANDALONE_TEST)
add_executable(vert_norm smooth.h smooth.c test.c)
if(CMAKE_COMPILER_IS_GNUCC)
		target_link_libraries(vert_norm m)
endif()

