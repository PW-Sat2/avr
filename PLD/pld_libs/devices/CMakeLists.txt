add_library(devices STATIC
    Include/AD7714.h
    Include/Watchdog.h
)

target_include_directories(devices PUBLIC Include)

target_link_libraries(devices
    hal
)
