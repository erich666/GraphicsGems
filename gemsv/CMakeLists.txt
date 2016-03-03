add_subdirectory(ch1-1)
add_subdirectory(ch1-2)
add_subdirectory(ch1-3)
add_subdirectory(ch1-4)
add_subdirectory(ch2-2)
add_subdirectory(ch2-6)
add_subdirectory(ch2-7)
add_subdirectory(ch3-3)
add_subdirectory(ch3-4)
add_subdirectory(ch3-5)
add_subdirectory(ch3-6)
add_subdirectory(ch4-3)
add_subdirectory(ch4-4)
add_subdirectory(ch4-5)
add_subdirectory(ch4-7)
add_subdirectory(ch4-8)
add_subdirectory(ch4-9)
add_subdirectory(ch5-2)
add_subdirectory(ch5-3)
add_subdirectory(ch5-4)
add_subdirectory(ch5-5)
add_subdirectory(ch6-2)
add_subdirectory(ch6-3)
add_subdirectory(ch6-4)
add_subdirectory(ch6-5)
add_subdirectory(ch6-7)
add_subdirectory(ch7-1)
add_subdirectory(ch7-2)
add_subdirectory(ch7-3)
add_subdirectory(ch7-4)
add_subdirectory(ch7-5)
add_subdirectory(ch7-6)
add_subdirectory(ch7-7)

set_property(TARGET
	quarcube invsqrt fixsqrt rat rev conmat len4 tricubic xcoord bsp5 axd
	arcdivid aspc ellipsoid bezlen qbezier lincrv quad rayscan poly oopov
	halfadap pclipper vectorize revfit sampat wave pcube collide5 partition
	triangulation ZRendv10 xs11 tga cg4d gm vec_h

	oopov_show

	PROPERTY FOLDER "GraphicsGems V")
