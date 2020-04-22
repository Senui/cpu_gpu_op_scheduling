cmake_minimum_required(VERSION 3.0.0)
project(CpuGpuScheduling VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_library(ext_op SHARED "external_op.h")
set_target_properties(ext_op PROPERTIES LINKER_LANGUAGE CXX)

add_executable(CpuGpuScheduling main.cc)
set_target_properties(CpuGpuScheduling PROPERTIES LINKER_LANGUAGE CXX)

link_libraries(CpuGpuScheduling ext_op)
