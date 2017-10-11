set(NAME TemperatureSensorsValidation)

add_library(${NAME} STATIC
    TemperatureSensorsValidation.cpp
    TemperatureSensorsValidation.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    calibration
    logger
)
