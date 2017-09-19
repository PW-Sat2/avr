set(NAME commands)

add_library(${NAME} STATIC
    Include/commands/measure.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME}
    hal
    hardware
    telemetry
    helpers
)
