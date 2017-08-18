set(NAME ObcWatchdog)

add_library(${NAME} STATIC
    ObcWatchdog.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    logger
    SoftwareTimer
)
