cmake_minimum_required(VERSION 3.1.3)
project(dynamicsystems LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(OpenMP)
add_executable(dynamicsystems main.cpp compute.cpp)
if(OPENMP_FOUND)
  target_compile_options(dynamicsystems PRIVATE ${OpenMP_CXX_FLAGS})
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(dynamicsystems PRIVATE ${OpenMP_CXX_FLAGS})
  endif()
endif()

find_package(Boost REQUIRED COMPONENTS program_options)
target_include_directories(dynamicsystems PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(dynamicsystems PRIVATE ${Boost_LIBRARIES})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  if (WIN32)
    target_compile_options(dynamicsystems PRIVATE "/Qopt-report:5"
                                                  "/Qopt-report-phase:vec"
                                                  "/QxHost")
  else(WIN32)
    target_compile_options(dynamicsystems PRIVATE "-qopt-report=5"
                                                  "-qopt-report-phase=vec"
                                                  "-xhost")
  endif(WIN32)
endif()
