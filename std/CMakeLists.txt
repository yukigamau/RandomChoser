# CMakeLists: RandomChoser专用的std模块的项目文件

cmake_minimum_required(VERSION 3.30)

project(std)
add_library(std)

set_property(TARGET std PROPERTY CXX_STANDARD 23)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_sources(std
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		"std.ixx"
)