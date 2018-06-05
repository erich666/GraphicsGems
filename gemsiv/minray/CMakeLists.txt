add_executable(minray ray.h minray.c)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		target_link_libraries(minray m)
endif()

