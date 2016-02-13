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

add_library(fakeirisgl fakeirisgl.h fakeirisgl.c)

add_subdirectory(gems)
add_subdirectory(gemsiv)

