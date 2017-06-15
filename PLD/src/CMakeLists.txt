add_compile_options(-g)

add_hal_executable(PLD
    main.cpp
    debug/mock.cpp
    debug/commands.h
)

target_link_libraries(PLD
    terminal
    telecommands
    telemetry
    ObcInterface
    logger
)
