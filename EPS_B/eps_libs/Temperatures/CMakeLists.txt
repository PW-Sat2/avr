set(NAME Temperatures)

add_library(${NAME} STATIC
    Temperatures.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    eps
    TemperatureSensorsValidation
)
