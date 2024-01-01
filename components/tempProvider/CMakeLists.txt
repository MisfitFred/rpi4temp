add_library(tempProvider INTERFACE)

target_sources(tempProvider INTERFACE 
      tempProvider.c
)


target_include_directories(tempProvider INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(tempProvider INTERFACE        
        pico_cyw43_arch_lwip_sys_freertos
        pico_lwip_iperf
        pico_stdlib
        common_cfg
)
