add_library(devices STATIC
    Include/AD7714.h
    Include/AD7714_driver.h
    Include/Watchdog.h
)

target_include_directories(devices PUBLIC Include)

target_link_libraries(devices
    hal
)
