cmake_minimum_required(VERSION 3.17)
project(plot_opencl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
find_package(OpenCL REQUIRED)
include_directories( ${OpenCL_INCLUDE_DIR} )

add_executable(plot_opencl main.cpp opencl.hpp tool.cpp tool.hpp Ploter.cpp Ploter.hpp)

target_link_libraries(plot_opencl ${OpenCL_LIBRARIES})
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()