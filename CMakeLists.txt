cmake_minimum_required(VERSION 3.1.3)
project(dynamicsystems LANGUAGES CXX)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fopt-info-vec-all")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(dynamicsystems main.cpp)
#if(UNIX)
#	target_compile_options(dynamicsystems PRIVATE "-ftree-vectorize" "-O3" "-ffast-math" "-qopt-report=5" "-qopt-report-phase=vec")
#endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  target_compile_options(dynamicsystems PRIVATE "-std=c++11" "-qopt-report=5" "-qopt-report-phase=vec")
endif()
