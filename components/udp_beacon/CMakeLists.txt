add_library(udp_beacon INTERFACE)

target_sources(udp_beacon INTERFACE 
      udp_beacon.c
)


target_include_directories(udp_beacon INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(udp_beacon INTERFACE        
        pico_cyw43_arch_lwip_sys_freertos
        pico_lwip_iperf
        pico_stdlib
        common_cfg
)
