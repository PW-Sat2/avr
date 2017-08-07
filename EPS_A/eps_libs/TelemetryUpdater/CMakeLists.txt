set(NAME TelemetryUpdater)

add_library(${NAME} STATIC
    TelemetryUpdater.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    telemetry
)
