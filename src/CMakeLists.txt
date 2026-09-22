# CMakeList.txt: RandomChoser 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

cmake_minimum_required(VERSION 3.30)

# 将源代码添加到此项目的可执行文件。
add_executable (RandomChoser "RandomChoser.cpp" "RandomChoser.h")

set_property(TARGET RandomChoser PROPERTY CXX_STANDARD 23)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_link_libraries(RandomChoser
	PRIVATE
	std
)

# TODO: 如有需要，请添加测试并安装目标。
