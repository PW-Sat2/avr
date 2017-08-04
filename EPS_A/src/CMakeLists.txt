add_hal_executable(EPS_A
    main.cpp
)

target_link_libraries(EPS_A
    ObcInterface
    logger
    telemetry
    commands
    IOMap
    MainTimer
)