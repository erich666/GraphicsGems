
add_library(c_format c_format.c)
add_library(FastUpdate FastUpdate.c)
add_library(Hilbert Hilbert.c)
add_library(hot hot.c)
add_library(InterPhong InterPhong.c)
add_library(inverse inverse.c)
add_library(noise3 noise3.c)
add_library(quantizer quantizer.c)
add_library(ran_ramp ran_ramp.c)
add_library(RayCPhdron RayCPhdron.c)
add_library(rotate rotate.c)
add_library(rotate8x8 rotate8x8.c)
add_library(sparse sparse.c)
add_library(unmatrix unmatrix.c)
add_library(VoxelCache VoxelCache.c)
add_library(xlines xlines.c)

add_subdirectory(BitCounting)
add_subdirectory(dither)
add_subdirectory(intersect)
add_subdirectory(inv_cmap)
add_subdirectory(Peano)
add_subdirectory(radiosity)
add_subdirectory(RealPixels)
add_subdirectory(viewcorr)

target_link_libraries(c_format GetOpt)

set_property(TARGET

	c_format FastUpdate Hilbert hot InterPhong inverse noise3 quantizer
	ran_ramp RayCPhdron rotate rotate8x8 sparse unmatrix VoxelCache xlines

	BitCounting dither intersect inv_cmap Peano PeanoMain PeanoMapply radiosity RealPixels viewcorr

	PROPERTY FOLDER "GraphicsGems II")
