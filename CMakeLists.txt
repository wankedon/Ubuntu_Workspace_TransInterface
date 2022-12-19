cmake_minimum_required(VERSION 3.10)

project(TransInterface C CXX)

#set toolchain
SET(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

#设置使用c++14标准
set(CMAKE_CXX_STANDARD 14)

#find Boost headers 
set(BOOST_ROOT "/home/forlinx/Workspace/boost_1_76_0")
find_package(Boost 1.76.0)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    MESSAGE(STATUS "Using Boost ${Boost_LIB_VERSION}.")
endif()

#project directory
include_directories(/home/forlinx/Workspace/vcpkg/installed/arm64-linux/include) #vcpkg

#link_directories
LINK_DIRECTORIES(/home/forlinx/Workspace/vcpkg/installed/arm64-linux/lib)#vcpkg

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
message(STATUS ${PROJECT_BINARY_DIR})

aux_source_directory(. SRC_LIST)

#add_executable(detector ${SRC_LIST})
add_library(TransInterface ${SRC_LIST})
target_link_libraries(TransInterface pthread)
