add_hal_executable(EPS_A
    main.cpp
)

target_link_libraries(EPS_A
    IOMap
    eps
    MainTimer
    telemetry
    TelemetryUpdater
)