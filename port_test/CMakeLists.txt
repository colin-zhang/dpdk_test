include_directories(src)

aux_source_directory(src/util uitl_files)
aux_source_directory(src/main main_files)
aux_source_directory(src/port port_files)
add_executable(port_test 
    ${port_files}
    ${uitl_files} 
    ${main_files}
)

add_executable(dpdk_mempool 
    ${port_files}
    ${uitl_files} 
   src/tools/dpdk_mempool.cc
)

add_executable(dpdk_ifconfig 
    ${port_files}
    ${uitl_files} 
   src/tools/dpdk_ifconfig.cc
)

add_executable(dpdk_dump
    ${port_files}
    ${uitl_files} 
   src/tools/dpdk_dump.cc
)

set(DPDK_DRIVERS
  "-lrte_pmd_ixgbe -lrte_pmd_vmxnet3_uio -lrte_pmd_e1000"
  )

set(DPDK_LIBS
  "-Wl,--whole-archive"
  "-ldpdk"
  "-Wl,--no-whole-archive"
  )

set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall"
  )
add_definitions(-mssse3)
add_definitions(-msse4)

target_link_libraries(port_test ${DPDK_LIBS})
target_link_libraries(port_test pthread dl m numa)

target_link_libraries(dpdk_mempool ${DPDK_LIBS})
target_link_libraries(dpdk_mempool pthread dl m numa)

target_link_libraries(dpdk_ifconfig ${DPDK_LIBS})
target_link_libraries(dpdk_ifconfig pthread dl m numa)

target_link_libraries(dpdk_dump ${DPDK_LIBS})
target_link_libraries(dpdk_dump pthread dl m numa)