cmake_minimum_required(VERSION 3.26)
set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.3.sdk)
project(RtxDemo)

include_directories(../include)

link_directories(../lib/build)

add_executable(main main.cpp)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -ggdb")
    target_link_libraries(main RtxDebug)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")
    target_link_libraries(main RtxRelease)
endif()
