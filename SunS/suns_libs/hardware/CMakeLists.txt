add_library(hardware STATIC
    Include/hardware/interface.h
    Include/hardware/mock.h
    Include/hardware/real.h
    Include/hardware/i2c_multiple.h
    Include/hardware/als.h

    mock.cpp
    real.cpp
)

target_include_directories(hardware PUBLIC Include)

target_link_libraries(hardware
    hal
    telemetry
    logger
)
