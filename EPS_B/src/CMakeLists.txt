add_hal_executable(EPS_B
    main.cpp
)

target_link_libraries(EPS_B
    IOMap
    eps
    MainTimer
    telemetry
    TelemetryUpdater
)