add_hal_executable(SunS
    main.cpp
    debug/mock.cpp
    debug/commands.h
)

target_link_libraries(SunS
    terminal
    commands
    telemetry
    ObcInterface
    logger
)
