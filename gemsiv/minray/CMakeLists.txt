add_executable(minray ray.h minray.c)
if(CMAKE_COMPILER_IS_GNUCC)
		target_link_libraries(minray m)
endif()

