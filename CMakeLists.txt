cmake_minimum_required(VERSION 3.1.3)
project(dynamicsystems)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fopt-info-vec-all")

set(CMAKE_CXX_STANDARD 11)

add_executable(dynamicsystems main.cpp)
if(UNIX)
	target_compile_options(dynamicsystems PRIVATE "-ftree-vectorize" "-O3" "-ffast-math")
endif()
