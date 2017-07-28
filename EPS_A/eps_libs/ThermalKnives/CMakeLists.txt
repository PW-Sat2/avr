add_library(ThermalKnives STATIC
    ThermalKnives.h
)

target_include_directories(ThermalKnives PUBLIC .)

target_link_libraries(ThermalKnives
    IOMap
    logger
    SoftTimer
)
