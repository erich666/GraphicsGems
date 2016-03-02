add_executable(nurb_polyg drawing.h nurbs.h FakeWindow.c Main.c NurbEval.c NurbRefine.c NurbSubdiv.c NurbUtils.c)
if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(nurb_polyg GraphicsGems m)
else()
	target_link_libraries(nurb_polyg GraphicsGems)
endif()

