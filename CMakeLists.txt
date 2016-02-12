cmake_minimum_required(VERSION 3.2)

project(GraphicsGems)

if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-std=c++11)
endif()

add_library(fakeirisgl fakeirisgl.h fakeirisgl.c)

add_subdirectory(gemsiv)

