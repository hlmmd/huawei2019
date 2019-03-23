# CMake 最低版本号要求
cmake_minimum_required(VERSION 2.8)

# 项目信息
project("CodeCraft-2019")

# include路径
include_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/include)

# 设置可执行文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

# 生成debug版本
SET(CMAKE_BUILD_TYPE "release")
if (CMAKE_BUILD_TYPE STREQUAL debug)
    add_definitions(-D_DEBUG)
endif ()
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++11")

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 指定生成目标
add_executable(CodeCraft-2019 ${DIR_SRCS})

# sh build_and_run.sh
#../../bin/CodeCraft-2019 ../../config/car.txt  ../../config/road.txt  ../../config/cross.txt answer.txt

#./CodeCraft-2019 ../../config_11/car.txt  ../../config_11/road.txt  ../../config_11/cross.txt answer.txt
#./CodeCraft-2019 ../../config_12/car.txt  ../../config_12/road.txt  ../../config_12/cross.txt answer.txt