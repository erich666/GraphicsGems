

add_library(centroid centroid.c)
add_library(clahe clahe.c)
add_executable(collide collide.c)
add_executable(convolve convolve.c)
add_library(coons_warp coons_warp.c)
add_library(dist_fast dist_fast.c)
add_library(emboss emboss.c)
add_executable(implicit implicit.c)
add_executable(interp_fast interp_fast.c)
add_library(inv_fast inv_fast.c)
add_library(ray_cyl ray_cyl.c)
add_library(sph_poly sph_poly.c)
add_library(thin_image thin_image.c)
add_library(trilerp trilerp.c)
add_library(vo_traverse vox_traverse.c)

add_subdirectory(arcball)
add_subdirectory(convex_test)
add_subdirectory(curve_isect)
add_subdirectory(data_smooth)
add_subdirectory(delaunay)
add_subdirectory(dyn_range)
add_subdirectory(euler_angle)
add_subdirectory(graph_layout)
add_subdirectory(minray)
add_subdirectory(multi_jitter)
add_subdirectory(nurb_polyg)
add_subdirectory(outcode)
add_subdirectory(polar_decomp)
add_subdirectory(ptpoly_haines)
add_subdirectory(ptpoly_weiler)
add_subdirectory(vec_mat)
add_subdirectory(vert_norm)

set_property(TARGET
	centroid clahe collide convolve coons_warp dist_fast emboss 
	implicit interp_fast inv_fast ray_cyl sph_poly thin_image trilerp vo_traverse
	arcball convex_test curve_isect data_smooth delaunay dyn_range euler_angle
	graph_layout minray multi_jitter nurb_polyg outcode xcc2d xcc4d polar_decomp
	ptpoly_haines ptpoly_weiler vec_mat ray vert_norm	
	PROPERTY FOLDER "GraphicsGems IV")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		target_link_libraries(collide m)
		target_link_libraries(implicit m)
endif()

