set(NAME MpptUpdater)

add_library(${NAME} STATIC
    MpptUpdater.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    telemetry
    calibration
)
