add_executable(euler_angle EulerAngles.c EulerAngles.h EulerSample.c QuatTypes.h)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		target_link_libraries(euler_angle m)
endif()

