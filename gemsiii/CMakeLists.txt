
add_library(3d 3d.c)
add_library(accForm accForm.c)
add_library(bitmap bitmap.c)
add_library(bounding_volumes bounding_volumes.c)
add_library(bsp bsp.c)
add_library(bzrinter bzrinter.c)
add_library(circlexc circlexc.c)
add_library(con2d con2d.c)
add_library(contour contour.c)
add_library(edgeCalc edgeCalc.c)
add_library(fastBitmap fastBitmap.c)
add_library(fastLinear fastLinear.c)
add_library(fastSpan fastSpan.c)
add_library(fillet fillet.c)
add_library(filter filter.c)
add_library(filter_rcg filter_rcg.c)
add_library(forfac forfac.c)
add_library(hemis hemis.c)
add_library(insectc insectc.c)
add_library(intell intell.c)
add_library(intqdr intqdr.c)
add_library(motblur motblur.c)
add_library(ndline ndline.c)
add_library(newell newell.c)
add_library(panorama panorama.c)
add_library(parelarc parelarc.c)
add_library(PIR PIR.c)
add_library(pl2plane pl2plane.c)
add_library(planeSets planeSets.c)
add_library(Polyintr Polyintr.c)
add_library(pt2plane pt2plane.c)
add_library(quatspin quatspin.c)
add_library(rand_rotation rand_rotation.c)
add_library(rgbvary rgbvary.c)
add_library(scallops8 scallops8.c)
add_library(sqfinal sqfinal.c)
add_library(sqrt sqrt.c)
add_library(triangleCube triangleCube.c)
add_library(urot urot.c)
add_library(zdepth zdepth.c)

add_subdirectory(accurate_scan)
add_subdirectory(alloc)
add_subdirectory(exttest)
add_subdirectory(luminaire)
add_subdirectory(partition3d)
add_subdirectory(simplex)

set_property(TARGET

	3d accForm bitmap bounding_volumes bsp bzrinter circlexc con2d contour 
	edgeCalc fastBitmap fastLinear fastSpan fillet filter filter_rcg forfac hemis insectc
	intell intqdr motblur ndline newell panorama parelarc PIR pl2plane planeSets
	Polyintr pt2plane quatspin rand_rotation rgbvary scallops8 sqfinal sqrt
	triangleCube urot zdepth

	accurate_scan accurate_exhaust accurate_test alloc exttest luminaire partition3d simplex

	PROPERTY FOLDER "GraphicsGems III")

if(MSVC)
	add_library(rgbvaryW rgbvaryW.c)
	set_property(TARGET rgbvaryW PROPERTY FOLDER "GraphicsGems III")
endif()

