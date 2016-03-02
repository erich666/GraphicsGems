cmake_minimum_required(VERSION 3.2)

project(GraphicsGems)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_STANDARD 11)
	#add_definitions(-std=c++11)
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(.)

add_library(FakeIrisGL fakeirisgl.h fakeirisgl.c)
#add_library(GGVecLib GGVecLib.c)
add_library(GraphicsGems GraphicsGems.h GraphicsGems.c)

#add_subdirectory(gems)
add_subdirectory(gemsii)
#add_subdirectory(gemsiii)
#add_subdirectory(gemsiv)
#add_subdirectory(gemsv)

