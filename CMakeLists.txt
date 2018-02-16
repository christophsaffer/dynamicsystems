cmake_minimum_required(VERSION 3.9.6)
project(dynamicsystems LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(PNG REQUIRED)

add_library(dynamicsystems compute.cpp picture.cpp)
target_link_libraries(dynamicsystems PRIVATE PNG::PNG Boost::boost)
target_link_libraries(dynamicsystems PUBLIC Boost::boost)
target_compile_features(dynamicsystems PUBLIC cxx_std_11)

find_package(OpenMP)
if(OPENMP_CXX_FOUND)
  target_link_libraries(dynamicsystems PRIVATE OpenMP::OpenMP_CXX)
endif()

add_executable(dynamicsystems-cli cli.cpp)
target_link_libraries(dynamicsystems-cli PRIVATE dynamicsystems)

set(FLTK_SKIP_OPENGL TRUE)
set(FLTK_SKIP_FLUID TRUE)
find_package(FLTK)
if(FLTK_FOUND)
add_executable(dynamicsystems-gui gui.cpp)
target_link_libraries(dynamicsystems-gui PRIVATE dynamicsystems)
target_include_directories(dynamicsystems-gui PRIVATE ${FLTK_INCLUDE_DIR})
target_link_libraries(dynamicsystems-gui PRIVATE ${FLTK_LIBRARIES})
endif()


target_link_libraries(dynamicsystems-cli PRIVATE Boost::program_options
                                                 Boost::disable_autolinking
                                                 Boost::dynamic_linking)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  if (WIN32)
    target_compile_options(dynamicsystems-cli PRIVATE "/Qopt-report:5"
                                                  "/Qopt-report-phase:vec"
                                                  "/QxHost")
    target_compile_options(dynamicsystems-gui PRIVATE "/Qopt-report:5"
                                                  "/Qopt-report-phase:vec"
                                                  "/QxHost")
  else(WIN32)
    target_compile_options(dynamicsystems-cli PRIVATE "-qopt-report=5"
                                                  "-qopt-report-phase=vec"
                                                  "-xhost")
    target_compile_options(dynamicsystems-gui PRIVATE "-qopt-report=5"
                                                  "-qopt-report-phase=vec"
                                                  "-xhost")
  endif(WIN32)
endif()
