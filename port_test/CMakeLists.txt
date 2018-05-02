aux_source_directory(src SrcList)
add_executable(port_test ${SrcList})

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

target_link_libraries(port_test ${DPDK_LIBS})
target_link_libraries(port_test pthread dl m numa)