add_library(hardware STATIC
    Include/hardware/interface.h
    Include/hardware/mock.h
    Include/hardware/real.h
    Include/hardware/IOMap.h

    mock.cpp
    real.cpp
    radfet.cpp
)

target_include_directories(hardware PUBLIC Include)

target_link_libraries(hardware
    hal
    devices
    telemetry
    logger
)
