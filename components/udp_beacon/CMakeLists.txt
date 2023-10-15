add_library(udp_beacon INTERFACE)

target_sources(udp_beacon INTERFACE 
      udp_beacon.c
)

target_compile_definitions(udp_beacon INTERFACE
        WIFI_SSID=\"PicoWLAN\"
        WIFI_PASSWORD=\"Pasword!123456\"
)

target_include_directories(udp_beacon INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(udp_beacon INTERFACE
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
)
