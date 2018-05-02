cmake_minimum_required(VERSION 3.10)
project(adps)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./output/build)

include_directories(output/local/include)
include_directories(output/dpdk/include/dpdk)
link_directories(output/local/lib)
link_directories(output/local/lib64)
link_directories(output/dpdk/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/output)


add_subdirectory(port_test)
