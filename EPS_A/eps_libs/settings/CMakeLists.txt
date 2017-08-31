set(NAME eps_specs)

add_library(${NAME} STATIC
    settings.h
    settings.cpp
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    eps
    IOMap
    MainTimer
    TelemetryUpdater
    calibration
)
