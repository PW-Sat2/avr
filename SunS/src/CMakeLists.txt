add_compile_options(-g)

add_hal_executable(SunS
    main.cpp
    debug/mock.cpp
    debug/commands.h
)

target_link_libraries(SunS
    terminal
    telecommands
    telemetry
    ObcInterface
    logger
)
