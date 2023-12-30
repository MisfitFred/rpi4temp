add_library(wifi_manager INTERFACE)

target_sources(wifi_manager INTERFACE 
      wifi_manager.c
)

target_compile_definitions(wifi_manager INTERFACE
        WIFI_SSID=\"FredCleverCatLuzipher\"
        WIFI_PASSWORD=\"Ddxe84Lp5YTyeN4A0jPI\"
)

target_include_directories(wifi_manager INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(wifi_manager INTERFACE
        pico_cyw43_arch_lwip_sys_freertos
        pico_lwip_iperf
        pico_stdlib
        common_cfg
)
