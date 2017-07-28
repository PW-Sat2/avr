set(NAME telemetry)

add_library(${NAME} STATIC
    telemetry.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    atomic
)
