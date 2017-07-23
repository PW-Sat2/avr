add_compile_options(-g)

add_hal_executable(SunS
    main.cpp
)

target_link_libraries(SunS
    terminal
    commands
    telemetry
    ObcInterface
    logger
)
