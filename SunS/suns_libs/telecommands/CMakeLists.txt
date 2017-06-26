set(NAME telecommands)

add_library(${NAME} STATIC
    Include/telecommands/measure.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME}
    hal
    hardware
    telemetry
)
