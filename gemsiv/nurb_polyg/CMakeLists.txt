add_executable(nurb_polyg drawing.h GraphicsGems.h nurbs.h FakeWindow.c GGVecLib.c Main.c NurbEval.c NurbRefine.c NurbSubdiv.c NurbUtils.c)
if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(nurb_polyg m)
endif()

