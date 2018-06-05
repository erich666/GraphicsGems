add_definitions(-DSTANDALONE_TEST)
add_executable(vert_norm smooth.h smooth.c test.c)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		target_link_libraries(vert_norm m)
endif()

