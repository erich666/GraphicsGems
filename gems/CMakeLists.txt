
add_library(Albers Albers.c)
add_library(AAPolyScan AAPolyScan.c)
add_library(BoundSphere BoundSphere.c)
add_library(BoxSphere BoxSphere.c)
add_library(CircleRect CircleRect.c)
add_library(ConcaveScan ConcaveScan.c)
add_library(DigitalLine DigitalLine.c)
add_library(Dissolve Dissolve.c)
add_library(DoubleLine DoubleLine.c)
add_library(FastJitter FastJitter.c)
add_library(FixedTrig FixedTrig.c)
add_library(HSLtoRGB HSLtoRGB.c)
add_library(HypotApprox HypotApprox.c)
add_library(Interleave Interleave.c)
add_library(LineEdge LineEdge.c)
add_library(MatrixInvert MatrixInvert.c)
add_library(MatrixOrtho MatrixOrtho.c)
add_library(MatrixPost MatrixPost.c)
add_library(Median Median.c)
add_library(PixelInteger PixelInteger.c)
add_library(PntOnLine PntOnLine.c)
add_library(Quaternions Quaternions.c)
add_library(RayBox RayBox.c)
add_library(RayPolygon RayPolygon.c)
add_library(RGBTo4Bits RGBTo4Bits.c)
add_library(Roots3And4 Roots3And4.c)
add_library(SeedFill SeedFill.c)
add_library(SquareRoot SquareRoot.c)
add_library(TransBox TransBox.c)
add_library(TriPoints TriPoints.c)
add_library(ViewTrans ViewTrans.c)

add_executable(BinRec BinRec.c)
add_executable(FitCurves FitCurves.c)
add_executable(Forms Forms.c)
add_executable(Hash3D Hash3D.c)
add_executable(Label Label.c)
add_executable(NearestPoint NearestPoint.c)
add_executable(OrderDither OrderDither.c)

add_subdirectory(2DClip)
add_subdirectory(AALines)
add_subdirectory(PolyScan)
add_subdirectory(Sturm)

set_property(TARGET
	Albers AAPolyScan BoundSphere BoxSphere CircleRect ConcaveScan DigitalLine Dissolve
	DoubleLine FastJitter FixedTrig HSLtoRGB HypotApprox Interleave MatrixInvert
	MatrixOrtho MatrixPost Median PixelInteger PntOnLine Quaternions RayBox RayPolygon
	RGBTo4Bits Roots3And4 SeedFill SquareRoot TransBox TriPoints ViewTrans

	BinRec FitCurves Forms Hash3D Label LineEdge NearestPoint OrderDither

	2DClip AALines PolyScan Sturm
		
	PROPERTY FOLDER "GraphicsGems I")

target_link_libraries(Label m)
target_link_libraries(FitCurves GraphicsGems)
target_link_libraries(NearestPoint GraphicsGems)

