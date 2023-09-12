cmake_minimum_required(VERSION 3.26)
set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.3.sdk)
project(RtxLibrary)

include_directories(../include)

file(GLOB LIB_SOURCES "*.cpp")


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -std=c++14")

add_library(RtxRelease STATIC ${LIB_SOURCES})
add_library(RtxDebug STATIC ${LIB_SOURCES})

