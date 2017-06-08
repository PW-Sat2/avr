add_hal_executable(PLD
    main.cpp
    telecommands/RadFET.h
    telecommands/SunSref.h
    telecommands/PT1000.h
    telecommands/Photodiodes.h
    telecommands/HouseKeeping.h
    telemetry.h
    hw.h
    hw.cpp
)

target_link_libraries(PLD
    ObcInterface
    TelemetryBuffer
)

target_include_directories(PLD PUBLIC
    .
)
