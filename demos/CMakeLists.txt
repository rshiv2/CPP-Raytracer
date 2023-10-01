cmake_minimum_required(VERSION 3.26)
set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk)
project(RtxDemo)

include_directories(../include)

link_directories(../lib/build)

add_executable(main main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Debug build")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -ggdb")
  target_link_libraries(main RtxDebug)
else()
  message("Release build")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")
  target_link_libraries(main RtxRelease)
endif()
