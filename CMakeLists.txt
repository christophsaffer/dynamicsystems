cmake_minimum_required(VERSION 3.1.3)
project(dynamicsystems LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(OpenMP)
add_executable(dynamicsystems main.cpp compute.cpp picture.cpp)

find_package(FLTK)
find_package(OpenGL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${-lfltk_images} ${-lfltk_png}")
add_executable(gui gui.cpp)
target_link_libraries(gui ${FLTK_LIBRARIES} ${OPENGL_LIBRARIES})

if(OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
find_package(Boost REQUIRED COMPONENTS program_options)
target_include_directories(dynamicsystems PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(dynamicsystems PRIVATE ${PNG_LIBRARY} ${Boost_LIBRARIES})


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
