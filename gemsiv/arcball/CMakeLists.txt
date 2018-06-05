add_executable(arcball Ball.c Ball.h BallAux.c BallAux.h BallMath.c BallMath.h Body.c Body.h Demo.c)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_link_libraries(arcball FakeIrisGL m)
else()
    target_link_libraries(arcball FakeIrisGL)
endif()

