add_hal_executable(EPS_B
    main.cpp
    eps/main.h
    eps/eps.cpp
    eps/eps.h
)

target_link_libraries(EPS_B
    logger
    hal
    telemetry
    ObcInterface
    commands
)