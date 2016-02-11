add_executable(arcball Ball.c Ball.h BallAux.c BallAux.h BallMath.c BallMath.h Body.c Body.h Demo.c ../../fakeirisgl.c)
if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(arcball m)
endif()

